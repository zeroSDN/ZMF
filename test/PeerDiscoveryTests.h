//
// Created by Matthias Blohm on 7/6/15.
//

#ifndef ZMF_UT_PEERDISCOVERYTESTS_H
#define ZMF_UT_PEERDISCOVERYTESTS_H

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>
#include "../../zmf/zmf/discovery_implementation/PeerDiscoveryService.hpp"
#include "../../zmf/zmf/core/ZmfCore.hpp"
#include "../../zmf/zmf/instance/ZmfInstance.hpp"
#include "TestModuleP.h"

using namespace CppUnit;

/**
      * @brief Unit test to check the functionality of the PeerDiscoveryService-class
      * @details This short unit test starts some ZMF-instances with their PeerDiscoveryServices and checks if they recognize each other correctly.
      * Also the detection of state changes to 'Inactive' or 'Dead' and the correct retrieval of name, version and additional state is tested.
      * More detailed tests concerning the correct transfer of information between instances can be found in the classes of the interoperability-test between Java and C++.
      * Warning:  there must be no other modules running in the network at this time, otherwise the test will fail.

      * @author Matthias Blohm
      * @date created on 7/6/15.
     */
class PeerDiscoveryTests : public CppUnit::TestCase {
CPPUNIT_TEST_SUITE(PeerDiscoveryTests);
        CPPUNIT_TEST(testPeerDiscovery);
    CPPUNIT_TEST_SUITE_END();

public:

    /**
     * Default constructor
     */
    PeerDiscoveryTests();

    /**
     * Starts the test of the peer discovery
     */
    void testPeerDiscovery();

};

#endif //ZMF_UT_PEERDISCOVERYTESTS_H
