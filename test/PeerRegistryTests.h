#ifndef ZMF_UT_PEERREGISTRYTESTS_H
#define ZMF_UT_PEERREGISTRYTESTS_H

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

using namespace CppUnit;

/**
 * @details Tests the PeerRegistry service, how it recognizes, handles and stores changes of peers.
 * @author Jonas Grunert
 * @date created 7/6/15.
 */
class PeerRegistryTests : public CppUnit::TestCase {
CPPUNIT_TEST_SUITE(PeerRegistryTests);
        CPPUNIT_TEST(testPeerRegistry);
    CPPUNIT_TEST_SUITE_END();

public:
    PeerRegistryTests();

    /**
     * Tests a sequence of peer state changes
     */
    void testPeerRegistry();

};

#endif //ZMF_UT_PEERREGISTRYTESTS_H
