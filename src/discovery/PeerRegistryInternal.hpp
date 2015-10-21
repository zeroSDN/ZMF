#ifndef ZMF_PEERREGISTRYINTERNAL_H
#define ZMF_PEERREGISTRYINTERNAL_H

#include <discovery/PeerRegistry.hpp>


namespace zmf {
    namespace discovery {
        /**
         * @details The PeerRegistry keeps track of every module in the network. It tracks their state and
         * is able to provide information about all those modules.
         * Derived class from PeerRegistry which implements the internal functions such as add, remove, clear and update.
         * @author Jonas Grunert
         * @date created 8/5/15.
         */
        class PeerRegistryInternal : public PeerRegistry {
        public:

            PeerRegistryInternal() : logger(zmf::logging::ZmfLogging::getLogger("PeerRegistry")) { }

            virtual ~PeerRegistryInternal();

            /**
             * Adds the given module to the registry
             */
            void addModule(std::shared_ptr<zmf::data::ModuleHandle> toAdd,
                           zmf::data::ModuleState state, std::vector<uint8_t> additionalState);

            /**
             * Removes the given module from the registry
             */
            void removeModule(zmf::data::ModuleUniqueId toRemoveId);

            /**
             * Clears the registry
             */
            void clearRegistry();


            /**
             * Warning: Returns reference unsynchronized, internal datastructures
             * Returns the internal data structure with all peers, sorted by ID
             * Contract: No guarantee that data structure not changed concurrently. Copy recommended.
             * Performance: Direct return of map, very fast
             * @param onlyActivePeers Returns only all active peers
             * @return Reference to map with all peers by Id
             */
            inline std::map<zmf::data::ModuleUniqueId, std::shared_ptr<zmf::data::ModuleHandle>>& INTERNAL_getAllPeers(
                    bool onlyActivePeers = true) {
                if (onlyActivePeers) {
                    return allActivePeers;

                } else {
                    return allPeers;
                }
            }

            /**
             * Warning: Returns reference unsynchronized, internal datastructures
             * Returns the internal data structure with all peers, sorted by Type
             * Contract: No guarantee that data structure not changed concurrently. Copy recommended.
             * Performance: Direct return of map, very fast
             * @param onlyActivePeers Returns only all active peers
             * @return Reference to map with all peers by Tape
             */
            inline std::map<uint16_t, std::list<std::shared_ptr<zmf::data::ModuleHandle>>>& INTERNAL_getAllPeersByType(
                    bool onlyActivePeers = true) {
                if (onlyActivePeers) {
                    return allActivePeersByType;

                } else {
                    return allPeersByType;
                }
            }


            /**
             * Updates the state of the given module from the registry
             */
            void INTERNAL_updatePeerState(zmf::data::ModuleUniqueId module, zmf::data::ModuleState newState);

            /**
             * Updates the additional state of the given module from the registry
             */
            void INTERNAL_updatePeerAdditionalState(zmf::data::ModuleUniqueId module,
                                                    std::vector<uint8_t> newAdditionalState);


            std::mutex& getPeerRegistryLock() {
                return peerRegistryLock;
            }


        private:
            Poco::Logger& logger;
        public:
            virtual void printPeerRegistry() override;
        };
    }
}

#endif //ZMF_PEERREGISTRYINTERNAL_H
