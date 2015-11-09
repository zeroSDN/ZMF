//
// Created by Jonas Grunert on 6/26/15.
//

#ifndef DEMO_MODULE_DEMOMODULECONSOLE_H
#define DEMO_MODULE_DEMOMODULECONSOLE_H

#include <memory>
#include <map>
#include <zmf/IZmfInstanceController.hpp>
#include "DemoModule.hpp"

class DemoModuleConsole {

public:
    DemoModuleConsole();

    void startConsole();


private:
    std::map<zmf::data::ModuleUniqueId, std::shared_ptr<zmf::AbstractModule>> allModules_;
    std::map<zmf::data::ModuleUniqueId, DemoModule*> demoModules_;
    std::map<zmf::data::ModuleUniqueId, std::shared_ptr<zmf::IZmfInstanceController>> zmfInstances_;

    void printHelpText();

    void createNewDemoModule(uint64_t moduleInstanceId, uint16_t moduleVersion, bool mockModule);

    void createModule(std::shared_ptr<zmf::AbstractModule> newModule, bool mockModule);

    void stopModule();

    bool readModuleInstanceId(uint64_t& readRef);

    bool readModuleVersion(uint16_t& readRef);

    bool readModuleUniqueId(zmf::data::ModuleUniqueId& readRef);

    bool tryStringToModuleId(std::string str, zmf::data::ModuleUniqueId& outRef);
};


#endif //DEMO_MODULE_DEMOMODULECONSOLE_H
