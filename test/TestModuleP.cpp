//
// Created by Matthias Blohm on 7/6/15.
//

#include "TestModuleP.h"


TestModuleP::TestModuleP(uint64_t instanceId, uint16_t instanceVersion) :
        AbstractModule(zmf::data::ModuleUniqueId(MODULE_TYPE, instanceId), instanceVersion, "TestModuleP",
                       std::vector<zmf::ModuleDependency>()) {
}


bool TestModuleP::enable() {
    std::cout << "Enabled DemoModule" << &std::endl;
    return true;
}

void TestModuleP::disable() {
    std::cout << "Disabled DemoModule" << &std::endl;
}


void TestModuleP::setAdditionalState(std::vector<uint8_t> state) {
    getZmf()->onModuleAdditionalStateChanged(state);
}


