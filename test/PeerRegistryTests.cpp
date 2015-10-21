//
// Created by Jonas Grunert on 7/6/15.
//

#include "PeerRegistryTests.h"
#include "TestModuleA.h"
#include <discovery/PeerRegistry.hpp>
#include <discovery/PeerRegistryInternal.hpp>
#include <AbstractModule.hpp>
#include <data/ModuleHandleInternal.hpp>
#include <memory>
#include <Poco/Exception.h>
#include <Poco/Logger.h>

using namespace std;


PeerRegistryTests::PeerRegistryTests() {
}


zmf::data::ModuleUniqueId id10 = zmf::data::ModuleUniqueId(1, 0);
zmf::data::ModuleUniqueId id11 = zmf::data::ModuleUniqueId(1, 1);
zmf::data::ModuleUniqueId id12 = zmf::data::ModuleUniqueId(1, 2);
zmf::data::ModuleUniqueId id20 = zmf::data::ModuleUniqueId(2, 0);
zmf::data::ModuleUniqueId id21 = zmf::data::ModuleUniqueId(2, 1);
// No module with this id, only for testing purposes
zmf::data::ModuleUniqueId id30 = zmf::data::ModuleUniqueId(3, 0);


void checkRegistryEmpty(zmf::discovery::PeerRegistryInternal& peerRegistry, bool onlyActivePeers) {
    CPPUNIT_ASSERT(peerRegistry.INTERNAL_getAllPeers(onlyActivePeers).size() == 0);
    CPPUNIT_ASSERT(peerRegistry.INTERNAL_getAllPeersByType(onlyActivePeers).size() == 0);
    // Contains no type of peers
    CPPUNIT_ASSERT(!peerRegistry.containsPeerWithType(1, onlyActivePeers));
    CPPUNIT_ASSERT(!peerRegistry.containsPeerWithType(2, onlyActivePeers));
    CPPUNIT_ASSERT(!peerRegistry.containsPeerWithType(3, onlyActivePeers));
    CPPUNIT_ASSERT(peerRegistry.getPeersWithType(1, onlyActivePeers).size() == 0);
    CPPUNIT_ASSERT(peerRegistry.getPeersWithType(2, onlyActivePeers).size() == 0);
    CPPUNIT_ASSERT(peerRegistry.getPeersWithType(3, onlyActivePeers).size() == 0);
    CPPUNIT_ASSERT(peerRegistry.getAnyPeerWithType(1, onlyActivePeers).get() == nullptr);
    CPPUNIT_ASSERT(peerRegistry.getAnyPeerWithType(2, onlyActivePeers).get() == nullptr);
    CPPUNIT_ASSERT(peerRegistry.getAnyPeerWithType(2, onlyActivePeers).get() == nullptr);
    // No types, not versions
    CPPUNIT_ASSERT(!peerRegistry.containsPeerWithTypeVersion(1, 0, onlyActivePeers));
    CPPUNIT_ASSERT(!peerRegistry.containsPeerWithTypeVersion(1, 1, onlyActivePeers));
    CPPUNIT_ASSERT(!peerRegistry.containsPeerWithTypeVersion(2, 0, onlyActivePeers));
    CPPUNIT_ASSERT(!peerRegistry.containsPeerWithTypeVersion(2, 1, onlyActivePeers));
    CPPUNIT_ASSERT(!peerRegistry.containsPeerWithTypeVersion(3, 0, onlyActivePeers));
    CPPUNIT_ASSERT(!peerRegistry.containsPeerWithTypeVersion(3, 1, onlyActivePeers));
    CPPUNIT_ASSERT(peerRegistry.getPeersWithTypeVersion(1, 0, onlyActivePeers).size() == 0);
    CPPUNIT_ASSERT(peerRegistry.getPeersWithTypeVersion(1, 1, onlyActivePeers).size() == 0);
    CPPUNIT_ASSERT(peerRegistry.getPeersWithTypeVersion(2, 0, onlyActivePeers).size() == 0);
    CPPUNIT_ASSERT(peerRegistry.getPeersWithTypeVersion(2, 1, onlyActivePeers).size() == 0);
    CPPUNIT_ASSERT(peerRegistry.getPeersWithTypeVersion(3, 0, onlyActivePeers).size() == 0);
    CPPUNIT_ASSERT(peerRegistry.getPeersWithTypeVersion(3, 1, onlyActivePeers).size() == 0);
    CPPUNIT_ASSERT(peerRegistry.getAnyPeerWithTypeVersion(1, 0, onlyActivePeers).get() == nullptr);
    CPPUNIT_ASSERT(peerRegistry.getAnyPeerWithTypeVersion(1, 1, onlyActivePeers).get() == nullptr);
    CPPUNIT_ASSERT(peerRegistry.getAnyPeerWithTypeVersion(2, 0, onlyActivePeers).get() == nullptr);
    CPPUNIT_ASSERT(peerRegistry.getAnyPeerWithTypeVersion(2, 1, onlyActivePeers).get() == nullptr);
    CPPUNIT_ASSERT(peerRegistry.getAnyPeerWithTypeVersion(3, 0, onlyActivePeers).get() == nullptr);
    CPPUNIT_ASSERT(peerRegistry.getAnyPeerWithTypeVersion(3, 1, onlyActivePeers).get() == nullptr);
    // Contains no peers
    CPPUNIT_ASSERT(!peerRegistry.containsPeerWithId(id10, onlyActivePeers));
    CPPUNIT_ASSERT(!peerRegistry.containsPeerWithId(id11, onlyActivePeers));
    CPPUNIT_ASSERT(!peerRegistry.containsPeerWithId(id12, onlyActivePeers));
    CPPUNIT_ASSERT(!peerRegistry.containsPeerWithId(id20, onlyActivePeers));
    CPPUNIT_ASSERT(!peerRegistry.containsPeerWithId(id21, onlyActivePeers));
    CPPUNIT_ASSERT(!peerRegistry.containsPeerWithId(id30, onlyActivePeers));
    CPPUNIT_ASSERT(peerRegistry.getPeerWithId(id10, onlyActivePeers).get() == nullptr);
    CPPUNIT_ASSERT(peerRegistry.getPeerWithId(id11, onlyActivePeers).get() == nullptr);
    CPPUNIT_ASSERT(peerRegistry.getPeerWithId(id12, onlyActivePeers).get() == nullptr);
    CPPUNIT_ASSERT(peerRegistry.getPeerWithId(id20, onlyActivePeers).get() == nullptr);
    CPPUNIT_ASSERT(peerRegistry.getPeerWithId(id21, onlyActivePeers).get() == nullptr);
    CPPUNIT_ASSERT(peerRegistry.getPeerWithId(id30, onlyActivePeers).get() == nullptr);
}


