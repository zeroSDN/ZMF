#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include "demoModuleTests.h"

using namespace CppUnit;

CPPUNIT_TEST_SUITE_REGISTRATION(demoModuleTests);


int main (int argc, char* argv[]) {
    TextUi::TestRunner runner;
    TestFactoryRegistry &registry = TestFactoryRegistry::getRegistry();
    runner.addTest( registry.makeTest() );
    bool failed = runner.run();
    return !failed;

}