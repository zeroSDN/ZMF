//
// Created by Matthias Blohm on 6/25/15.
//

#ifndef ZMF_PEERDISCOVERYSERVICE_H
#define ZMF_PEERDISCOVERYSERVICE_H

#include "../discovery/IPeerDiscoveryService.hpp"
#include "../discovery/PeerRegistryInternal.hpp"
#include "../data/ModuleHandleInternal.hpp"
#include "NetworkUtil.hpp"
#include <Poco/Net/MulticastSocket.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Random.h>
#include "../proto/FrameworkProto.pb.h"
#include "IConfigurationProvider.hpp"
#include <unistd.h>
#include <iostream>
#include <string>
#include <cmath>
#include <thread>

namespace zmf {
    namespace discovery {
        /**
       * @brief Service class to recognize other modules and there states in the network
       * @details The PeerDiscoveryService Class regularly sends out udp-multicasts to the network and receives those of other module instances.
       * In doing so it recognizes other modules with their current lifecycleState and adds them to the PeerRegistryClass, where all known module instances are listed and updated.
       * At the same time the service lets all other instances know of the current state of its own module so that they can update their registries in the same way.
       * The PeerDiscoveryService is a main component of the ZMF-framework and thus started and stopped with the ZMF-instance.
       *
       * @author Matthias Blohm
       * @author Jonas Grunert
       * @author Jan Strauss
       * @date created on 6/25/15.
      */
        class PeerDiscoveryService : public IPeerDiscoveryService {
        public:

            /**
             *Constructor initializes Poco::Logger and all flags with false
             * @param instanceId the uniqueId of the own module
             */
            PeerDiscoveryService(zmf::data::ModuleUniqueId instanceId);

            /**
             *Destructor stops the service if necessary and makes sure that both threads terminated execution
             */
            ~PeerDiscoveryService();

            /**
             * Starts the peer discovery service: State multicasting, peer discovery and module registry
             * @param core Reference to the ZMF core
             * @param selfHandle Module handle for the module of this instance. Contains information about module and its state
             * @param multicastFrequency Frequency to send out state multicasts (in milliseconds between multicasts)
             * @param udpPort UDP port for state multicast communication
             * @param peerDiscoveryWait If true the service will wait until all active modules discovered before it returns from the starting process
             *
             * @return Returns true if start successful, false otherwise
             */
            bool start(std::shared_ptr<config::IConfigurationProvider> config,
                       IPeerDiscoveryCoreInterface* const core,
                       std::shared_ptr<zmf::data::ModuleHandle> selfHandle,
                       uint32_t multicastFrequency, uint16_t udpPort, bool peerDiscoveryWait,
                       bool disableEqualModuleInterconnect);

            /**
             * Stops the peer discovery service
            */
            void stop();

            /**
             * sends out a udp-multicast to the network with the current state of the own module
             */
            void sendStateMulticast();

            /**
             * Returns the current peer registry with all actual know modules
             * @return Peer registry where all known peers are registered
             */
            virtual inline const std::shared_ptr<zmf::discovery::PeerRegistry>& getPeerRegistry() {
                return peerRegistry_;
            }

            /**
           * Called when there is a change in the lifecycle-state of the own module, updates the selfState_ variable
           * @param state The new lifecycle-state that the own module has now
           */
            virtual void updateSelfState(zmf::data::ModuleState state);

            /**
           * Called when there is a change in the additional state of own module, updates the selfAdditionalState_ variable
           * @param additionalState The new additional state that the own module should be set to
           */
            virtual void updateSelfAdditionalState(std::vector<uint8_t> additionalState);


        private:
            /**
           * MULTICAST_ADDRESS contains the ip address that is uses for the multicast group to which all peer discovery services subscribe
           */
            static const std::string MULTICAST_ADDRESS;

            /**
           * Reference to the ZMF-Core to which the service belongs
           */
            IPeerDiscoveryCoreInterface* core_;

