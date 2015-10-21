#ifndef ZMF_PEERREGISTRY_H
#define ZMF_PEERREGISTRY_H

#include <map>
#include <memory>
#include <list>
#include <stdint.h>
#include <Poco/Logger.h>
#include "../data/ModuleHandle.hpp"
#include "../logging/ZmfLogging.hpp"


namespace zmf {
    namespace discovery {
        /**
         * @details Data structure for the PeerRegistry which represents all known other peers in the system.
         * Base class for PeerRegistryInternal.
         * @author Jonas Grunert
         * @date created on 6/26/15.
         */
        class PeerRegistry {
        public:

            /**
             * Contract: Thread safe
             * Performance: No data structures created, fast
             * @param onlyActivePeers Checks only active peers
             * @return True if there is a module with the given unique ID
             */
            inline bool containsPeerWithId(zmf::data::ModuleUniqueId id, bool onlyActivePeers = true) {
                std::lock_guard<std::mutex> scopeLock(peerRegistryLock);
                std::map<zmf::data::ModuleUniqueId, std::shared_ptr<zmf::data::ModuleHandle>>& allPeersTmp = onlyActivePeers
                                                                                                             ? allActivePeers
                                                                                                             : allPeers;
                if (allPeersTmp.count(id) > 0) {
                    return !onlyActivePeers || peerStates[id] == zmf::data::ModuleState::Active;
                }
                else {
                    return false;
                }
            }


            /**
             * Contract: Thread safe and returns shared_ptr on module handle
             * Performance: No data structures created, fast
             * @param onlyActivePeers Returns only an active peer
             * @return A shared_ptr(ModuleHandle) with the given ID or shared_ptr(nullptr) if there is none
             */
            inline std::shared_ptr<zmf::data::ModuleHandle> getPeerWithId(zmf::data::ModuleUniqueId id,
                                                                          bool onlyActivePeers = true) {
                std::lock_guard<std::mutex> scopeLock(peerRegistryLock);
                std::map<zmf::data::ModuleUniqueId, std::shared_ptr<zmf::data::ModuleHandle>>& allPeersTmp = onlyActivePeers
                                                                                                             ? allActivePeers
                                                                                                             : allPeers;
                if (allPeersTmp.count(id) > 0) {
                    std::shared_ptr<zmf::data::ModuleHandle> peer = allPeersTmp[id];
                    if (!onlyActivePeers || peerStates[id] == zmf::data::ModuleState::Active) {
                        return peer;
                    } else {
                        return std::shared_ptr<zmf::data::ModuleHandle>(nullptr);
                    }
                } else {
                    return std::shared_ptr<zmf::data::ModuleHandle>(nullptr);
                }
            }


            /**
             * Contract: Thread safe and returns copied list, not concurrently modified later
             * Performance: Copied list of all module handles, moderate
             * @param onlyActivePeers Returns only all active peers
             * @return A list of all modules with the given type, an empty list if there are none
             */
            inline std::list<std::shared_ptr<zmf::data::ModuleHandle>> getPeersWithType(uint16_t type,
                                                                                        bool onlyActivePeers = true) {
                std::lock_guard<std::mutex> scopeLock(peerRegistryLock);
                std::map<uint16_t, std::list<std::shared_ptr<zmf::data::ModuleHandle>>>& allPeersTmp = onlyActivePeers
                                                                                                       ? allActivePeersByType
                                                                                                       : allPeersByType;
                if (allPeersTmp.count(type) > 0) {
                    return allPeersTmp[type];
                } else {
                    return std::list<std::shared_ptr<zmf::data::ModuleHandle>>();
                }
            }

            /**
             * Contract: Thread safe
             * Performance: No new data structure created, fast
             * @param onlyActivePeers Checks only all active peers
             * @return True if there is at least one module of the given type in the registry
             */
            inline bool containsPeerWithType(uint16_t type, bool onlyActivePeers = true) {
                std::lock_guard<std::mutex> scopeLock(peerRegistryLock);
                if (onlyActivePeers) {
                    return allActivePeersByType.count(type) > 0;
                }
                else {
                    return allPeersByType.count(type) > 0;
                }
            }

            /**
             * Contract: Thread safe and returns copied list, not concurrently modified later
             * Performance: Copied list of all module handles, moderate
             * @param onlyActivePeers Returns only all active peers
             * @return A list of all modules with the given type, an empty set if there are none
             */
            inline std::shared_ptr<zmf::data::ModuleHandle> getAnyPeerWithType(uint16_t type,
                                                                               bool onlyActivePeers = true) {
                std::lock_guard<std::mutex> scopeLock(peerRegistryLock);
                std::map<uint16_t, std::list<std::shared_ptr<zmf::data::ModuleHandle>>>& allPeersTmp = onlyActivePeers
                                                                                                       ? allActivePeersByType
                                                                                                       : allPeersByType;
                if (allPeersTmp.count(type) > 0) {
                    std::list<std::shared_ptr<zmf::data::ModuleHandle>>& typePeers = allPeersTmp[type];
                    for (auto peer : typePeers) {
                        return peer;
                    }
                }
                return std::shared_ptr<zmf::data::ModuleHandle>(nullptr);
            }


