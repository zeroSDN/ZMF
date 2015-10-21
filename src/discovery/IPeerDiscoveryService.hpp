#ifndef ZMF_IPEERDISCOVERYSERVICE_H
#define ZMF_IPEERDISCOVERYSERVICE_H

#include <memory>
#include <set>
#include "IPeerDiscoveryCoreInterface.hpp"
#include <data/ModuleHandle.hpp>
#include <discovery/PeerRegistry.hpp>
#include <config/IConfigurationProvider.hpp>


namespace zmf {
    namespace discovery {

        /**
         * @details Interface to access the PeerDiscoveryService -
         * Service class to recognize other modules and there states in the network.
         * PeerStates can be accessed via PeerRegistry (getPeerRegistry)
         * @author Jonas Grunert
         * @date created on 7/7/15.
         */
        class IPeerDiscoveryService {

        public:
            /**
             * Starts the peer discovery service: State broadcasting, peer discovery and module registry.
             * @param core Reference to the ZMF core
             * @param selfHandle Module handle for the module of this instance. Contains information about module and its state
             * @param broadcastFrequency Frequency to send out state broadcasts (in milliseconds between broadcasts)
             * @param udpPort UDP port for state broadcast communication
             * @param trackModuleStates Flag to enable/disable tracking of module states. If disabled only module leave and join will be tracked
             *
             * @return Returns true if start successful, false otherwise
             */
            virtual bool start(std::shared_ptr<config::IConfigurationProvider> config,
                               IPeerDiscoveryCoreInterface* const core,
                               std::shared_ptr<zmf::data::ModuleHandle> selfHandle,
                               uint32_t multicastFrequency, uint16_t udpPort, bool peerDiscoveryWait,
                               bool disableEqualModuleInterconnect) = 0;

            virtual void stop() = 0;

            /**
             * Forces a broadcast when a module was changed
             */
            virtual void sendStateMulticast() = 0;

            /**
             * @return Peer registry where all known active peers are registered
             */
            virtual const std::shared_ptr<zmf::discovery::PeerRegistry>& getPeerRegistry() = 0;


            virtual void updateSelfState(zmf::data::ModuleState state) = 0;

            virtual void updateSelfAdditionalState(std::vector<uint8_t> additionalState) = 0;
        };
    }
}


#endif //ZMF_IPEERDISCOVERYSERVICE_H
