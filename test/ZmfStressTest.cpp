/**
 * @author Jonas Grunert
 * @date created 8/7/15.
 */

#include "ZmfStressTest.h"
#include "TestModule.hpp"
#include <stdio.h>

void ZmfStressTest::detailedTest() {
    std::cout << "----------------------- STARTING detailedTest -----------------------" << &std::endl;

    zmf::data::ModuleUniqueId id_10 = zmf::data::ModuleUniqueId(1, 0);
    zmf::data::ModuleUniqueId id_11 = zmf::data::ModuleUniqueId(1, 1);
    zmf::data::ModuleUniqueId id_12 = zmf::data::ModuleUniqueId(1, 2);
    zmf::data::ModuleUniqueId id_20 = zmf::data::ModuleUniqueId(2, 0);
    zmf::data::ModuleUniqueId id_21 = zmf::data::ModuleUniqueId(2, 1);
    zmf::data::ModuleUniqueId id_30 = zmf::data::ModuleUniqueId(3, 0);
    zmf::data::ModuleUniqueId id_31 = zmf::data::ModuleUniqueId(3, 1);

    auto module_10_0 = std::make_shared<TestModule>(id_10, 0, "module_10_0",
                                                    std::vector<zmf::ModuleDependency>());
    auto module_11_0 = std::make_shared<TestModule>(id_11, 0, "module_11_0",
                                                    std::vector<zmf::ModuleDependency>());
    auto module_12_1 = std::make_shared<TestModule>(id_12, 1, "module_12_1",
                                                    std::vector<zmf::ModuleDependency>());

    auto module_20_0 = std::make_shared<TestModule>(id_20, 0, "module_20_0",
                                                    std::vector<zmf::ModuleDependency>());
    auto module_21_1 = std::make_shared<TestModule>(id_21, 1, "module_21_1",
                                                    std::vector<zmf::ModuleDependency>());

    auto module_30_0 = std::make_shared<TestModule>(id_30, 0, "module_30_0",
                                                    std::vector<zmf::ModuleDependency>());
    auto module_31_1 = std::make_shared<TestModule>(id_31, 1, "module_31_1",
                                                    std::vector<zmf::ModuleDependency>());



    ////// Auto enable, disable, enable, requestStop
    // Test start with auto-enable of module_10_0
    auto handle_10_0 = zmf::instance::ZmfInstance::startInstance(module_10_0, true, true, true, false, false);
    for (int i = 0; i < 10; i++) {
        if (module_10_0->isEnabled()) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    CPPUNIT_ASSERT(module_10_0->isEnabled());

    // Test requestStopInstance of module_10_0
    handle_10_0->requestDisableModule();
    for (int i = 0; i < 10; i++) {
        if (!module_10_0->isEnabled()) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    CPPUNIT_ASSERT(!module_10_0->isEnabled());

    // Test requestEnableModule of module_10_0
    handle_10_0->requestEnableModule();
    for (int i = 0; i < 10; i++) {
        if (module_10_0->isEnabled()) {
            break;
        }//
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    CPPUNIT_ASSERT(module_10_0->isEnabled());

    // Test requestStopInstance of module_10_0
    handle_10_0->requestStopInstance();
    handle_10_0->joinExecution();
    CPPUNIT_ASSERT(!module_10_0->isEnabled());



    ////// No Auto enable, enable, stop
    // Test start with auto-enable of module_10_0
    handle_10_0 = zmf::instance::ZmfInstance::startInstance(module_10_0, true, false, true, false, false);
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    CPPUNIT_ASSERT(!module_10_0->isEnabled());

    // Test requestEnableModule of module_10_0
    handle_10_0->requestEnableModule();
    for (int i = 0; i < 10; i++) {
        if (module_10_0->isEnabled()) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    CPPUNIT_ASSERT(module_10_0->isEnabled());

    // Test stopInstance of module_10_0
    handle_10_0->stopInstance();
    CPPUNIT_ASSERT(!module_10_0->isEnabled());

    std::cout << "----------------------- FINISHED detailedTest -----------------------" << &std::endl;
}


void ZmfStressTest::remoteOperationTest() {
    std::cout << "----------------------- STARTING remoteOperationTest -----------------------" << &std::endl;

    zmf::data::ModuleUniqueId id_10 = zmf::data::ModuleUniqueId(1, 0);
    zmf::data::ModuleUniqueId id_11 = zmf::data::ModuleUniqueId(1, 1);

    auto module_10_0 = std::make_shared<TestModule>(id_10, 0, "module_10_0",
                                                    std::vector<zmf::ModuleDependency>());
    auto module_11_0 = std::make_shared<TestModule>(id_11, 0, "module_11_0",
                                                    std::vector<zmf::ModuleDependency>());



    ////// Auto enable, disable, enable, requestStop
    // Test start with auto-enable of module_10_0
    auto handle_10_0 = zmf::instance::ZmfInstance::startInstance(module_10_0, true, true, true, false, false);
    auto handle_11_0 = zmf::instance::ZmfInstance::startInstance(module_11_0, true, true, true, false, false);
    for (int i = 0; i < 10; i++) {
        if (module_10_0->isEnabled() && module_11_0->isEnabled()) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    CPPUNIT_ASSERT(module_10_0->isEnabled());

    // Test requestStopInstance of module_10_0
    handle_11_0->requestDisableRemoteInstance(id_10, 1000);
    for (int i = 0; i < 10; i++) {
        if (!module_10_0->isEnabled()) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    CPPUNIT_ASSERT(!module_10_0->isEnabled());

    // Test requestEnableRemoteInstance of module_10_0
    handle_11_0->requestEnableRemoteInstance(id_10, 1000);
    for (int i = 0; i < 10; i++) {
        if (module_10_0->isEnabled()) {
            break;
        }//
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    CPPUNIT_ASSERT(module_10_0->isEnabled());

    // Test requestStopInstance of module_10_0
    handle_11_0->requestStopRemoteInstance(id_10, 1000);
    handle_10_0->joinExecution();
    CPPUNIT_ASSERT(!module_10_0->isEnabled());

    std::cout << "----------------------- FINISHED remoteOperationTest -----------------------" << &std::endl;
}


void ZmfStressTest::stressTest() {

    std::cout << "----------------------- STARTING stressTest -----------------------" << &std::endl;

    size_t NUM_TEST_MODULES = 5;

    /*zmf::data::ModuleUniqueId id_10 = zmf::data::ModuleUniqueId(1, 0);
    zmf::data::ModuleUniqueId id_11 = zmf::data::ModuleUniqueId(1, 1);
    zmf::data::ModuleUniqueId id_12 = zmf::data::ModuleUniqueId(1, 2);
    zmf::data::ModuleUniqueId id_20 = zmf::data::ModuleUniqueId(2, 0);
    zmf::data::ModuleUniqueId id_21 = zmf::data::ModuleUniqueId(2, 1);
    zmf::data::ModuleUniqueId id_30 = zmf::data::ModuleUniqueId(3, 0);
    zmf::data::ModuleUniqueId id_31 = zmf::data::ModuleUniqueId(3, 1);

    auto module_10_0 = std::make_shared<TestModule>(id_10, 0, "module_10_0",
                                                    std::vector<zmf::ModuleDependency>());*/

    // Modules with no dependencies, auto enable
    std::vector<std::shared_ptr<zmf::AbstractModule>> modules1;
    std::vector<std::shared_ptr<zmf::IZmfInstanceController>> controller1;

    // Modules with dependencies to 1 - auto enable
    std::vector<std::shared_ptr<zmf::AbstractModule>> modules2;
    std::vector<std::shared_ptr<zmf::IZmfInstanceController>> controller2;

    // Modules with dependencies to 1, 2 - no auto enable
    std::vector<std::shared_ptr<zmf::AbstractModule>> modules3;
    std::vector<std::shared_ptr<zmf::IZmfInstanceController>> controller3;

    // Modules with dependencies to 1, 2, 3 - auto enable
    std::vector<std::shared_ptr<zmf::AbstractModule>> modules4;
    std::vector<std::shared_ptr<zmf::IZmfInstanceController>> controller4;

    // Modules with dependencies to 1, 2, 3, 4 - no auto enable
    std::vector<std::shared_ptr<zmf::AbstractModule>> modules5;
    std::vector<std::shared_ptr<zmf::IZmfInstanceController>> controller5;

    // Modules with dependencies to 1, 2, 3, 4, 5 - no auto enable
    std::vector<std::shared_ptr<zmf::AbstractModule>> modules6;
    std::vector<std::shared_ptr<zmf::IZmfInstanceController>> controller6;


    // Start modules 1
    for (size_t i = 0; i < NUM_TEST_MODULES; i++) {
        auto moduleTmp = std::make_shared<TestModule>(zmf::data::ModuleUniqueId(1, i), i,
                                                      "module_1:" + std::to_string(i),
                                                      std::vector<zmf::ModuleDependency>());
        modules1.push_back(moduleTmp);
        controller1.push_back(zmf::instance::ZmfInstance::startInstance(moduleTmp, true, true, true, false, false));
    }

    // Test if modules started
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    for (size_t i = 0; i < NUM_TEST_MODULES; i++) {
        CPPUNIT_ASSERT(controller1[i]->isStarted());
        CPPUNIT_ASSERT(!controller1[i]->isStopped());
        CPPUNIT_ASSERT(modules1[i]->isEnabled());
    }

    std::cout << "------------ Finished modules1 test ------------" << &std::endl;


    // Start modules 2
    for (size_t i = 0; i < NUM_TEST_MODULES; i++) {
        std::vector<zmf::ModuleDependency> dependenciesTmp = {zmf::ModuleDependency(1, i)};
        auto moduleTmp = std::make_shared<TestModule>(zmf::data::ModuleUniqueId(2, i), i,
                                                      "module_2:" + std::to_string(i),
                                                      dependenciesTmp);
        modules2.push_back(moduleTmp);
        controller2.push_back(zmf::instance::ZmfInstance::startInstance(moduleTmp, true, true, true, false, false));
    }

    // Test if modules started
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    for (size_t i = 0; i < NUM_TEST_MODULES; i++) {
        CPPUNIT_ASSERT(controller2[i]->isStarted());
        CPPUNIT_ASSERT(!controller1[i]->isStopped());
        CPPUNIT_ASSERT(modules2[i]->isEnabled());
    }

    std::cout << "------------ Finished modules2 test ------------" << &std::endl;


    // Start modules 3, no auto-enable
    for (size_t i = 0; i < NUM_TEST_MODULES; i++) {
        std::vector<zmf::ModuleDependency> dependenciesTmp = {zmf::ModuleDependency(1, i), zmf::ModuleDependency(2, i)};
        auto moduleTmp = std::make_shared<TestModule>(zmf::data::ModuleUniqueId(3, i), i,
                                                      "module_3:" + std::to_string(i),
                                                      dependenciesTmp);
        modules3.push_back(moduleTmp);
        controller3.push_back(zmf::instance::ZmfInstance::startInstance(moduleTmp, true, false, true, false, false));
    }
    // Start modules 4, auto-enable
    for (size_t i = 0; i < NUM_TEST_MODULES; i++) {
        std::vector<zmf::ModuleDependency> dependenciesTmp = {zmf::ModuleDependency(1, i), zmf::ModuleDependency(2, i),
                                                              zmf::ModuleDependency(3, i)};
        auto moduleTmp = std::make_shared<TestModule>(zmf::data::ModuleUniqueId(4, i), i,
                                                      "module_4:" + std::to_string(i),
                                                      dependenciesTmp);
        modules4.push_back(moduleTmp);
        controller4.push_back(zmf::instance::ZmfInstance::startInstance(moduleTmp, true, true, true, false, false));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    // Test if modules not enabled (no auto enable for 3; 4 depends on 3)
    for (size_t i = 0; i < NUM_TEST_MODULES; i++) {
        CPPUNIT_ASSERT(controller3[i]->isStarted());
        CPPUNIT_ASSERT(controller4[i]->isStarted());
        CPPUNIT_ASSERT(!modules3[i]->isEnabled());
        CPPUNIT_ASSERT(!modules4[i]->isEnabled());
    }

    // Enable modules 3
    for (size_t i = 0; i < NUM_TEST_MODULES; i++) {
        controller3[i]->requestEnableModule();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    // Test if modules enabled (enabled for 3; 4 depends on 3)
    for (size_t i = 0; i < NUM_TEST_MODULES; i++) {
        CPPUNIT_ASSERT(controller3[i]->isStarted());
        CPPUNIT_ASSERT(controller4[i]->isStarted());
        CPPUNIT_ASSERT(modules3[i]->isEnabled());
        CPPUNIT_ASSERT(modules4[i]->isEnabled());
    }

    std::cout << "------------ Finished modules 3/4 enable test ------------" << &std::endl;


    // Disable modules 2
    for (size_t i = 0; i < NUM_TEST_MODULES; i++) {
        controller2[i]->requestDisableModule();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    // Test if modules 1 enabled and 2,3,4 disabled
    for (size_t i = 0; i < NUM_TEST_MODULES; i++) {
        CPPUNIT_ASSERT(controller1[i]->isStarted());
        CPPUNIT_ASSERT(controller2[i]->isStarted());
        CPPUNIT_ASSERT(controller3[i]->isStarted());
        CPPUNIT_ASSERT(controller4[i]->isStarted());
        CPPUNIT_ASSERT(modules1[i]->isEnabled());
        CPPUNIT_ASSERT(!modules2[i]->isEnabled());
        CPPUNIT_ASSERT(!modules3[i]->isEnabled());
        CPPUNIT_ASSERT(!modules4[i]->isEnabled());
    }

    std::cout << "------------ Finished disable modules2 test ------------" << &std::endl;


    // Re-enable modules 2,3 - 4 is auto-enabled
    for (size_t i = 0; i < NUM_TEST_MODULES; i++) {
        controller2[i]->requestEnableModule();
        // Modules waiting for re-enable for dependencies but requestDisable sops this
        controller4[i]->requestDisableModule();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    // Test if modules 1 enabled and 2,3,4 disabled
    for (size_t i = 0; i < NUM_TEST_MODULES; i++) {
        CPPUNIT_ASSERT(controller1[i]->isStarted());
        CPPUNIT_ASSERT(controller2[i]->isStarted());
        CPPUNIT_ASSERT(controller3[i]->isStarted());
        CPPUNIT_ASSERT(controller4[i]->isStarted());
        CPPUNIT_ASSERT(modules1[i]->isEnabled());
        CPPUNIT_ASSERT(modules2[i]->isEnabled());
        CPPUNIT_ASSERT(modules3[i]->isEnabled());
        CPPUNIT_ASSERT(!modules4[i]->isEnabled());
    }

    for (size_t i = 0; i < NUM_TEST_MODULES; i++) {
        controller4[i]->requestEnableModule();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    // Test if modules 1 enabled and 2,3,4 disabled
    for (size_t i = 0; i < NUM_TEST_MODULES; i++) {
        CPPUNIT_ASSERT(controller1[i]->isStarted());
        CPPUNIT_ASSERT(controller2[i]->isStarted());
        CPPUNIT_ASSERT(controller3[i]->isStarted());
        CPPUNIT_ASSERT(controller4[i]->isStarted());
        CPPUNIT_ASSERT(modules1[i]->isEnabled());
        CPPUNIT_ASSERT(modules2[i]->isEnabled());
        CPPUNIT_ASSERT(modules3[i]->isEnabled());
        CPPUNIT_ASSERT(modules4[i]->isEnabled());
    }

    std::cout << "------------ Finished re-enable modules2 test ------------" << &std::endl;


    // Start modules 5, auto-enable
    for (size_t i = 0; i < NUM_TEST_MODULES; i++) {
        std::vector<zmf::ModuleDependency> dependenciesTmp = {zmf::ModuleDependency(1, i), zmf::ModuleDependency(2, i),
                                                              zmf::ModuleDependency(3, i), zmf::ModuleDependency(4, i)};
        auto moduleTmp = std::make_shared<TestModule>(zmf::data::ModuleUniqueId(5, i), i,
                                                      "module_5:" + std::to_string(i),
                                                      dependenciesTmp);
        modules5.push_back(moduleTmp);
        controller5.push_back(zmf::instance::ZmfInstance::startInstance(moduleTmp, true, false, true, false, false));
    }
    // Start modules 6, auto-enable
    for (size_t i = 0; i < NUM_TEST_MODULES; i++) {
        std::vector<zmf::ModuleDependency> dependenciesTmp = {zmf::ModuleDependency(1, i), zmf::ModuleDependency(2, i),
                                                              zmf::ModuleDependency(3, i), zmf::ModuleDependency(4, i),
                                                              zmf::ModuleDependency(5, i)};
        auto moduleTmp = std::make_shared<TestModule>(zmf::data::ModuleUniqueId(6, i), i,
                                                      "module_6:" + std::to_string(i),
                                                      dependenciesTmp);
        modules6.push_back(moduleTmp);
        controller6.push_back(zmf::instance::ZmfInstance::startInstance(moduleTmp, true, false, true, false, false));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    // Test if modules 1,2,3,4 enabled and 5,6 disabled
    for (size_t i = 0; i < NUM_TEST_MODULES; i++) {
        CPPUNIT_ASSERT(modules1[i]->isEnabled());
        CPPUNIT_ASSERT(modules2[i]->isEnabled());
        CPPUNIT_ASSERT(modules3[i]->isEnabled());
        CPPUNIT_ASSERT(modules4[i]->isEnabled());
        CPPUNIT_ASSERT(!modules5[i]->isEnabled());
        CPPUNIT_ASSERT(!modules6[i]->isEnabled());
    }

    std::cout << "------------ Finished start modules5 test ------------" << &std::endl;


    // Enable modules 5
    for (size_t i = 0; i < NUM_TEST_MODULES; i++) {
        controller5[i]->requestEnableModule();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    // Test if modules 1,2,3,4,5 enabled and 6 disabled
    for (size_t i = 0; i < NUM_TEST_MODULES; i++) {
        CPPUNIT_ASSERT(modules1[i]->isEnabled());
        CPPUNIT_ASSERT(modules2[i]->isEnabled());
        CPPUNIT_ASSERT(modules3[i]->isEnabled());
        CPPUNIT_ASSERT(modules4[i]->isEnabled());
        CPPUNIT_ASSERT(modules5[i]->isEnabled());
        CPPUNIT_ASSERT(!modules6[i]->isEnabled());
    }

    std::cout << "------------ Finished enable modules5 test ------------" << &std::endl;


    // Enable modules 6
    for (size_t i = 0; i < NUM_TEST_MODULES; i++) {
        controller6[i]->requestEnableModule();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    // Test if modules 1,2,3,4,5,6 enabled
    for (size_t i = 0; i < NUM_TEST_MODULES; i++) {
        CPPUNIT_ASSERT(modules1[i]->isEnabled());
        CPPUNIT_ASSERT(modules2[i]->isEnabled());
        CPPUNIT_ASSERT(modules3[i]->isEnabled());
        CPPUNIT_ASSERT(modules4[i]->isEnabled());
        CPPUNIT_ASSERT(modules5[i]->isEnabled());
        CPPUNIT_ASSERT(modules6[i]->isEnabled());
    }

    std::cout << "------------ Finished enable modules6 test ------------" << &std::endl;


    // Stop modules 4, should also disable 5 and 6
    for (size_t i = 0; i < NUM_TEST_MODULES; i++) {
        controller4[i]->requestStopInstance();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    // Test if modules 1,2,3,5,6 enabled
    for (size_t i = 0; i < NUM_TEST_MODULES; i++) {
        CPPUNIT_ASSERT(modules1[i]->isEnabled());
        CPPUNIT_ASSERT(modules2[i]->isEnabled());
        CPPUNIT_ASSERT(modules3[i]->isEnabled());
        CPPUNIT_ASSERT(!modules5[i]->isEnabled());
        CPPUNIT_ASSERT(!modules6[i]->isEnabled());
    }

    std::cout << "------------ Finished stop modules4 test ------------" << &std::endl;


    // Stop modules 2, should also disable 3
    for (size_t i = 0; i < NUM_TEST_MODULES; i++) {
        controller2[i]->requestDisableModule();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    // Test if modules 1,2,3,5,6 enabled
    for (size_t i = 0; i < NUM_TEST_MODULES; i++) {
        CPPUNIT_ASSERT(modules1[i]->isEnabled());
        CPPUNIT_ASSERT(!modules2[i]->isEnabled());
        CPPUNIT_ASSERT(!modules3[i]->isEnabled());
        CPPUNIT_ASSERT(!modules5[i]->isEnabled());
        CPPUNIT_ASSERT(!modules6[i]->isEnabled());
    }

    std::cout << "------------ Finished disable modules2 test ------------" << &std::endl;


    // Stop modules 1,2,3,5,6
    for (size_t i = 0; i < NUM_TEST_MODULES; i++) {
        controller1[i]->requestStopInstance();
        controller2[i]->requestStopInstance();
        controller3[i]->requestStopInstance();
        controller4[i]->requestStopInstance();
        controller5[i]->requestStopInstance();
        controller6[i]->requestStopInstance();
    }
    for (size_t i = 0; i < NUM_TEST_MODULES; i++) {
        controller1[i]->joinExecution();
        controller2[i]->joinExecution();
        controller3[i]->joinExecution();
        controller4[i]->joinExecution();
        controller5[i]->joinExecution();
        controller6[i]->joinExecution();
    }

    // Test if modules 1,2,3,4,5,6 stopped
    for (size_t i = 0; i < NUM_TEST_MODULES; i++) {
        CPPUNIT_ASSERT(!controller1[i]->isStarted());
        CPPUNIT_ASSERT(!controller2[i]->isStarted());
        CPPUNIT_ASSERT(!controller3[i]->isStarted());
        CPPUNIT_ASSERT(!controller4[i]->isStarted());
        CPPUNIT_ASSERT(!controller5[i]->isStarted());
        CPPUNIT_ASSERT(!controller6[i]->isStarted());
        CPPUNIT_ASSERT(controller1[i]->isStopped());
        CPPUNIT_ASSERT(controller2[i]->isStopped());
        CPPUNIT_ASSERT(controller3[i]->isStopped());
        CPPUNIT_ASSERT(controller4[i]->isStopped());
        CPPUNIT_ASSERT(controller5[i]->isStopped());
        CPPUNIT_ASSERT(controller6[i]->isStopped());
    }

    std::cout << "------------ Joined all executions ------------" << &std::endl;


    modules1.clear();
    modules2.clear();
    modules3.clear();
    modules4.clear();
    modules5.clear();
    modules6.clear();
    controller1.clear();
    controller2.clear();
    controller3.clear();
    controller4.clear();
    controller5.clear();
    controller6.clear();

    std::cout << "----------------------- Finished stressTest -----------------------" << &std::endl;
}
