//
// Created by Matthias Blohm on 7/6/15.
//

#include "PeerDiscoveryTests.h"
#include <memory>

PeerDiscoveryTests::PeerDiscoveryTests() {
}

void PeerDiscoveryTests::testPeerDiscovery() {
    try {


        uint64_t version = 1;
        TestModuleP* demoModule = new TestModuleP(1, version);
        TestModuleP* demoModule2 = new TestModuleP(2, version);

        std::shared_ptr<zmf::AbstractModule> testModule10 = std::shared_ptr<TestModuleP>(demoModule);
        std::shared_ptr<zmf::AbstractModule> testModule20 = std::shared_ptr<TestModuleP>(demoModule2);

        zmf::data::ModuleUniqueId id10 = testModule10->getUniqueId();
        zmf::data::ModuleUniqueId id20 = testModule20->getUniqueId();

        //Test starting of modules
        std::shared_ptr<zmf::IZmfInstanceController> newZmfInstance = zmf::instance::ZmfInstance::startInstance(
                testModule10, std::initializer_list<zmf::instance::ZmfInstance::StartOption>{
                zmf::instance::ZmfInstance::StartOption::NO_EXIT_WHEN_ENABLE_FAILED,
                zmf::instance::ZmfInstance::StartOption::NO_PEER_DISCOVERY_WAIT});

        std::this_thread::sleep_for(std::chrono::milliseconds(500));


        std::shared_ptr<zmf::discovery::PeerRegistry> peerRegistry1 = testModule10->getZmf()->getPeerRegistry();
        zmf::discovery::PeerRegistryInternal* peerRegistryInternal = static_cast<zmf::discovery::PeerRegistryInternal*>(peerRegistry1.get());

        std::shared_ptr<zmf::IZmfInstanceController> newZmfInstance2 = zmf::instance::ZmfInstance::startInstance(
                testModule20, {
                zmf::instance::ZmfInstance::StartOption::NO_PEER_DISCOVERY_WAIT});

        std::this_thread::sleep_for(std::chrono::milliseconds(500));





        std::shared_ptr<zmf::discovery::PeerRegistry> peerRegistry2 = testModule20->getZmf()->getPeerRegistry();
        zmf::discovery::PeerRegistryInternal* peerRegistryInternal2 = static_cast<zmf::discovery::PeerRegistryInternal*>(peerRegistry2.get());

        // set additional state
        std::string addI1 = "M1";
        std::string addI2 = "M2";
        std::vector<uint8_t> addState1(addI1.begin(), addI1.end());
        std::vector<uint8_t> addState2(addI2.begin(), addI2.end());
        demoModule->setAdditionalState(addState1);
        demoModule2->setAdditionalState(addState2);

        std::this_thread::sleep_for(std::chrono::milliseconds(3000));

        std::map<zmf::data::ModuleUniqueId, std::shared_ptr<zmf::data::ModuleHandle>> reg1 = peerRegistryInternal->INTERNAL_getAllPeers(
                false);
        std::map<zmf::data::ModuleUniqueId, std::shared_ptr<zmf::data::ModuleHandle>> reg2 = peerRegistryInternal2->INTERNAL_getAllPeers(
                false);

        std::shared_ptr<zmf::data::ModuleHandle> handle10 = peerRegistry2->getPeerWithId(demoModule->getUniqueId());
        std::shared_ptr<zmf::data::ModuleHandle> handle20 = peerRegistry1->getPeerWithId(demoModule2->getUniqueId());


        // check if the modules registered only each other
        CPPUNIT_ASSERT(reg1.size() == 1);
        CPPUNIT_ASSERT(reg2.size() == 1);

        // check if the modules registered each other correctly
        CPPUNIT_ASSERT(reg1.count(id20) == 1);
        CPPUNIT_ASSERT(reg2.count(id10) == 1);

        // make sure both modules are enabled and active
        CPPUNIT_ASSERT(peerRegistryInternal->getPeerState(handle20) == zmf::data::ModuleState::Active);
        CPPUNIT_ASSERT(peerRegistryInternal2->getPeerState(handle10) == zmf::data::ModuleState::Active);

        std::vector<uint8_t> recAddState = peerRegistry2->getPeerAdditionalState(handle10);
        std::vector<uint8_t> recAddState2 = peerRegistry1->getPeerAdditionalState(handle20);

        // make sure the additional states were recognized correctly
        CPPUNIT_ASSERT(std::string(recAddState.begin(), recAddState.end()) == addI1 );
        CPPUNIT_ASSERT(std::string(recAddState2.begin(), recAddState2.end()) == addI2 );

        // make sure the version of the other module is recognized correctly
        CPPUNIT_ASSERT(handle10->Version == version);
        CPPUNIT_ASSERT(handle20->Version == version);

        // check correctness of the module name
        CPPUNIT_ASSERT(handle10->Name == "TestModuleP");
        CPPUNIT_ASSERT(handle20->Name == "TestModuleP");

        // test recognizing state changes
        newZmfInstance2->requestDisableModule();
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        CPPUNIT_ASSERT(peerRegistryInternal->getPeerState(handle20) == zmf::data::ModuleState::Inactive);

        newZmfInstance2->requestEnableModule();
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        CPPUNIT_ASSERT(peerRegistryInternal->getPeerState(handle20) == zmf::data::ModuleState::Active);

        newZmfInstance2->requestStopInstance();
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        reg1 = peerRegistryInternal->INTERNAL_getAllPeers(false);
        CPPUNIT_ASSERT(reg1.size() == 0);

        // stop also the other instance
        newZmfInstance->requestStopInstance();

    } catch (Poco::Exception exc) {
        std::cerr << "Test failed after exception: " << exc.message() << &std::endl;
        CPPUNIT_ASSERT(false);
    }
}
