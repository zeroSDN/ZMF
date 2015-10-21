/**
* @author Jonas Grunert
* @date created on 6/24/15.
*/

#include "ZmfCore.hpp"
#include "../data/ModuleHandleInternal.hpp"

namespace zmf {
    namespace core {

        ZmfCore::ZmfCore(std::shared_ptr<zmf::config::IConfigurationProvider> configProvider,
                         std::shared_ptr<AbstractModule> module,
                         std::shared_ptr<zmf::discovery::IPeerDiscoveryService> peerDiscoverySrv,
                         std::shared_ptr<zmf::messaging::IZmfMessagingService> zmqSrv,
                         std::function<void(zmf::data::ModuleUniqueId)> stoppedCallback) :
                logger_(zmf::logging::ZmfLogging::getLogger(module->getUniqueId().getString() + " # ZmfCore")),
                configProvider_(configProvider),
                stoppedCallback_(stoppedCallback),
                selfModule_(module),
                peerDiscoveryService_(peerDiscoverySrv) {

            isStarted_ = false;
            isStopped_ = true;
            moduleActive_ = false;
            stopRequested_ = false;
            disableRequested_ = false;
            enableRequested_ = false;
            zmfInstanceThreadInitialized_ = false;

            selfModuleHandle_ = std::shared_ptr<zmf::data::ModuleHandle>(
                    new zmf::data::ModuleHandleInternal(selfModule_->getUniqueId(), module->getVersion(),
                                                        module->getModuleName(), true));

            eventDispatcher_ = std::shared_ptr<zmf::core::ModuleEventDispatcher>(
                    new ModuleEventDispatcher(module->getUniqueId(), zmqSrv, configProvider, this));
        }

        ZmfCore::~ZmfCore() {
            // Stop instance if not already stopped or stopping
            if (isStarted_ && !stopRequested_) {
                logger_.information("Stopping by destructor");
                stopInstance();
            }

            // Join and dispose thread if not disposed
            disposeThread();
        }

        void ZmfCore::disposeThread() {
            if (zmfInstanceThreadInitialized_) {
                // If instance stopping in progress: Wait for instance to finish before destroying it
                this->joinExecution();
                // Free zmfInstanceThread
                zmfInstanceThread_.reset(nullptr);
            }
        }


        bool ZmfCore::startInstance(bool moduleAutoEnable, bool exitWhenEnableFail,
                                    bool peerDiscoveryWait, bool disableEqualModuleInterconnect) {

            if (isStarted_) {
                poco_warning(logger_, "Start ZMF Core: Already started - canceling Start");
                return false;
            }

            logger_.information("Starting ZMF Core");
            poco_warning(logger_, "TestVersion 1.01");

            isStopped_ = false;

            // First start ZMQ service so that it will not miss any host discovered notification
            if (!eventDispatcher_->start(selfModule_, selfModuleHandle_, configProvider_)) {
                poco_error(logger_, "Start ZMF Core: Failed to start ZMQ Service - canceling Start");
                return false;
            }
            poco_information(logger_, "Started ZMQ Service");

            // Now start peer discovery service
            if (!peerDiscoveryService_->start(configProvider_, this, selfModuleHandle_,
                                              ZmfCore::PEER_DISCOVERY_BROADCAST_FREQ, ZmfCore::PEER_DISCOVERY_UDP_PORT,
                                              peerDiscoveryWait, disableEqualModuleInterconnect)) {
                poco_error(logger_, "Start ZMF Core: Failed to start Peer Discovery Service - canceling Start");
                eventDispatcher_->stop();
                return false;
            }
            poco_information(logger_, "Started Peer Discovery Service");


            // Update and broadcast module state: Inactive now
            peerDiscoveryService_->updateSelfState(zmf::data::ModuleState::Inactive);
            peerDiscoveryService_->sendStateMulticast();

            exitWhenEnableFail_ = exitWhenEnableFail;
            enableRequested_ = moduleAutoEnable;


            // Start zmfCheckThread
            poco_information(logger_, "Starting module state check thread");
            stopRequested_ = false;
            zmfInstanceThread_.reset(new std::thread(&ZmfCore::zmfInstanceLoop, this));
            zmfInstanceThreadInitialized_ = true;

            // Start finished
            isStarted_ = true;
            poco_information(logger_, "ZMF Core started");

            return true;
        }


        void ZmfCore::stopInstance() {

            if (!isStarted_ && enableRequested_) {
                enableRequested_ = false;
                return;
            }

            if (!isStarted_) {
                poco_error(logger_, "Stop ZMF Core: Not running - canceling Stop");
                return;
            }

            poco_information(logger_, "Stopping ZMF Core");
            poco_information(logger_, "Wait for zmfCheckThread to terminate");
            stopRequested_ = true;

            if (zmfInstanceThread_->joinable()) {
                poco_trace(logger_, "ZmfCore::stopInstance: joining zmfInstanceThread_ now");
                try {
                    zmfInstanceThread_->join();
                } catch (...) {  // Catch all
                    poco_error(logger_, "Failed to join zmfInstanceThread_");
                }
                poco_trace(logger_, "ZmfCore::stopInstance: joined zmfInstanceThread_");
            }
        }


