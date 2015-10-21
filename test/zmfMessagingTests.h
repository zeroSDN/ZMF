#ifndef ZMF_UT_ZMFMESSAGINGTESTS_H
#define ZMF_UT_ZMFMESSAGINGTESTS_H

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>
#include <core/ModuleEventDispatcher.hpp>

/**
 * @brief Unit tests for the event dispatcher class
 * @author Jan Strauß
 * @date created on 7/9/15.
*/

class zmfMessagingTests : public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE(zmfMessagingTests);
        CPPUNIT_TEST(testMessaging);
    CPPUNIT_TEST_SUITE_END();
public:
    /*
     * @description test the event dispatcher functionality
     */
    void testMessaging();

    void onSub(const zmf::data::ZmfMessage& msg, const zmf::data::ModuleUniqueId& sender);

private:
    std::mutex m;
    std::condition_variable cv;
    bool subReceived = false;
};

#endif //ZMF_UT_ZMFMESSAGINGTESTS_H
