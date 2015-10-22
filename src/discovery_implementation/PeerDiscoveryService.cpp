//
// Created by Matthias Blohm on 6/25/15.
//


#include "PeerDiscoveryService.hpp"
#include "IConfigurationProvider.hpp"


namespace zmf {
    namespace discovery {

        const std::string PeerDiscoveryService::MULTICAST_ADDRESS = "239.255.255.250";

        PeerDiscoveryService::PeerDiscoveryService(zmf::data::ModuleUniqueId instanceId) :
                logger(zmf::logging::ZmfLogging::getLogger(
                        instanceId.getString() + " # PeerDiscoveryService")),
                peerRegistry_(
                        std::shared_ptr<zmf::discovery::PeerRegistry>(new zmf::discovery::PeerRegistryInternal())) {
            isStarted_ = false;
            stopRequested_ = false;
            threadInitialized_ = false;


            selfState_ = zmf::data::ModuleState::Dead;
        }

        PeerDiscoveryService::~PeerDiscoveryService() {
            // Stop service if not already stopped or stopping
            if (isStarted_ && !stopRequested_) {
                stop();
            }

            // If service stopping in progress: Wait for service to finish before destroying it
            if (isStarted_ && stopRequested_) {
                joinThread();
            }
            disposeThread();
        }


        bool PeerDiscoveryService::start(std::shared_ptr<config::IConfigurationProvider> config,
                                         IPeerDiscoveryCoreInterface* const core,
                                         std::shared_ptr<zmf::data::ModuleHandle> selfHandle,
                                         uint32_t multicastFrequency, uint16_t udpPort, bool peerDiscoveryWait,
                                         bool disableEqualModuleInterconnect) {
            if (isStarted_) {
                logger.error("PeerDiscoveryService::start: Already started - cancel start");
                return false;
            }
            try {
                this->core_ = core;
                this->multicastFrequency_ = multicastFrequency;
                this->peerTimeout_ = multicastFrequency * PEER_TIMEOUT_FACTOR;
                this->udpPort_ = udpPort;
                this->selfHandle_ = selfHandle;
                this->disableEqualModuleInterconnect_ = disableEqualModuleInterconnect;

                // Identifier for distinguishing two module instances with the same uniqueId
                Poco::Random random;
                random.seed();
                this->multicastIdentifier_ = random.next();

                /// source-address for receiving UDP-packets
                Poco::Net::SocketAddress ownAdress(Poco::Net::IPAddress(),
                                                   this->udpPort_);

                /// destination-address for sending multicasts
                this->multicastAdress_ = Poco::Net::SocketAddress(this->MULTICAST_ADDRESS, this->udpPort_);


                auto ip_ver = Poco::Net::IPAddress::Family::IPv4;

                std::string ifaceName;
                if (config->getAsString("ZMF_NETWORK_INTERFACE_NAME", ifaceName) && ifaceName.size() != 0) {
                    logger.debug("found config entry for network iface: " + ifaceName);
                    this->multicastInterface_ = zmf::util::NetworkUtil::getNetworkInterface(ifaceName, ip_ver);
                } else {
                    logger.debug("did not find config for network iface, will try to use default ");
                    this->multicastInterface_ = zmf::util::NetworkUtil::getNetworkInterface(ip_ver);
                }

                /// preparing the UDP-Socket for multicasts
                this->UdpMulticastSocket_ = Poco::Net::MulticastSocket(ownAdress, true);
                this->UdpMulticastSocket_.joinGroup(this->multicastAdress_.host(), multicastInterface_);
                this->UdpMulticastSocket_.setReusePort(true);

                this->stopRequested_ = false;

                // initializing and starting the thread sending and receiving
                this->multicastThread_.reset(new std::thread(&PeerDiscoveryService::multicastLoop, this));
                threadInitialized_ = true;

                this->isStarted_ = true;

                logger.information("PeerDiscoveryService started");
                if (peerDiscoveryWait) {
                    logger.information("will wait to discover peers");
                    std::this_thread::sleep_for(std::chrono::milliseconds(2 * multicastFrequency));
                    logger.information("wait complete");
                }


                return true;

            } catch (Poco::Exception e) {
                logger.error("PeerDiscoveryService::start: ERROR due to Poco:Exception:" + e.displayText());
                return false;
            } catch (std::exception e) {
                std::string message = "PeerDiscoveryService::start: ERROR due to Exception: ";
                message.append(e.what());
                logger.error(message);
                return false;
            }
        }