void checkRegistry_10(zmf::discovery::PeerRegistryInternal& peerRegistry, bool onlyActivePeers) {
    CPPUNIT_ASSERT(peerRegistry.INTERNAL_getAllPeers(onlyActivePeers).size() == 1);
    CPPUNIT_ASSERT(peerRegistry.INTERNAL_getAllPeersByType(onlyActivePeers).size() == 1);
    // Contains no type of peers
    CPPUNIT_ASSERT(peerRegistry.containsPeerWithType(1, onlyActivePeers));
    CPPUNIT_ASSERT(!peerRegistry.containsPeerWithType(2, onlyActivePeers));
    CPPUNIT_ASSERT(!peerRegistry.containsPeerWithType(3, onlyActivePeers));
    CPPUNIT_ASSERT(peerRegistry.getPeersWithType(1, onlyActivePeers).size() == 1);
    CPPUNIT_ASSERT(peerRegistry.getPeersWithType(2, onlyActivePeers).size() == 0);
    CPPUNIT_ASSERT(peerRegistry.getPeersWithType(3, onlyActivePeers).size() == 0);
    CPPUNIT_ASSERT(peerRegistry.getAnyPeerWithType(1, onlyActivePeers).get() != nullptr);
    CPPUNIT_ASSERT(peerRegistry.getAnyPeerWithType(2, onlyActivePeers).get() == nullptr);
    CPPUNIT_ASSERT(peerRegistry.getAnyPeerWithType(3, onlyActivePeers).get() == nullptr);
    // No types, not versions
    CPPUNIT_ASSERT(peerRegistry.containsPeerWithTypeVersion(1, 0, onlyActivePeers));
    CPPUNIT_ASSERT(!peerRegistry.containsPeerWithTypeVersion(1, 1, onlyActivePeers));
    CPPUNIT_ASSERT(!peerRegistry.containsPeerWithTypeVersion(2, 0, onlyActivePeers));
    CPPUNIT_ASSERT(!peerRegistry.containsPeerWithTypeVersion(2, 1, onlyActivePeers));
    CPPUNIT_ASSERT(!peerRegistry.containsPeerWithTypeVersion(3, 0, onlyActivePeers));
    CPPUNIT_ASSERT(!peerRegistry.containsPeerWithTypeVersion(3, 1, onlyActivePeers));
    CPPUNIT_ASSERT(peerRegistry.getPeersWithTypeVersion(1, 0, onlyActivePeers).size() == 1);
    CPPUNIT_ASSERT(peerRegistry.getPeersWithTypeVersion(1, 1, onlyActivePeers).size() == 0);
    CPPUNIT_ASSERT(peerRegistry.getPeersWithTypeVersion(2, 0, onlyActivePeers).size() == 0);
    CPPUNIT_ASSERT(peerRegistry.getPeersWithTypeVersion(2, 1, onlyActivePeers).size() == 0);
    CPPUNIT_ASSERT(peerRegistry.getPeersWithTypeVersion(3, 0, onlyActivePeers).size() == 0);
    CPPUNIT_ASSERT(peerRegistry.getPeersWithTypeVersion(3, 1, onlyActivePeers).size() == 0);
    CPPUNIT_ASSERT(peerRegistry.getAnyPeerWithTypeVersion(1, 0, onlyActivePeers).get() != nullptr);
    CPPUNIT_ASSERT(peerRegistry.getAnyPeerWithTypeVersion(1, 0, onlyActivePeers)->UniqueId == id10);
    CPPUNIT_ASSERT(peerRegistry.getAnyPeerWithTypeVersion(1, 1, onlyActivePeers).get() == nullptr);
    CPPUNIT_ASSERT(peerRegistry.getAnyPeerWithTypeVersion(2, 0, onlyActivePeers).get() == nullptr);
    CPPUNIT_ASSERT(peerRegistry.getAnyPeerWithTypeVersion(2, 1, onlyActivePeers).get() == nullptr);
    CPPUNIT_ASSERT(peerRegistry.getAnyPeerWithTypeVersion(3, 0, onlyActivePeers).get() == nullptr);
    CPPUNIT_ASSERT(peerRegistry.getAnyPeerWithTypeVersion(3, 1, onlyActivePeers).get() == nullptr);
    // Contains no peers
    CPPUNIT_ASSERT(peerRegistry.containsPeerWithId(id10, onlyActivePeers));
    CPPUNIT_ASSERT(!peerRegistry.containsPeerWithId(id11, onlyActivePeers));
    CPPUNIT_ASSERT(!peerRegistry.containsPeerWithId(id12, onlyActivePeers));
    CPPUNIT_ASSERT(!peerRegistry.containsPeerWithId(id20, onlyActivePeers));
    CPPUNIT_ASSERT(!peerRegistry.containsPeerWithId(id21, onlyActivePeers));
    CPPUNIT_ASSERT(!peerRegistry.containsPeerWithId(id30, onlyActivePeers));
    CPPUNIT_ASSERT(peerRegistry.getPeerWithId(id10, onlyActivePeers).get() != nullptr);
    CPPUNIT_ASSERT(peerRegistry.getPeerWithId(id10, onlyActivePeers)->UniqueId == id10);
    CPPUNIT_ASSERT(peerRegistry.getPeerWithId(id11, onlyActivePeers).get() == nullptr);
    CPPUNIT_ASSERT(peerRegistry.getPeerWithId(id12, onlyActivePeers).get() == nullptr);
    CPPUNIT_ASSERT(peerRegistry.getPeerWithId(id20, onlyActivePeers).get() == nullptr);
    CPPUNIT_ASSERT(peerRegistry.getPeerWithId(id21, onlyActivePeers).get() == nullptr);
    CPPUNIT_ASSERT(peerRegistry.getPeerWithId(id30, onlyActivePeers).get() == nullptr);
}