            /**
           * Reference to the own ModuleHandle of this ZMF-instance
           */
            std::shared_ptr<zmf::data::ModuleHandle> selfHandle_;

            /**
           * Contains the current lifecycle-state of the own module
           */
            zmf::data::ModuleState selfState_;

            /**
           * Contains the current additional state of the own module
           */
            std::vector<uint8_t> selfAdditionalState_;

            /// This is where all peer modules are registered
            std::shared_ptr<zmf::discovery::PeerRegistry> peerRegistry_;

            /// Poco-Multicast-Address used for sending out heartbeats to the multicast group
            Poco::Net::SocketAddress multicastAdress_;

            /// Poco-Network-Interface used for joining the multicast group
            Poco::Net::NetworkInterface multicastInterface_;

            /// Poco-Socket for sending and receiving UDP-multicasts
            Poco::Net::MulticastSocket UdpMulticastSocket_;

            /** Portnumber used for multicasts */
            uint16_t udpPort_;

            /** Factor to determine peerTimeout which is (multicastFrequency * timeoutFactor) */
            uint32_t PEER_TIMEOUT_FACTOR = 4;
            /** Frequency to send state multicasts (in ms.) */
            uint32_t multicastFrequency_;
            /** Time without a received state multicast to mark a peer as dead (in ms.) */
            uint32_t peerTimeout_;
            /** multicastIdentifier gets a random number that helps this instance to recognize its own broadcasts coming in */
            uint32_t multicastIdentifier_;

            /** Indicates whether the PeerDiscoveryService is currently running */
            std::atomic_bool isStarted_;
            /** Indicates whether a stop of the service has been requested which is beeing executed now */
            std::atomic_bool stopRequested_;
            /** Indicates whether the multicastThread has been initialized already and is currently running */
            std::atomic_bool threadInitialized_;

            /** Thread for sending and receiving multicasts */
            std::unique_ptr<std::thread> multicastThread_;
            /// Locks the execution of sending a state broadcast
            std::mutex sendBroadcastMutex_;

            /// Usage of the the Poco-Logger in this service
            Poco::Logger& logger;

            /// if set, ignore peers with same module id
            bool disableEqualModuleInterconnect_;


            /**
             * Performed by the multicastThread_ this loop continuously sends and receives multicasts as long as the service is running
            */
            void multicastLoop();

            /**
             * Checks whether the socket has received new multicasts and processes them if so
            */
            void receiveMulticasts();

            /**
             * processes an incoming multicast-message and updates PeerRegistry if necessary
             * @param stateBroadcast the incoming message parsed back to protobuffer-class
             * @param string ip the sender's ip address
            */
            void processIncomingMulticast(zmf::proto::StateBroadcast stateBroadcast, std::string ip);

            /**
             * updates the timeout-values of the registered modules in peerRegistry_
            */
            void updatePeerTimeouts();

            /**
             * Called when a new peer recognized, updates peerRegistry_ and notifies core
             * @param newPeer the ModuleHandle of a new peer that was recognized through multicast
             * @param state the current lifecycle-state of the new peer that was recognized
             * @param additionalState the current additional state of the new peer that was recognized
             */
            void onPeerNew(std::shared_ptr<zmf::data::ModuleHandle> newPeer,
                           zmf::data::ModuleState state, std::vector<uint8_t> additionalState);

            /**
             * Called when a peer timed out or received dead multicast, updates PeerRegistry and notifies core
             * @param deadPeer the ModuleHandle of a peer that was recognized as dead
             */
            void onPeerDead(std::shared_ptr<zmf::data::ModuleHandle> deadPeer);

            /**
             * Called in the destructor of the class. Makes sure that the multicastThread is stopped correctly
             */
            void disposeThread();

            /**
             * Waits for the multicast-thread to terminate its execution if not done so far
             */
            void joinThread();


        };
    }
}


#endif //ZMF_PEERDISCOVERYSERVICE_H