        bool ZmfCore::enableModule(bool checkDependencies) {

            // TODO Atomic compare/exchange
            if (moduleActive_) {
                poco_error(logger_, "EnableModule: Module already started - cancel enable");
                return false;
            }

            if (checkDependencies && !checkDependenciesSatisfied()) {
                poco_error(logger_, "EnableModule: Dependencies not satisfied - cancel enable");
                return false;
            }

            eventDispatcher_->onEnable();

            selfModule_->INTERNAL_getInternalMutex().lock();
            bool enableSuccess = selfModule_->INTERNAL_internalEnable(this);
            selfModule_->INTERNAL_getInternalMutex().unlock();
            if (enableSuccess) {
                moduleActive_ = true;
                // Update state and send broadcast
                peerDiscoveryService_->updateSelfState(zmf::data::ModuleState::Active);
                poco_debug(logger_, "Module active now");
                peerDiscoveryService_->sendStateMulticast();
                return true;
            }
            else {
                poco_error(logger_, "EnableModule: Failed to enable module - cancel enable");
                eventDispatcher_->onDisable();
            }

            return false;
        }

        void ZmfCore::disableModule() {

            // TODO Atomic compare/exchange
            if (!moduleActive_) {
                poco_error(logger_, "EnableModule: Module already stopped - cancel disable");
                return;
            }

            // Disable module
            moduleActive_ = false;
            selfModule_->INTERNAL_getInternalMutex().lock();
            selfModule_->INTERNAL_internalDisable();
            selfModule_->INTERNAL_getInternalMutex().unlock();

            // Stop event dispatcher
            eventDispatcher_->onDisable();

            // Update state and send broadcast
            peerDiscoveryService_->updateSelfState(zmf::data::ModuleState::Inactive);
            poco_debug(logger_, "Module inactive now");
            peerDiscoveryService_->sendStateMulticast();
        }


        bool ZmfCore::checkDependenciesSatisfied() {
            std::vector<ModuleDependency> moduleDependencies = selfModule_->getDependencies();
            for (const ModuleDependency& moduleDependency : moduleDependencies) {
                if (!peerDiscoveryService_->getPeerRegistry()->containsPeerWithType(moduleDependency.ModuleTypeId,
                                                                                    true)) {
                    return false;
                }
            }
            //  now we know that there is at least one active module of each dependency
            return true;


        }


        // -------------------- State Update -------------------- \\

        void ZmfCore::zmfInstanceLoop() {

            poco_information(logger_, "zmfInstanceLoop started");

            // Loop for checking and updating state of the module
            while (!stopRequested_) {
                if (moduleActive_) {
                    // Module active
                    if (disableRequested_) {
                        // Disable module
                        disableRequested_ = false;
                        disableModule();
                    } else {
                        // Check dependencies
                        if (!checkDependenciesSatisfied()) {
                            // Disable when dependencies not satisfied anymore
                            disableModule();
                            // But re-enable as soon as possible
                            enableRequested_ = true;
                        } else if (!moduleIsUnique()) {
                            logger_.fatal("Module not unique - other module with same ID found. Stopping instance");
                            stopRequested_ = true;
                        }
                    }
                }
                else {
                    // Module inactive
                    if (enableRequested_ && checkDependenciesSatisfied() && moduleIsUnique()) {
                        // Try to enable module
                        enableRequested_ = false;
                        logger_.information("Initiating enable module");
                        bool enableSuccess = enableModule(false);
                        if (!enableSuccess && exitWhenEnableFail_) {
                            // Exit when exitWhenEnableFail_ and enable failed
                            stopRequested_ = true;
                        }
                    }
                }

                // Sleep for defined interval
                int checkInterval = ZmfCore::STATE_CHECK_INTERVAL; // Workaround to make it compile
                std::this_thread::sleep_for(std::chrono::milliseconds(checkInterval));
            }

            poco_information(logger_, "zmfInstanceLoop finished");

            onInstanceStopped();
        }

        // Called when zmfInstanceThread finished
        void ZmfCore::onInstanceStopped() {

            if (!isStarted_) {
                poco_error(logger_, "ZmfCore::onInstanceStopped: Instance not started or already stopped.");
                return;
            }

            // Disable module if still enabled - we are done now
            if (moduleActive_) {
                disableModule();
            }

            // Callback to notify that this instance is stopped, eg. to a ZmfMock
            stoppedCallback_(this->selfModuleHandle_->UniqueId);

            isStarted_ = false;

            // Update state and send broadcast
            peerDiscoveryService_->updateSelfState(zmf::data::ModuleState::Dead);
            poco_debug(logger_, "Instance dead now");
            peerDiscoveryService_->sendStateMulticast();

            // Stop services
            peerDiscoveryService_->stop();
            eventDispatcher_->stop();

            isStopped_ = true;

            poco_information(logger_, "Instance stopped");
        }