            /**
             * Contract: Thread safe and returns newly created list, not concurrently modified later
             * Performance: New list of all module handle created, slow
             * @param onlyActivePeers Returns only all active peers
             * @return A list of all modules with the given type and version, an empty set if there are none
             */
            inline std::list<std::shared_ptr<zmf::data::ModuleHandle>> getPeersWithTypeVersion(uint16_t type,
                                                                                               uint16_t version,
                                                                                               bool onlyActivePeers = true) {
                std::lock_guard<std::mutex> scopeLock(peerRegistryLock);
                std::map<uint16_t, std::list<std::shared_ptr<zmf::data::ModuleHandle>>>& allPeersTmp = onlyActivePeers
                                                                                                       ? allActivePeersByType
                                                                                                       : allPeersByType;
                if (allPeersTmp.count(type) > 0) {
                    std::list<std::shared_ptr<zmf::data::ModuleHandle>> typePeers = allPeersTmp[type];
                    std::list<std::shared_ptr<zmf::data::ModuleHandle>> typeVersionPeers;
                    for (auto peer : typePeers) {
                        if (peer->Version == version) {
                            typeVersionPeers.push_back(peer);
                        }
                    }
                    return typeVersionPeers;
                } else {
                    return std::list<std::shared_ptr<zmf::data::ModuleHandle>>();
                }
            }

            /**
             * Contract: Thread safe
             * Performance: No new data structure created, fast
             * @param onlyActivePeers only checks all active peers
             * @return True if there is at least one module of the given type with the given version in the registry
             */
            inline bool containsPeerWithTypeVersion(uint16_t type, uint16_t version, bool onlyActivePeers = true) {
                std::lock_guard<std::mutex> scopeLock(peerRegistryLock);
                std::map<uint16_t, std::list<std::shared_ptr<zmf::data::ModuleHandle>>>& allPeersTmp = onlyActivePeers
                                                                                                       ? allActivePeersByType
                                                                                                       : allPeersByType;
                if (allPeersTmp.count(type) > 0) {
                    std::list<std::shared_ptr<zmf::data::ModuleHandle>>& typePeers = allPeersTmp[type];
                    for (auto peer : typePeers) {
                        if (peer->Version == version) {
                            return true;
                        }
                    }
                }
                return false;
            }

            /**
             * Contract: Thread safe and returns shared_ptr on module handle (copy)
             * Performance: No new data structure created, fast
             * @param onlyActivePeers only checks all active peers
             * @return Returns one module if at least one module of the given type with the given version in the registry.
             * Otherwise Null
             */
            inline std::shared_ptr<zmf::data::ModuleHandle> getAnyPeerWithTypeVersion(uint16_t type, uint16_t version,
                                                                                      bool onlyActivePeers = true) {
                std::lock_guard<std::mutex> scopeLock(peerRegistryLock);
                std::map<uint16_t, std::list<std::shared_ptr<zmf::data::ModuleHandle>>>& allPeersTmp = onlyActivePeers
                                                                                                       ? allActivePeersByType
                                                                                                       : allPeersByType;
                if (allPeersTmp.count(type) > 0) {
                    std::list<std::shared_ptr<zmf::data::ModuleHandle>>& typePeers = allPeersTmp[type];
                    for (auto peer : typePeers) {
                        if (peer->Version == version) {
                            return peer;
                        }
                    }
                }
                return std::shared_ptr<zmf::data::ModuleHandle>(nullptr);
            }


            /**
             * Contract: Thread safe and returns value of current state
             * Performance: No new data structure created, fast
             * @return Current state of a peer if peer in registry, ModuleState.Dead otherwise
             */
            inline zmf::data::ModuleState getPeerState(std::shared_ptr<zmf::data::ModuleHandle> peerHandle) {
                std::lock_guard<std::mutex> scopeLock(peerRegistryLock);
                if (peerStates.count(peerHandle->UniqueId) > 0) {
                    return peerStates[peerHandle->UniqueId];
                } else {
                    return zmf::data::ModuleState::Dead;
                }
            }

            /**
             * Contract: Thread safe and returns copy of additional state vector
             * Performance: Copies additional state vector, moderate
             * @return Current additional state of a peer in registry, empty vector otherwise
             */
            inline std::vector<uint8_t> getPeerAdditionalState(std::shared_ptr<zmf::data::ModuleHandle> peerHandle) {
                std::lock_guard<std::mutex> scopeLock(peerRegistryLock);
                if (allPeers.count(peerHandle->UniqueId) > 0) {
                    return peerAdditionalStates[peerHandle->UniqueId];
                } else {
                    return std::vector<uint8_t>();
                }
            }

            virtual void printPeerRegistry() = 0;


        protected:
            PeerRegistry() { }

            /// Map of all registered modules
            std::map<zmf::data::ModuleUniqueId, std::shared_ptr<zmf::data::ModuleHandle>> allPeers;
            /// Map of all registered modules, grouped by type
            std::map<uint16_t, std::list<std::shared_ptr<zmf::data::ModuleHandle>>> allPeersByType;

            /// Map of all active registered modules
            std::map<zmf::data::ModuleUniqueId, std::shared_ptr<zmf::data::ModuleHandle>> allActivePeers;
            /// Map of all active registered modules, grouped by type
            std::map<uint16_t, std::list<std::shared_ptr<zmf::data::ModuleHandle>>> allActivePeersByType;

            /// Map of all registered module IDs and their states
            std::map<zmf::data::ModuleUniqueId, zmf::data::ModuleState> peerStates;
            /// Map of all registered module IDs and their additional states
            std::map<zmf::data::ModuleUniqueId, std::vector<uint8_t>> peerAdditionalStates;

            std::mutex peerRegistryLock;
        };
    }
}

#endif //ZMF_PEERREGISTRY_H