        void PeerDiscoveryService::stop() {
            if (!isStarted_) {
                logger.warning("PeerDiscoveryService::stop: Not started - cancel stop");
                return;
            }

            // Initiate stopping and send notifications
            stopRequested_ = true;

            joinThread();

            try {
                this->UdpMulticastSocket_.leaveGroup(this->multicastAdress_.host(), multicastInterface_);
            } catch (const Poco::Exception& e) {
                logger.information("failed to leave group: " + e.displayText());
            }

            try {
                this->UdpMulticastSocket_.close();
            } catch (const Poco::Exception& e) {
                logger.information("failed to close socket: " + e.displayText());
            }

            this->isStarted_ = false;
            logger.information("PeerDiscoveryService stopped");
        }

        void PeerDiscoveryService::multicastLoop() {
            logger.information("PeerDiscoveryService multicastLoop started");

            std::chrono::time_point<std::chrono::_V2::steady_clock> last_send = std::chrono::_V2::steady_clock::now();

            sendStateMulticast();

            while (!this->stopRequested_) {

                auto time_diff = std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::_V2::steady_clock::now() - last_send).count();


                if (time_diff > multicastFrequency_) {
                    sendStateMulticast();
                    updatePeerTimeouts();
                    last_send = std::chrono::_V2::steady_clock::now();
                }

                receiveMulticasts();
            }

            logger.information("PeerDiscoveryService multicastLoop stopped");
        }


        void PeerDiscoveryService::updatePeerTimeouts() {
            zmf::discovery::PeerRegistryInternal* peerRegistryInternal =
                    static_cast<zmf::discovery::PeerRegistryInternal*>(peerRegistry_.get());
            peerRegistryInternal->getPeerRegistryLock().lock();
            // Increment and check peer timeouts
            std::map<zmf::data::ModuleUniqueId, std::shared_ptr<zmf::data::ModuleHandle>> allPeers =
                    peerRegistryInternal->INTERNAL_getAllPeers(false);
            std::list<std::shared_ptr<zmf::data::ModuleHandle>> timedOutModules;
            // Update and check timeouts
            for (std::pair<zmf::data::ModuleUniqueId, std::shared_ptr<zmf::data::ModuleHandle>> peer : allPeers) {
                static_cast<zmf::data::ModuleHandleInternal*>(peer.second.get())->PeerTimeout += multicastFrequency_;
                if (static_cast<zmf::data::ModuleHandleInternal*>(peer.second.get())->PeerTimeout > peerTimeout_) {
                    // Peer Timeout
                    timedOutModules.push_back(peer.second);
                }
            }
            peerRegistryInternal->getPeerRegistryLock().unlock();
            // Handle timeouts
            for (std::shared_ptr<zmf::data::ModuleHandle> timedOutPeer : timedOutModules) {
                poco_debug(logger, "Module-" + timedOutPeer->UniqueId.getString() + " timed out");
                onPeerDead(timedOutPeer);
            }
        }

        void PeerDiscoveryService::receiveMulticasts() {
            Poco::Timespan timeout(0, 100 * 1000); // 100ms
            char buffer[2048];

            try {
                bool hasInput = UdpMulticastSocket_.poll(timeout, Poco::Net::Socket::SELECT_READ);

                if (hasInput) {
                    Poco::Net::SocketAddress senderAddress;
                    int receivedCount = this->UdpMulticastSocket_.receiveFrom(buffer, sizeof(buffer),
                                                                              senderAddress);
                    zmf::proto::StateBroadcast stateBroadcast;
                    stateBroadcast.ParseFromArray(buffer, receivedCount);
                    processIncomingMulticast(stateBroadcast, senderAddress.host().toString());
                }
            } catch (const Poco::Exception& e) {
                logger.error(e.displayText());
                std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Wait to avoid poll going cracy
            } catch (const std::exception& e) {
                logger.error(e.what());
                std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Wait to avoid poll going cracy
            } catch (...) {
                logger.error("receiveMulticasts - unknown exception");
                std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Wait to avoid poll going cracy
            }
        }