void checkRegistry_10_21(zmf::discovery::PeerRegistryInternal& peerRegistry, bool onlyActivePeers) {
    CPPUNIT_ASSERT(peerRegistry.INTERNAL_getAllPeers(onlyActivePeers).size() == 2);
    CPPUNIT_ASSERT(peerRegistry.INTERNAL_getAllPeersByType(onlyActivePeers).size() == 2);
    // Contains no type of peers
    CPPUNIT_ASSERT(peerRegistry.containsPeerWithType(1, onlyActivePeers));
    CPPUNIT_ASSERT(peerRegistry.containsPeerWithType(2, onlyActivePeers));
    CPPUNIT_ASSERT(!peerRegistry.containsPeerWithType(3, onlyActivePeers));
    CPPUNIT_ASSERT(peerRegistry.getPeersWithType(1, onlyActivePeers).size() == 1);
    CPPUNIT_ASSERT(peerRegistry.getPeersWithType(2, onlyActivePeers).size() == 1);
    CPPUNIT_ASSERT(peerRegistry.getPeersWithType(3, onlyActivePeers).size() == 0);
    CPPUNIT_ASSERT(peerRegistry.getAnyPeerWithType(1, onlyActivePeers).get() != nullptr);
    CPPUNIT_ASSERT(peerRegistry.getAnyPeerWithType(2, onlyActivePeers).get() != nullptr);
    CPPUNIT_ASSERT(peerRegistry.getAnyPeerWithType(3, onlyActivePeers).get() == nullptr);
    // No types, not versions
    CPPUNIT_ASSERT(peerRegistry.containsPeerWithTypeVersion(1, 0, onlyActivePeers));
    CPPUNIT_ASSERT(!peerRegistry.containsPeerWithTypeVersion(1, 1, onlyActivePeers));
    CPPUNIT_ASSERT(!peerRegistry.containsPeerWithTypeVersion(2, 0, onlyActivePeers));
    CPPUNIT_ASSERT(peerRegistry.containsPeerWithTypeVersion(2, 1, onlyActivePeers));
    CPPUNIT_ASSERT(!peerRegistry.containsPeerWithTypeVersion(3, 0, onlyActivePeers));
    CPPUNIT_ASSERT(!peerRegistry.containsPeerWithTypeVersion(3, 1, onlyActivePeers));
    CPPUNIT_ASSERT(peerRegistry.getPeersWithTypeVersion(1, 0, onlyActivePeers).size() == 1);
    CPPUNIT_ASSERT(peerRegistry.getPeersWithTypeVersion(1, 1, onlyActivePeers).size() == 0);
    CPPUNIT_ASSERT(peerRegistry.getPeersWithTypeVersion(2, 0, onlyActivePeers).size() == 0);
    CPPUNIT_ASSERT(peerRegistry.getPeersWithTypeVersion(2, 1, onlyActivePeers).size() == 1);
    CPPUNIT_ASSERT(peerRegistry.getPeersWithTypeVersion(3, 0, onlyActivePeers).size() == 0);
    CPPUNIT_ASSERT(peerRegistry.getPeersWithTypeVersion(3, 1, onlyActivePeers).size() == 0);
    CPPUNIT_ASSERT(peerRegistry.getAnyPeerWithTypeVersion(1, 0, onlyActivePeers).get() != nullptr);
    CPPUNIT_ASSERT(peerRegistry.getAnyPeerWithTypeVersion(1, 0, onlyActivePeers)->UniqueId == id10);
    CPPUNIT_ASSERT(peerRegistry.getAnyPeerWithTypeVersion(1, 1, onlyActivePeers).get() == nullptr);
    CPPUNIT_ASSERT(peerRegistry.getAnyPeerWithTypeVersion(2, 0, onlyActivePeers).get() == nullptr);
    CPPUNIT_ASSERT(peerRegistry.getAnyPeerWithTypeVersion(2, 1, onlyActivePeers).get() != nullptr);
    CPPUNIT_ASSERT(peerRegistry.getAnyPeerWithTypeVersion(2, 1, onlyActivePeers)->UniqueId == id21);
    CPPUNIT_ASSERT(peerRegistry.getAnyPeerWithTypeVersion(3, 0, onlyActivePeers).get() == nullptr);
    CPPUNIT_ASSERT(peerRegistry.getAnyPeerWithTypeVersion(3, 1, onlyActivePeers).get() == nullptr);
    // Contains no peers
    CPPUNIT_ASSERT(peerRegistry.containsPeerWithId(id10, onlyActivePeers));
    CPPUNIT_ASSERT(!peerRegistry.containsPeerWithId(id11, onlyActivePeers));
    CPPUNIT_ASSERT(!peerRegistry.containsPeerWithId(id12, onlyActivePeers));
    CPPUNIT_ASSERT(!peerRegistry.containsPeerWithId(id20, onlyActivePeers));
    CPPUNIT_ASSERT(peerRegistry.containsPeerWithId(id21, onlyActivePeers));
    CPPUNIT_ASSERT(!peerRegistry.containsPeerWithId(id30, onlyActivePeers));
    CPPUNIT_ASSERT(peerRegistry.getPeerWithId(id10, onlyActivePeers).get() != nullptr);
    CPPUNIT_ASSERT(peerRegistry.getPeerWithId(id10, onlyActivePeers)->UniqueId == id10);
    CPPUNIT_ASSERT(peerRegistry.getPeerWithId(id11, onlyActivePeers).get() == nullptr);
    CPPUNIT_ASSERT(peerRegistry.getPeerWithId(id12, onlyActivePeers).get() == nullptr);
    CPPUNIT_ASSERT(peerRegistry.getPeerWithId(id20, onlyActivePeers).get() == nullptr);
    CPPUNIT_ASSERT(peerRegistry.getPeerWithId(id21, onlyActivePeers).get() != nullptr);
    CPPUNIT_ASSERT(peerRegistry.getPeerWithId(id21, onlyActivePeers)->UniqueId == id21);
    CPPUNIT_ASSERT(peerRegistry.getPeerWithId(id30, onlyActivePeers).get() == nullptr);
}


