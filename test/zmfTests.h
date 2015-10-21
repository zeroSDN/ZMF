
#ifndef DEMO_MODULE_UT_DEMOMODULETESTS_H
#define DEMO_MODULE_UT_DEMOMODULETESTS_H

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>


using namespace CppUnit;
/**
 * @description Unit tests for testing various ZMF functions
 * @author Jonas Grunert
 * @author Tobias Korb
 * @date created on 7/9/15.
*/

class zmfTests : public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE(zmfTests);
        CPPUNIT_TEST(testConfig);
        CPPUNIT_TEST(testMessageTypeByteshift);
        CPPUNIT_TEST(testMessageTypeConstructorOverride);
        CPPUNIT_TEST(testMessageTypeMethods);
        CPPUNIT_TEST(testZMFDataTypes);
        CPPUNIT_TEST(testInstance);
    CPPUNIT_TEST_SUITE_END();

public:
    int i = 5;

    /**
     * Tests byte shifting of message types match bytes
     */
    void testMessageTypeByteshift();

    /**
     * Tests constuction of message type and append
     */
    void testMessageTypeConstructorOverride();

    /**
     * Tests various methods of message type such as append and override
     */
    void testMessageTypeMethods();

    /**
     * Tests providing of configuration values
     */
    void testConfig();

    /**
     * Tests the ZMF data types such as the Message class
     */
    void testZMFDataTypes();

    /**
     * Tests basic functions of a ZMF instance
     */
    void testInstance();

    virtual void setUp() override;

    virtual void tearDown() override;

private:


};

#endif //DEMO_MODULE_UT_DEMOMODULETESTS_H
