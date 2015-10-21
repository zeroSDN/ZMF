#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include "zmfTests.h"
#include "PeerRegistryTests.h"
#include "PeerDiscoveryTests.h"
#include "zmqTests.h"
#include "zmfMessagingTests.h"
#include "zmfInteroptTest.h"
#include "ZmfStressTest.h"
#include <cppunit/TestResult.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TextOutputter.h>
#include <google/protobuf/stubs/common.h>


using namespace CppUnit;

CPPUNIT_TEST_SUITE_REGISTRATION(zmfTests);

CPPUNIT_TEST_SUITE_REGISTRATION(PeerRegistryTests);
CPPUNIT_TEST_SUITE_REGISTRATION(PeerDiscoveryTests);
CPPUNIT_TEST_SUITE_REGISTRATION(zmfMessagingTests);
CPPUNIT_TEST_SUITE_REGISTRATION(zmfInteroptTest);
CPPUNIT_TEST_SUITE_REGISTRATION(zmqTests);
CPPUNIT_TEST_SUITE_REGISTRATION(ZmfStressTest);


int main(int argc, char* argv[]) {

// informs test-listener about testresults
    CppUnit::TestResult testresult;

    // register listener for collecting the test-results
    CppUnit::TestResultCollector collectedresults;
    testresult.addListener(&collectedresults);

    // register listener for per-test progress output
    CppUnit::BriefTestProgressListener progress;
    testresult.addListener(&progress);

    TextUi::TestRunner runner;
    TestFactoryRegistry& registry = TestFactoryRegistry::getRegistry();
    runner.addTest(registry.makeTest());
    runner.run(testresult);

    // output results in compiler-format
    CppUnit::TextOutputter textOutputter(&collectedresults, std::cerr);
    textOutputter.write();

    // return 0 if tests were successful


    google::protobuf::ShutdownProtobufLibrary();

    return collectedresults.wasSuccessful() ? 0 : 1;
}