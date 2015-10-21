//
// Created by zsdn on 8/6/15.
//

#include <AbstractModule.hpp>
#include <instance/ZmfInstance.hpp>
#include "zmfInteroptTest.h"
#include "TestModule.hpp"


void zmfInteroptTest::interoptTest() {
    zmf::data::ModuleUniqueId uniqueId = zmf::data::ModuleUniqueId(45, 23);
    uint16_t version = 11;
    std::string name = "KEK";
    std::vector<zmf::ModuleDependency> dependencies;

    std::cout << "qqqqqqqqqqqqqqq" << std::endl;

    auto module = std::make_shared<TestModule>(uniqueId, version, name, dependencies);

    auto handle = zmf::instance::ZmfInstance::startInstance(module, true, true, true, false, false);

    while (!module->isEnabled()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    int cnt = 5;
    while (cnt >= 0) {
        module->doPub();
        module->doReq();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        cnt--;
    }

    handle->requestStopInstance();

    handle->joinExecution();
}