        void PeerDiscoveryService::processIncomingMulticast(zmf::proto::StateBroadcast stateBroadcast,
                                                            std::string ip) {

            if (!isStarted_ || stopRequested_) {
                return;
            }

            zmf::data::ModuleUniqueId peerId = zmf::data::ModuleUniqueId((uint16_t) stateBroadcast.senderid().typeid_(),
                                                                         stateBroadcast.senderid().instanceid());
            uint32_t multicastId = stateBroadcast.multicastidentifier();

            // check if received multicast of own module
            if (multicastId == this->multicastIdentifier_ && peerId == this->selfHandle_->UniqueId) {
                return;
            }

            // check if disableEqualModuleInterconnect_ is set and if this is the case, discard any msg from peers with same module id
            if (this->disableEqualModuleInterconnect_ && peerId.TypeId == this->selfHandle_->UniqueId.TypeId) {
                logger.debug("ignoring peer with same module type");
                return;
            }


            zmf::data::ModuleState peerState = (zmf::data::ModuleState) stateBroadcast.lifecyclestate();

            if (peerState == zmf::data::ModuleState::Dead) {
                std::shared_ptr<zmf::data::ModuleHandle> peerHandle = peerRegistry_->getPeerWithId(peerId, false);
                if (peerHandle.get() != nullptr) {
                    // Registered module is dead now - update, remove and notify core
                    poco_debug(logger, "Module-" + peerHandle->UniqueId.getString() + " dead message received");
                    onPeerDead(peerHandle);
                }
            }
            else {
                std::string additionalStateInfo = "";
                if (stateBroadcast.has_additionalstateinfos()) {
                    additionalStateInfo = stateBroadcast.additionalstateinfos();
                }

                std::vector<uint8_t> addStateVec(additionalStateInfo.begin(), additionalStateInfo.end());
                std::shared_ptr<zmf::data::ModuleHandle> peerHandle = peerRegistry_->getPeerWithId(peerId, false);
                if (peerHandle.get() == nullptr) {
                    // Add newly recognized module
                    assert(ip != "127.0.0.1");

                    std::string pubAddr = "tcp://" + ip + ":" + std::to_string(stateBroadcast.zmqpubport());
                    std::string repAddr = "tcp://" + ip + ":" + std::to_string(stateBroadcast.zmqrepport());

                    // Add newly recognized module
                    peerHandle = std::shared_ptr<zmf::data::ModuleHandle>(
                            new zmf::data::ModuleHandleInternal(peerId, (uint16_t) stateBroadcast.version(),
                                                                stateBroadcast.sendername(), pubAddr, repAddr, false));
                    poco_debug(logger, "Module-" + peerHandle->UniqueId.getString() + " new with state " +
                                       std::to_string(peerState));
                    onPeerNew(peerHandle, peerState, addStateVec);
                }
                else {
                    // Reset peer timeout
                    static_cast<zmf::data::ModuleHandleInternal*>(peerHandle.get())->PeerTimeout = 0;
                    // Update module state in registry (if trackModuleStates_)

                    zmf::discovery::PeerRegistryInternal* peerRegistryInternal =
                            static_cast<zmf::discovery::PeerRegistryInternal*>(peerRegistry_.get());
                    peerRegistryInternal->INTERNAL_updatePeerAdditionalState(
                            peerHandle->UniqueId, addStateVec);
                    zmf::data::ModuleState oldState = peerRegistryInternal->getPeerState(peerHandle);
                    if (oldState != peerState) {
                        poco_debug(logger, "Module-" + peerHandle->UniqueId.getString() + " state changed " +
                                           std::to_string(oldState) + " to " +
                                           std::to_string(peerState));
                        peerRegistryInternal->INTERNAL_updatePeerState(
                                peerHandle->UniqueId, peerState);
                        core_->peerStateChange(peerHandle, peerState, oldState);
                    }
                }
            }
        }

