#ifndef ZMF_UT_ZMFSTRESSTEST_H
#define ZMF_UT_ZMFSTRESSTEST_H


#include <cppunit/extensions/HelperMacros.h>

/**
 * @details Tests the ZMF core with complicated scenarios
 * @author Jonas Grunert
 * @date created 8/7/15.
 */
class ZmfStressTest : public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE(ZmfStressTest);
        CPPUNIT_TEST(detailedTest);
        CPPUNIT_TEST(remoteOperationTest);
        CPPUNIT_TEST(stressTest);
    CPPUNIT_TEST_SUITE_END();

public:
    /**
     * Detailed test of state changes (enable, disable, stop)
     */
    void detailedTest();

    /**
     * Tests remote operations like enabling, disabling and stopping other modules.
     */
    void remoteOperationTest();

    /**
     * Tests fast state changes and dependencies. Can be configured how stressful, by default simpler test.
     */
    void stressTest();
};


#endif //ZMF_UT_ZMFSTRESSTEST_H