        // -------------------- ZMF Instance Public Commands -------------------- \\

        void ZmfCore::requestEnableModule() {
            // TODO Atomic compare/exchange
            if (moduleActive_) {
                poco_error(logger_, "RequestEnableModule: Module already enabled - ignoring request");
                return;
            }
            disableRequested_ = false;
            enableRequested_ = true;
        }

        void ZmfCore::requestDisableModule() {
            // TODO Atomic compare/exchange
            if (!moduleActive_ && !enableRequested_) {
                poco_error(logger_, "RequestDisableModule: Module not enabled - ignoring request");
                return;
            }
            enableRequested_ = false;
            disableRequested_ = true;
        }

        void ZmfCore::requestStopInstance() {
            // TODO Atomic compare/exchange
            if (!isStarted_ && !enableRequested_) {
                poco_error(logger_, "RequestShutdownModule: Module not started - ignoring request");
                return;
            }
            enableRequested_ = false;
            stopRequested_ = true;
        }

        void ZmfCore::joinExecution() {
            if (!zmfInstanceThreadInitialized_) {
                poco_error(logger_, "JoinExecution: Module thread not started - nothing to join");
                return;
            }
            if (zmfInstanceThread_->joinable()) {
                poco_trace(logger_, "JoinExecution: joining zmfInstanceThread now");
                try {
                    zmfInstanceThread_->join();
                } catch (...) {  // Catch all
                    poco_error(logger_, "Failed to join zmfInstanceThread");
                }
                poco_trace(logger_, "JoinExecution: joined zmfInstanceThread");
            }
        }


        // -------------------- Peer Discovery -------------------- \\

        void ZmfCore::peerStateChange(std::shared_ptr<zmf::data::ModuleHandle> module,
                                      zmf::data::ModuleState newState,
                                      zmf::data::ModuleState lastState) {
            eventDispatcher_->onPeerChange(module, newState, lastState);
        }

        void ZmfCore::onModuleAdditionalStateChanged(std::vector<uint8_t> additionalState) {
            peerDiscoveryService_->updateSelfAdditionalState(additionalState);
            // No force broadcast when additional state changed
        }

        void ZmfCore::forceStateBroadcast() {
            peerDiscoveryService_->sendStateMulticast();
        }


        // -------------------- ZMQ -------------------- \\

        zmf::data::ZmfInReply ZmfCore::sendRequest(const zmf::data::ModuleUniqueId& target,
                                                   const zmf::data::ZmfMessage& msg) {
            return eventDispatcher_->sendRequest(target, msg);
        }

        IZmfInstanceAccess::SubscriptionHandle ZmfCore::subscribe(const zmf::data::MessageType& topic,
                                                                  std::function<void(const zmf::data::ZmfMessage& msg,
                                                                                     const zmf::data::ModuleUniqueId& sender)> handler) {
            return eventDispatcher_->subscribe(topic, handler);
        }

        void ZmfCore::publish(const zmf::data::ZmfMessage& msg) {
            eventDispatcher_->publish(msg);
        }

        bool ZmfCore::moduleIsUnique() {
            zmf::data::ModuleUniqueId uniqueId = selfModule_->getUniqueId();
            return !peerDiscoveryService_->getPeerRegistry()->containsPeerWithId(uniqueId, false);
        }

        std::shared_ptr<AbstractModule> ZmfCore::getModule() {
            return this->selfModule_;
        }

        bool ZmfCore::isStarted() {
            return isStarted_;
        }

        bool ZmfCore::isStopped() {
            return isStopped_;
        }

        bool ZmfCore::requestEnableRemoteInstance(data::ModuleUniqueId id, long timeout) {
            auto reply = eventDispatcher_->sendRequest(id, zmf::data::ZmfMessage("\x03\xff", "\x03"));

            auto ready = reply.wait_for(std::chrono::milliseconds(timeout));
            return ready == std::future_status::ready && 0x03 == reply.get().getData()[0];
        }

        bool ZmfCore::requestDisableRemoteInstance(data::ModuleUniqueId id, long timeout) {
            auto reply = eventDispatcher_->sendRequest(id, zmf::data::ZmfMessage("\x03\xff", "\x01"));

            auto ready = reply.wait_for(std::chrono::milliseconds(timeout));
            return ready == std::future_status::ready && 0x01 == reply.get().getData()[0];
        }

        bool ZmfCore::requestStopRemoteInstance(data::ModuleUniqueId id, long timeout) {
            auto reply = eventDispatcher_->sendRequest(id, zmf::data::ZmfMessage("\x03\xff", "\x02"));

            auto ready = reply.wait_for(std::chrono::milliseconds(timeout));
            return ready == std::future_status::ready && 0x02 == reply.get().getData()[0];
        }


    }
}