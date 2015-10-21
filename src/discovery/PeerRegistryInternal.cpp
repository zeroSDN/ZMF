/**
* @author Jonas Grunert
* @date created 8/5/15.
*/

#include "PeerRegistryInternal.hpp"
#include <sstream>


using namespace std;


namespace zmf {
    namespace discovery {

        PeerRegistryInternal::~PeerRegistryInternal() {
            clearRegistry();
        }


        void addModuleInternal(shared_ptr<zmf::data::ModuleHandle> toAdd,
                               std::map<zmf::data::ModuleUniqueId, std::shared_ptr<zmf::data::ModuleHandle>>& peersMap,
                               std::map<uint16_t, std::list<std::shared_ptr<zmf::data::ModuleHandle>>>& peersByTypeMap) {
            // Add to modules
            peersMap.insert(
                    std::pair<zmf::data::ModuleUniqueId, shared_ptr<zmf::data::ModuleHandle>>
                            (toAdd->UniqueId, toAdd));

            // Add to modules by type
            if (peersByTypeMap.count(toAdd->UniqueId.TypeId) > 0) {
                // Add to existing set
                peersByTypeMap[toAdd->UniqueId.TypeId].push_back(toAdd);
            } else {
                // Create new set
                list<std::shared_ptr<zmf::data::ModuleHandle>> newTypeList;
                newTypeList.push_back(toAdd);
                peersByTypeMap.insert(
                        std::pair<uint16_t, list<shared_ptr<zmf::data::ModuleHandle>>>(toAdd->UniqueId.TypeId,
                                                                                       newTypeList));
            }
        }

        void PeerRegistryInternal::addModule(shared_ptr<zmf::data::ModuleHandle> toAdd,
                                             zmf::data::ModuleState state, std::vector<uint8_t> additionalState) {
            std::lock_guard<std::mutex> scopeLock(peerRegistryLock);

            if (allPeers.count(toAdd->UniqueId) == 0) {
                // Add to modules
                addModuleInternal(toAdd, allPeers, allPeersByType);
                peerStates[toAdd->UniqueId] = state;
                peerAdditionalStates[toAdd->UniqueId] = additionalState;
                // Add to active modules if active
                if (state == zmf::data::ModuleState::Active) {
                    addModuleInternal(toAdd, allActivePeers, allActivePeersByType);
                }
            }
            else {
                // Module already in registry
                std::ostringstream logMsg;
                logMsg << "Tried to add a module twice to the registry: " << toAdd->UniqueId.TypeId
                << ":" << toAdd->UniqueId.InstanceId << '\n';
                logger.error(logMsg.str());
            }
        }


        void removeModuleInternal(zmf::data::ModuleUniqueId toRemoveId,
                                  std::map<zmf::data::ModuleUniqueId, std::shared_ptr<zmf::data::ModuleHandle>>& peersMap,
                                  std::map<uint16_t, std::list<std::shared_ptr<zmf::data::ModuleHandle>>>& peersByTypeMap) {
            if (peersMap.count(toRemoveId) > 0) {
                // Erase from allModules
                peersMap.erase(toRemoveId);

                // Erase from allModulesByType
                list<std::shared_ptr<zmf::data::ModuleHandle>>& typeList = peersByTypeMap[toRemoveId.TypeId];
                if (typeList.size() == 1) {
                    // If all assumptions are correct this module is the last module of this type
                    peersByTypeMap.erase(toRemoveId.TypeId);
                } else {
                    // Remove module from type list
                    for (std::list<std::shared_ptr<zmf::data::ModuleHandle>>::iterator i = typeList.begin();
                         i != typeList.end(); i++) {
                        if ((*i)->UniqueId == toRemoveId) {
                            typeList.erase(i);
                            break;
                        }
                    }
                }
            }
        }

        void PeerRegistryInternal::removeModule(zmf::data::ModuleUniqueId toRemoveId) {
            std::lock_guard<std::mutex> scopeLock(peerRegistryLock);

            if (allPeers.count(toRemoveId) > 0) {
                // Remove from all data structures
                removeModuleInternal(toRemoveId, allPeers, allPeersByType);
                removeModuleInternal(toRemoveId, allActivePeers, allActivePeersByType);
            }
            else {
                // Module not in registry
                std::ostringstream logMsg;
                logMsg << "Tried to remove a module not in the registry: " << toRemoveId.TypeId << ":" <<
                toRemoveId.InstanceId <<
                '\n';
                logger.error(logMsg.str());
            }
        }


        void PeerRegistryInternal::clearRegistry() {
            std::lock_guard<std::mutex> scopeLock(peerRegistryLock);

            allPeers.clear();
            allActivePeers.clear();
            allPeersByType.clear();
            allActivePeersByType.clear();
        }


        void PeerRegistryInternal::INTERNAL_updatePeerState(zmf::data::ModuleUniqueId module,
                                                            zmf::data::ModuleState newState) {
            std::lock_guard<std::mutex> scopeLock(peerRegistryLock);

            if (peerStates.count(module) > 0) {
                if (peerStates[module] != newState) {
                    if (newState == zmf::data::ModuleState::Active) {
                        // Module active now
                        if (allActivePeers.count(module) == 0) {
                            addModuleInternal(allPeers[module], allActivePeers, allActivePeersByType);
                        }
                    } else {
                        // Module not active now
                        if (allActivePeers.count(module) > 0) {
                            removeModuleInternal(module, allActivePeers, allActivePeersByType);
                        }
                    }
                    peerStates[module] = newState;
                }
            }
            else {
                // Module not in registry
                std::ostringstream logMsg;
                logMsg << "Tried to update state of a module not in the registry: " << module.TypeId <<
                ":" <<
                module.InstanceId <<
                '\n';
                logger.error(logMsg.str());
            }
        }


        void PeerRegistryInternal::INTERNAL_updatePeerAdditionalState(zmf::data::ModuleUniqueId module,
                                                                      std::vector<uint8_t> newAdditionalState) {
            std::lock_guard<std::mutex> scopeLock(peerRegistryLock);

            if (peerAdditionalStates.count(module) > 0) {
                peerAdditionalStates[module] = newAdditionalState;
            }
            else {
                // Module not in registry
                std::ostringstream logMsg;
                logMsg << "Tried to update additional states of a module not in the registry: " <<
                module.TypeId <<
                ":" <<
                module.InstanceId <<
                '\n';
                logger.error(logMsg.str());
            }
        }


        void PeerRegistryInternal::printPeerRegistry() {
            for (auto typeModules : allPeersByType) {
                std::cout << "---- type " << typeModules.first << &std::endl;
                for (auto module : typeModules.second) {
                    std::cout << "------ instance " << module->UniqueId.InstanceId << ", version " << module->Version
                    //<< ", state " << module->CurrentState << ", additional-state " << module->AdditionalStateInfos << &std::endl;
                    << ", state " << getPeerState(module) << ", additional-state " <<
                    &std::endl;
                }
            }
        }
    }
}