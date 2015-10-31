#ifndef ZMF_ZMFCORE_H
#define ZMF_ZMFCORE_H

#include <thread>
#include <map>
#include <list>
#include <Poco/Logger.h>
#include <Poco/FileChannel.h>
#include <Poco/ConsoleChannel.h>
#include <Poco/SplitterChannel.h>
#include <Poco/FormattingChannel.h>
#include <Poco/PatternFormatter.h>
#include <AbstractModule.hpp>
#include <IZmfInstanceAccess.hpp>
#include <IZmfInstanceController.hpp>
#include "../discovery/IPeerDiscoveryService.hpp"
#include <IConfigurationProvider.hpp>
#include "ModuleEventDispatcher.hpp"
#include "../messaging/IZmfMessagingService.hpp"


namespace zmf {
    namespace core {
        /**
         * @brief Core of a Zmf instance execution managing services and the attatched module.
         * @details Manages the ZMF execution and the module lifecycle.
         * Connects the module to execute, messaging and peer discovery.
         * Initiates module enabling, disabling or instance stopping.
         * Needs a module reference, a peer discovery service and a messaging service.
         * Also manages module dependency management.
         * Via the interface IZmfInstanceAccess it allows modules to interact with ZMF.
         * Via the interface IZmfInstanceController it allows external control of the ZMF.
         * @author Jonas Grunert
         * @date created on 6/24/15.
        */
        class ZmfCore : public zmf::IZmfInstanceAccess, public zmf::IZmfInstanceController {

        public:
            /**
             * Constructor of the Core class, initializing flags and basic variables
             * @param configProvider Configuration service providing configuration values
             * @param module Module to be managed by this core
             * @param peerDiscoveryService Peer discovery service detecting other peers on the ZMF bus
             * @param zmqSrv ZMQ messaging service for communicating over the ZMF/ZMQ bus
             */
            ZmfCore(std::shared_ptr<zmf::config::IConfigurationProvider> configProvider,
                    std::shared_ptr<AbstractModule> module,
                    std::shared_ptr<zmf::discovery::IPeerDiscoveryService> peerDiscoveryServ,
                    std::shared_ptr<zmf::messaging::IZmfMessagingService> zmqServ,
                    std::function<void(zmf::data::ModuleUniqueId)> stoppedCallback = [](zmf::data::ModuleUniqueId) { });


            virtual ~ZmfCore();

            /**
             * Starts the ZMF instance: peer discovery service and messaging service.
             * @param trackModuleStates Peer discovery Flag to enable/disable tracking of module states. If disabled only module leave and join will be tracked
             * @param moduleAutoEnable If is true a module enable will be tried after intialization
             * @param stopWhenEnableFail If true and enabling fails stop will be called
             */
            bool startInstance(bool moduleAutoEnable,
                               bool exitWhenEnableFail,
                               bool peerDiscoveryWait,
                               bool disableEqualModuleInterconnect);

            /**
             * Stops the ZMF instance: module, peer discovery service and messaging service.
             * Waits until stopped
             */
            virtual void stopInstance();


            virtual void requestEnableModule() override;

            virtual void requestDisableModule() override;

            virtual void requestStopInstance() override;

            virtual bool requestEnableRemoteInstance(zmf::data::ModuleUniqueId, long timeout) override;

            virtual bool requestDisableRemoteInstance(zmf::data::ModuleUniqueId, long timeout) override;

            virtual bool requestStopRemoteInstance(zmf::data::ModuleUniqueId, long timeout) override;

            virtual void joinExecution();



            // -------------------- Module PeerDiscovery Access -------------------- \\

            /**
             * @return The public interface of the PeerRegistry
             */
            virtual inline const std::shared_ptr<zmf::discovery::PeerRegistry>& getPeerRegistry() {
                return peerDiscoveryService_->getPeerRegistry();
            }

            // -------------------- Module ZMQ Access -------------------- \\


            virtual zmf::data::ZmfInReply sendRequest(const zmf::data::ModuleUniqueId& target,
                                                      const zmf::data::ZmfMessage& msg);

            virtual IZmfInstanceAccess::SubscriptionHandle subscribe(const zmf::data::MessageType& topic,
                                                                     std::function<void(
                                                                             const zmf::data::ZmfMessage& msg,
                                                                             const zmf::data::ModuleUniqueId& sender)> handler);

            virtual void publish(const zmf::data::ZmfMessage& msg);

            virtual void onModuleAdditionalStateChanged(std::vector<uint8_t> additionalState);

            virtual void forceStateBroadcast();


            /**
             * Returns the controlled module
             */
            virtual std::shared_ptr<AbstractModule> getModule();

            /**
             * Returns if zmf instance is started and no shutdown in progress
             */
            virtual bool isStarted();

            /**
             * Returns if instance not started or shutdown finished and no starting in progress
             */
            virtual bool isStopped();


        private:
            /*
             * broadcastFrequency Peer discovery Frequency to send out state broadcasts (in milliseconds between broadcasts)
             */
            static const int PEER_DISCOVERY_BROADCAST_FREQ = 1000;
            /*
             * udpPort Peer discovery UDP port for state broadcast communication
             */
            static const int PEER_DISCOVERY_UDP_PORT = 4213;

            /** Interval to check state and if all dependencies satisfied (in ms) */
            static const int STATE_CHECK_INTERVAL = 100;

            Poco::Logger& logger_;

            /// Configuration provider providing module configuration
            std::shared_ptr<zmf::config::IConfigurationProvider> configProvider_;

            std::function<void(zmf::data::ModuleUniqueId)> stoppedCallback_;

            /// Returns configuration provider providing module configuration
            inline virtual const std::shared_ptr<zmf::config::IConfigurationProvider>& getConfigurationProvider() { return configProvider_; }

            /** Module to be operated by this instance */
            std::shared_ptr<AbstractModule> selfModule_;
            /** Module state handle of this instances module */
            std::shared_ptr<zmf::data::ModuleHandle> selfModuleHandle_;

            /** Service to discover other modules */
            std::shared_ptr<zmf::discovery::IPeerDiscoveryService> peerDiscoveryService_;
            /** Service operating the message bus */
            std::shared_ptr<zmf::core::ModuleEventDispatcher> eventDispatcher_;


            bool exitWhenEnableFail_ = false;

            // TODO Atomic compare/exchange for flags
            /// Indicates if zmf instance is started and no shutdown in progress
            std::atomic_bool isStarted_;
            /// Indicates if instance not started or shutdown finished and no starting in progress
            std::atomic_bool isStopped_;
            std::atomic_bool moduleActive_;
            std::atomic_bool stopRequested_;
            std::atomic_bool disableRequested_;
            std::atomic_bool enableRequested_;

            /// Indicates if zmfInstanceThread initialized and not disposed
            std::atomic_bool zmfInstanceThreadInitialized_;
            std::unique_ptr<std::thread> zmfInstanceThread_;

            void disposeThread();


            void zmfInstanceLoop();

            void onInstanceStopped();

            /**
             * Tries to enable the module controlled by this instance controller
             * @param checkDependencies Only enables module if all dependencies satisfied
             */
            virtual bool enableModule(bool checkDependencies);

            virtual void disableModule();

            /**
             * Checks if all required module dependencies are present
             */
            bool checkDependenciesSatisfied();

            /**
             * Checks if the module allready exists in the PeerDiscovery
             */
            bool moduleIsUnique();

        };
    }
}


#endif //ZMF_ZMFCORE_H