void checkRegistry_10_11_12_21_22(zmf::discovery::PeerRegistryInternal& peerRegistry, bool onlyActivePeers) {
    CPPUNIT_ASSERT(peerRegistry.INTERNAL_getAllPeers(onlyActivePeers).size() == 5);
    CPPUNIT_ASSERT(peerRegistry.INTERNAL_getAllPeersByType(onlyActivePeers).size() == 2);
    // Contains no type of peers
    CPPUNIT_ASSERT(peerRegistry.containsPeerWithType(1, onlyActivePeers));
    CPPUNIT_ASSERT(peerRegistry.containsPeerWithType(2, onlyActivePeers));
    CPPUNIT_ASSERT(!peerRegistry.containsPeerWithType(3, onlyActivePeers));
    CPPUNIT_ASSERT(peerRegistry.getPeersWithType(1, onlyActivePeers).size() == 3);
    CPPUNIT_ASSERT(peerRegistry.getPeersWithType(2, onlyActivePeers).size() == 2);
    CPPUNIT_ASSERT(peerRegistry.getPeersWithType(3, onlyActivePeers).size() == 0);
    CPPUNIT_ASSERT(peerRegistry.getAnyPeerWithType(1, onlyActivePeers).get() != nullptr);
    CPPUNIT_ASSERT(peerRegistry.getAnyPeerWithType(2, onlyActivePeers).get() != nullptr);
    CPPUNIT_ASSERT(peerRegistry.getAnyPeerWithType(3, onlyActivePeers).get() == nullptr);
    // No types, not versions
    CPPUNIT_ASSERT(peerRegistry.containsPeerWithTypeVersion(1, 0, onlyActivePeers));
    CPPUNIT_ASSERT(peerRegistry.containsPeerWithTypeVersion(1, 1, onlyActivePeers));
    CPPUNIT_ASSERT(peerRegistry.containsPeerWithTypeVersion(2, 0, onlyActivePeers));
    CPPUNIT_ASSERT(peerRegistry.containsPeerWithTypeVersion(2, 1, onlyActivePeers));
    CPPUNIT_ASSERT(!peerRegistry.containsPeerWithTypeVersion(3, 0, onlyActivePeers));
    CPPUNIT_ASSERT(!peerRegistry.containsPeerWithTypeVersion(3, 1, onlyActivePeers));
    CPPUNIT_ASSERT(peerRegistry.getPeersWithTypeVersion(1, 0, onlyActivePeers).size() == 1);
    CPPUNIT_ASSERT(peerRegistry.getPeersWithTypeVersion(1, 1, onlyActivePeers).size() == 2);
    CPPUNIT_ASSERT(peerRegistry.getPeersWithTypeVersion(2, 0, onlyActivePeers).size() == 1);
    CPPUNIT_ASSERT(peerRegistry.getPeersWithTypeVersion(2, 1, onlyActivePeers).size() == 1);
    CPPUNIT_ASSERT(peerRegistry.getPeersWithTypeVersion(3, 0, onlyActivePeers).size() == 0);
    CPPUNIT_ASSERT(peerRegistry.getPeersWithTypeVersion(3, 1, onlyActivePeers).size() == 0);
    CPPUNIT_ASSERT(peerRegistry.getAnyPeerWithTypeVersion(1, 0, onlyActivePeers).get() != nullptr);
    CPPUNIT_ASSERT(peerRegistry.getAnyPeerWithTypeVersion(1, 0, onlyActivePeers)->UniqueId == id10);
    CPPUNIT_ASSERT(peerRegistry.getAnyPeerWithTypeVersion(1, 1, onlyActivePeers).get() != nullptr);
    CPPUNIT_ASSERT(peerRegistry.getAnyPeerWithTypeVersion(2, 0, onlyActivePeers).get() != nullptr);
    CPPUNIT_ASSERT(peerRegistry.getAnyPeerWithTypeVersion(2, 1, onlyActivePeers).get() != nullptr);
    CPPUNIT_ASSERT(peerRegistry.getAnyPeerWithTypeVersion(2, 1, onlyActivePeers)->UniqueId == id21);
    CPPUNIT_ASSERT(peerRegistry.getAnyPeerWithTypeVersion(3, 0, onlyActivePeers).get() == nullptr);
    CPPUNIT_ASSERT(peerRegistry.getAnyPeerWithTypeVersion(3, 1, onlyActivePeers).get() == nullptr);
    // Contains no peers
    CPPUNIT_ASSERT(peerRegistry.containsPeerWithId(id10, onlyActivePeers));
    CPPUNIT_ASSERT(peerRegistry.containsPeerWithId(id11, onlyActivePeers));
    CPPUNIT_ASSERT(peerRegistry.containsPeerWithId(id12, onlyActivePeers));
    CPPUNIT_ASSERT(peerRegistry.containsPeerWithId(id20, onlyActivePeers));
    CPPUNIT_ASSERT(peerRegistry.containsPeerWithId(id21, onlyActivePeers));
    CPPUNIT_ASSERT(!peerRegistry.containsPeerWithId(id30, onlyActivePeers));
    CPPUNIT_ASSERT(peerRegistry.getPeerWithId(id10, onlyActivePeers).get() != nullptr);
    CPPUNIT_ASSERT(peerRegistry.getPeerWithId(id10, onlyActivePeers)->UniqueId == id10);
    CPPUNIT_ASSERT(peerRegistry.getPeerWithId(id11, onlyActivePeers).get() != nullptr);
    CPPUNIT_ASSERT(peerRegistry.getPeerWithId(id12, onlyActivePeers).get() != nullptr);
    CPPUNIT_ASSERT(peerRegistry.getPeerWithId(id20, onlyActivePeers).get() != nullptr);
    CPPUNIT_ASSERT(peerRegistry.getPeerWithId(id21, onlyActivePeers).get() != nullptr);
    CPPUNIT_ASSERT(peerRegistry.getPeerWithId(id21, onlyActivePeers)->UniqueId == id21);
    CPPUNIT_ASSERT(peerRegistry.getPeerWithId(id30, onlyActivePeers).get() == nullptr);
}

