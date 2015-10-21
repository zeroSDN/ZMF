
#ifndef ZMF_UT_ZMFINTEROPTTEST_H
#define ZMF_UT_ZMFINTEROPTTEST_H


#include <cppunit/extensions/HelperMacros.h>

/**
 * @details Tests interoperability of ZMF and JMF
 * @author Matthias Blohm
 * @date created 8/6/15.
 */
class zmfInteroptTest : public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE(zmfInteroptTest);
        CPPUNIT_TEST(interoptTest);
    CPPUNIT_TEST_SUITE_END();

public:
    /**
     * Tests communication between a ZMF and a JMF instance
     */
    void interoptTest();
};

#endif //ZMF_UT_ZMFINTEROPTTEST_H