        void PeerDiscoveryService::updateSelfState(zmf::data::ModuleState state) {
            sendBroadcastMutex_.lock();
            this->selfState_ = state;
            sendBroadcastMutex_.unlock();
        }

        void PeerDiscoveryService::updateSelfAdditionalState(std::vector<uint8_t> additionalState) {
            sendBroadcastMutex_.lock();
            this->selfAdditionalState_ = additionalState;
            sendBroadcastMutex_.unlock();
        }

        void PeerDiscoveryService::sendStateMulticast() {

            if (!isStarted_ || stopRequested_) {
                return;
            }

            std::lock_guard<std::mutex> lock(sendBroadcastMutex_);
            try {
                // Build and send the protobuf-Message
                zmf::proto::StateBroadcast stateBroadcast;

                stateBroadcast.set_zmqpubport(
                        static_cast<zmf::data::ModuleHandleInternal*>(selfHandle_.get())->self_ZmqPubPort);
                stateBroadcast.set_zmqrepport(
                        static_cast<zmf::data::ModuleHandleInternal*>(selfHandle_.get())->self_ZmqRepPort);
                stateBroadcast.set_version(selfHandle_->Version);
                stateBroadcast.set_lifecyclestate(selfState_);
                stateBroadcast.set_sendername(selfHandle_->Name);

                zmf::proto::SenderId* senderId = stateBroadcast.mutable_senderid();
                senderId->set_typeid_(selfHandle_->UniqueId.TypeId);
                senderId->set_instanceid(selfHandle_->UniqueId.InstanceId);

                std::vector<uint8_t> additionalStates = selfAdditionalState_;
                stateBroadcast.set_additionalstateinfos(std::string(additionalStates.begin(), additionalStates.end()));

                stateBroadcast.set_multicastidentifier(this->multicastIdentifier_);

                std::string message;
                stateBroadcast.SerializeToString(&message);

                this->UdpMulticastSocket_.sendTo(message.data(), (int) message.length(), this->multicastAdress_);
            } catch (Poco::Exception e) {
                logger.error(
                        "PeerDiscoveryService sendMulticast failed due to Poco::Exception:" + e.displayText());
                return;
            } catch (std::exception e) {
                std::string message = "PeerDiscoveryService sendMulticast failed due to Exception:";
                message.append(e.what());
                logger.error(message);
                return;
            }
        }


        void PeerDiscoveryService::onPeerNew(std::shared_ptr<zmf::data::ModuleHandle> newPeer,
                                             zmf::data::ModuleState state, std::vector<uint8_t> additionalState) {
            if (!isStarted_ || stopRequested_) {
                return;
            }
            static_cast<zmf::discovery::PeerRegistryInternal*>(peerRegistry_.get())->addModule(newPeer,
                                                                                               state, additionalState);
            core_->peerStateChange(newPeer, state, zmf::data::ModuleState::Dead);
        }

        void PeerDiscoveryService::onPeerDead(std::shared_ptr<zmf::data::ModuleHandle> deadPeer) {
            if (!isStarted_ || stopRequested_) {
                return;
            }
            zmf::data::ModuleState lastState = peerRegistry_->getPeerState(deadPeer);
            static_cast<zmf::discovery::PeerRegistryInternal*>(peerRegistry_.get())->removeModule(deadPeer->UniqueId);
            poco_information(logger, "Removed module from registry: " + deadPeer->UniqueId.getString());
            core_->peerStateChange(deadPeer, zmf::data::ModuleState::Dead, lastState);
        }

        void PeerDiscoveryService::disposeThread() {
            if (threadInitialized_) {
                joinThread();
                threadInitialized_ = false;
            }
        }


        void PeerDiscoveryService::joinThread() {
            if (multicastThread_->joinable()) {
                logger.trace("Waiting for sendMulticastThread to terminate");
                try {
                    multicastThread_->join();
                    logger.trace("sendMulticastThread terminated");
                }
                catch (...) {  // Catch all
                    logger.error("Failed to join sendMulticastThread");
                }
            }

        }


    }
}