void PeerRegistryTests::testPeerRegistry() {
    try {
        zmf::discovery::PeerRegistryInternal peerRegistry;

        // Creates adds and removes modules from peer registry

        shared_ptr<zmf::data::ModuleHandleInternal> testHandle10 = shared_ptr<zmf::data::ModuleHandleInternal>(
                new zmf::data::ModuleHandleInternal(id10, 0, "id10", false));
        shared_ptr<zmf::data::ModuleHandleInternal> testHandle11 = shared_ptr<zmf::data::ModuleHandleInternal>(
                new zmf::data::ModuleHandleInternal(id11, 1, "id11", false));
        shared_ptr<zmf::data::ModuleHandleInternal> testHandle12 = shared_ptr<zmf::data::ModuleHandleInternal>(
                new zmf::data::ModuleHandleInternal(id12, 1, "id12", false));
        shared_ptr<zmf::data::ModuleHandleInternal> testHandle20 = shared_ptr<zmf::data::ModuleHandleInternal>(
                new zmf::data::ModuleHandleInternal(id20, 0, "id20", false));
        shared_ptr<zmf::data::ModuleHandleInternal> testHandle21 = shared_ptr<zmf::data::ModuleHandleInternal>(
                new zmf::data::ModuleHandleInternal(id21, 1, "id21", false));

        // Registry must be empty at the beginning
        checkRegistryEmpty(peerRegistry, true);
        checkRegistryEmpty(peerRegistry, false);

        // Add the module 1:0 which is inactive
        peerRegistry.addModule(testHandle10, zmf::data::ModuleState::Inactive, std::vector<uint8_t>());
        checkRegistryEmpty(peerRegistry, true);
        checkRegistry_10(peerRegistry, false);
        CPPUNIT_ASSERT(peerRegistry.getPeerState(testHandle10) == zmf::data::ModuleState::Inactive);

        // Enable 1:0
        peerRegistry.INTERNAL_updatePeerState(testHandle10->UniqueId, zmf::data::ModuleState::Active);
        checkRegistry_10(peerRegistry, true);
        checkRegistry_10(peerRegistry, false);
        CPPUNIT_ASSERT(peerRegistry.getPeerState(testHandle10) == zmf::data::ModuleState::Active);

        // Disable 1:0
        peerRegistry.INTERNAL_updatePeerState(testHandle10->UniqueId, zmf::data::ModuleState::Inactive);
        checkRegistryEmpty(peerRegistry, true);
        checkRegistry_10(peerRegistry, false);
        CPPUNIT_ASSERT(peerRegistry.getPeerState(testHandle10) == zmf::data::ModuleState::Inactive);

        // Enable 1:0
        peerRegistry.INTERNAL_updatePeerState(testHandle10->UniqueId, zmf::data::ModuleState::Active);
        checkRegistry_10(peerRegistry, true);
        checkRegistry_10(peerRegistry, false);
        CPPUNIT_ASSERT(peerRegistry.getPeerState(testHandle10) == zmf::data::ModuleState::Active);



        // Add active module 2:1
        peerRegistry.addModule(testHandle21, zmf::data::ModuleState::Active, std::vector<uint8_t>());
        checkRegistry_10_21(peerRegistry, true);
        checkRegistry_10_21(peerRegistry, false);
        CPPUNIT_ASSERT(peerRegistry.getPeerState(testHandle10) == zmf::data::ModuleState::Active);
        CPPUNIT_ASSERT(peerRegistry.getPeerState(testHandle21) == zmf::data::ModuleState::Active);

        // Disable 2:1
        peerRegistry.INTERNAL_updatePeerState(testHandle21->UniqueId, zmf::data::ModuleState::Inactive);
        checkRegistry_10(peerRegistry, true);
        checkRegistry_10_21(peerRegistry, false);
        CPPUNIT_ASSERT(peerRegistry.getPeerState(testHandle10) == zmf::data::ModuleState::Active);
        CPPUNIT_ASSERT(peerRegistry.getPeerState(testHandle21) == zmf::data::ModuleState::Inactive);

        // Enable module 2:1
        peerRegistry.INTERNAL_updatePeerState(testHandle21->UniqueId, zmf::data::ModuleState::Active);
        checkRegistry_10_21(peerRegistry, true);
        checkRegistry_10_21(peerRegistry, false);
        CPPUNIT_ASSERT(peerRegistry.getPeerState(testHandle10) == zmf::data::ModuleState::Active);
        CPPUNIT_ASSERT(peerRegistry.getPeerState(testHandle21) == zmf::data::ModuleState::Active);

        // Remove the module 2:1
        peerRegistry.removeModule(id21);
        checkRegistry_10(peerRegistry, true);
        checkRegistry_10(peerRegistry, false);

        // Add again module 2:1
        peerRegistry.addModule(testHandle21, zmf::data::ModuleState::Active, std::vector<uint8_t>());
        checkRegistry_10_21(peerRegistry, true);
        checkRegistry_10_21(peerRegistry, false);


        // Add all except 30
        peerRegistry.addModule(testHandle11, zmf::data::ModuleState::Active, std::vector<uint8_t>());
        peerRegistry.addModule(testHandle12, zmf::data::ModuleState::Active, std::vector<uint8_t>());
        peerRegistry.addModule(testHandle20, zmf::data::ModuleState::Active, std::vector<uint8_t>());
        // Check
        checkRegistry_10_11_12_21_22(peerRegistry, true);
        checkRegistry_10_11_12_21_22(peerRegistry, false);


        // Clear registry
        peerRegistry.clearRegistry();
        // Registry must be empty after clear
        checkRegistryEmpty(peerRegistry, true);
        checkRegistryEmpty(peerRegistry, false);
    }
    catch (Poco::Exception exc) {
        std::cerr << "Test failed after exception: " << exc.message() << &std::endl;
        CPPUNIT_ASSERT(false);
    }
}