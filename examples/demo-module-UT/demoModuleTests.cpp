//
// Created by zsdn on 6/25/15.
//

#include "demoModuleTests.h"
demoModuleTests::demoModuleTests()
{

}

void demoModuleTests::testMethod1()
{
    CPPUNIT_ASSERT(true);
}

void demoModuleTests::testMethod2()
{
    CPPUNIT_ASSERT(true);
}

void demoModuleTests::testMethod3()
{
    CPPUNIT_ASSERT_EQUAL(i,5);
    CPPUNIT_ASSERT(true);

}