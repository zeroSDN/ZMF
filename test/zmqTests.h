#ifndef ZMF_UT_ZMQTESTS_H
#define ZMF_UT_ZMQTESTS_H

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>
#include "../zmf/messaging_implementation/ZmqZmfMessagingServiceImplementation.hpp"

using namespace CppUnit;

/**
 * @brief Unit tests for the messaging service zmq implementation
 * @author Jan Strauß
 * @date created on 7/9/15.
*/
class zmqTests : public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE(zmqTests);
        CPPUNIT_TEST(testZmq);
        //CPPUNIT_TEST(perfTestReqRep);
        //CPPUNIT_TEST(perfTestPubSub);
        CPPUNIT_TEST(testUnsubBug);
    CPPUNIT_TEST_SUITE_END();

public:
    /**
     * @description test all basic functionality of zmq
     */
    void testZmq();

    /**
     * @description test performance (throughput) of req/rep
     */
    void perfTestReqRep();

    /**
     * @description test performance (throughput) of pub/sub
     */
    void perfTestPubSub();

    /**
     * @description regression test for unsubscribe bug
     */
    void testUnsubBug();

    /**
     * @description test performance (throughput) combined
     */
    void perfRun(zmf::messaging::ZmqZmfMessagingServiceImplementation& serviceA) const;
};


#endif //ZMF_UT_ZMQTESTS_H
