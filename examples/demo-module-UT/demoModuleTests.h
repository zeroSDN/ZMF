//
// Created by zsdn on 6/25/15.
//

#ifndef DEMO_MODULE_UT_DEMOMODULETESTS_H
#define DEMO_MODULE_UT_DEMOMODULETESTS_H

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

using namespace CppUnit;

class demoModuleTests : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(demoModuleTests);
        CPPUNIT_TEST(testMethod1);
        CPPUNIT_TEST(testMethod2);
        CPPUNIT_TEST(testMethod3);
        CPPUNIT_TEST_SUITE_END();

public:
    int i = 5;
    demoModuleTests();
    void testMethod1();
    void testMethod2();
    void testMethod3();

};
#endif //DEMO_MODULE_UT_DEMOMODULETESTS_H
