//
// Created by Jonas Grunert on 6/26/15.
//

#include "DemoModuleConsole.hpp"
//#include <ZmfMock.hpp>
#include <zmf/ZmfInstance.hpp>
#include <Poco/StringTokenizer.h>
//#include "../../modules/ExampleModuleA/ExampleModuleA.hpp"
//#include "../../modules/ExampleModuleB/ExampleModuleB.hpp"


DemoModuleConsole::DemoModuleConsole() {
}

void DemoModuleConsole::startConsole() {

    std::cout << &std::endl;
    std::cout << "---------- ZMF DemoModule Console ----------" << &std::endl;
    printHelpText();

    std::string line;

    // Warning! Quick&Dirty Code! Don't try this at home!
    while (true) {
        std::getline(std::cin, line);

        // Split commands
        Poco::StringTokenizer lineCommandSplit(line, " ", 0);
        if (lineCommandSplit.count() == 0) {
            std::cerr << ">>Nothing entered" << &std::endl;
            continue;
        }

        // TODO Check argument counts

        // General commands
        /*if (lineCommandSplit[0].compare("start") == 0) {
            std::cout << ">>Create new module and start ZMF instance" << &std::endl;
            uint16_t version = 0;
            if (lineCommandSplit.count() >= 3) {
                version = std::stoi(lineCommandSplit[2]);
            }
            createNewDemoModule(std::stoull(lineCommandSplit[1]), version, true);
            continue;
        } else */
        if (lineCommandSplit[0].compare("start") == 0) {
            std::cout << ">>Create new module and start ZMF instance" << &std::endl;
            uint16_t version = 0;
            if (lineCommandSplit.count() >= 3) {
                version = std::stoi(lineCommandSplit[2]);
            }
            createNewDemoModule(std::stoull(lineCommandSplit[1]), version, false);
            continue;
        }
            /*else if (lineCommandSplit[0].compare("starta") == 0) {
                std::cout << ">>Create new ExampleModuleA module and start ZMF instance" << &std::endl;
                createModule(std::shared_ptr<zmf::AbstractModule>(new ExampleModuleA(std::stoull(lineCommandSplit[1]))),
                             true);
                continue;
            }
            else if (lineCommandSplit[0].compare("startb") == 0) {
                std::cout << ">>Create new ExampleModuleB module and start ZMF instance" << &std::endl;
                createModule(std::shared_ptr<zmf::AbstractModule>(new ExampleModuleB(std::stoull(lineCommandSplit[1]))),
                             true);
                continue;
            }*/
            /*else if (lineCommandSplit[0].compare("starta") == 0) {
                std::cout << ">>Create new ExampleModuleA module and start ZMF instance" << &std::endl;
                createModule(std::shared_ptr<zmf::AbstractModule>(new ExampleModuleA(std::stoull(lineCommandSplit[1]))),
                             false);
                continue;
            }
            else if (lineCommandSplit[0].compare("startb") == 0) {
                std::cout << ">>Create new ExampleModuleB module and start ZMF instance" << &std::endl;
                createModule(std::shared_ptr<zmf::AbstractModule>(new ExampleModuleB(std::stoull(lineCommandSplit[1]))),
                             false);
                continue;
            }*/
        else if (line.compare("exit") == 0) {
            std::cout << ">>Exiting now" << &std::endl;
            return;
        }
        else if (lineCommandSplit[0].compare("reqrep") == 0) {
            zmf::data::ModuleUniqueId senderId;
            if (!tryStringToModuleId(lineCommandSplit[1], senderId)) {
                continue;
            }

            zmf::data::ModuleUniqueId targetId;
            if (!tryStringToModuleId(lineCommandSplit[2], targetId)) {
                continue;
            }

            std::cout << ">>Sending request/reply from " << senderId.getString() << " to " << targetId.getString() <<
            &std::endl;
            // TODO Check exists
            demoModules_[senderId]->testRequestReply(targetId);
        }
        else if (lineCommandSplit[0].compare("en") == 0 || lineCommandSplit[0].compare("enable") == 0) {
            zmf::data::ModuleUniqueId moduleId;
            if (!tryStringToModuleId(lineCommandSplit[1], moduleId)) {
                continue;
            }
            zmfInstances_[moduleId]->requestEnableModule();
        }
        else if (lineCommandSplit[0].compare("dis") == 0 || lineCommandSplit[0].compare("disable") == 0) {
            zmf::data::ModuleUniqueId moduleId;
            if (!tryStringToModuleId(lineCommandSplit[1], moduleId)) {
                continue;
            }
            zmfInstances_[moduleId]->requestDisableModule();
        }
        else if (lineCommandSplit[0].compare("stop") == 0) {
            zmf::data::ModuleUniqueId moduleId;
            if (!tryStringToModuleId(lineCommandSplit[1], moduleId)) {
                continue;
            }
            zmfInstances_[moduleId]->stopInstance();
            if (demoModules_.count(moduleId) > 0) {
                demoModules_.erase(moduleId);
            }
            if (allModules_.count(moduleId) > 0) {
                allModules_.erase(moduleId);
            }
            zmfInstances_.erase(moduleId);
        }
        else if (lineCommandSplit[0].compare("mreg") == 0) {
            zmf::data::ModuleUniqueId moduleId;
            if (!tryStringToModuleId(lineCommandSplit[1], moduleId)) {
                continue;
            }
            demoModules_[moduleId]->printModulePeerRegistry();
        }
        else if (lineCommandSplit[0].compare("setas") == 0) {
            zmf::data::ModuleUniqueId moduleId;
            if (!tryStringToModuleId(lineCommandSplit[1], moduleId)) {
                continue;
            }
            std::cout << ">>Enter additional state to set" << &std::endl;
            std::getline(std::cin, line);
            demoModules_[moduleId]->setAdditionalState(std::stoull(line));
        }
        else if (lineCommandSplit[0].compare("sub") == 0) {
            zmf::data::ModuleUniqueId moduleId;
            if (!tryStringToModuleId(lineCommandSplit[1], moduleId)) {
                continue;
            }
            zmf::data::MessageType topic;
            demoModules_[moduleId]->testSubscribe(topic);
        }
        else if (lineCommandSplit[0].compare("pub") == 0) {
            zmf::data::ModuleUniqueId moduleId;
            if (!tryStringToModuleId(lineCommandSplit[1], moduleId)) {
                continue;
            }
            zmf::data::ZmfMessage msg;
            demoModules_[moduleId]->testPublish(msg);
        }
        else if (lineCommandSplit[0].compare("ren") == 0 || lineCommandSplit[0].compare("renable") == 0) {
            zmf::data::ModuleUniqueId moduleId;
            if (!tryStringToModuleId(lineCommandSplit[1], moduleId)) {
                continue;
            }
            zmf::data::ModuleUniqueId moduleIdR;
            if (!tryStringToModuleId(lineCommandSplit[2], moduleIdR)) {
                continue;
            }
            // Remote enable
            std::cout << "Requesting remote enable " << moduleIdR.getString() << &std::endl;
            bool result = zmfInstances_[moduleId]->requestEnableRemoteInstance(moduleIdR, 1000);
            if (result) {
                std::cout << "Remote Enable Success" << &std::endl;
            } else {
                std::cout << "Remote Enable Failed" << &std::endl;
            }
        }
        else if (lineCommandSplit[0].compare("rdis") == 0 || lineCommandSplit[0].compare("rdisable") == 0) {
            zmf::data::ModuleUniqueId moduleId;
            if (!tryStringToModuleId(lineCommandSplit[1], moduleId)) {
                continue;
            }
            zmf::data::ModuleUniqueId moduleIdR;
            if (!tryStringToModuleId(lineCommandSplit[2], moduleIdR)) {
                continue;
            }
            // Remote disable
            std::cout << "Requesting remote disable " << moduleIdR.getString() << &std::endl;
            bool result = zmfInstances_[moduleId]->requestDisableRemoteInstance(moduleIdR, 1000);
            if (result) {
                std::cout << "Remote Disable Success" << &std::endl;
            } else {
                std::cout << "Remote Disable Failed" << &std::endl;
            }
        }
        else if (lineCommandSplit[0].compare("rstop") == 0) {
            zmf::data::ModuleUniqueId moduleId;
            if (!tryStringToModuleId(lineCommandSplit[1], moduleId)) {
                continue;
            }
            zmf::data::ModuleUniqueId moduleIdR;
            if (!tryStringToModuleId(lineCommandSplit[2], moduleIdR)) {
                continue;
            }
            // Remote stop
            std::cout << "Requesting remote stop " << moduleIdR.getString() << &std::endl;
            bool result = zmfInstances_[moduleId]->requestStopRemoteInstance(moduleIdR, 1000);
            if (result) {
                std::cout << "Remote Stop Success" << &std::endl;
            } else {
                std::cout << "Remote Stop Failed" << &std::endl;
            }
        }
        else {
            std::cout << "Unknown command" << &std::endl;
            printHelpText();
        }
    }
}

void DemoModuleConsole::printHelpText() {

    std::cout << &std::endl;
    std::cout << "Commands" << &std::endl;
    std::cout <<
    "\"start [instance] [optional version]\" creates a new mocked module and starts a ZMF instance with the module" <<
    //std::cout <<
    //"\"startr [instance] [optional version]\" creates a new real module and starts a ZMF instance with the module" <<
    //&std::endl;
    std::cout << "\"exit\" to exit demo module application" << &std::endl;
    std::cout << "\"en [type]:[instance]\" to enable an existing module" << &std::endl;
    std::cout << "\"dis [type]:[instance]\" to disable an existing module" << &std::endl;
    std::cout << "\"stop [type]:[instance]\" to stop an existing instance" << &std::endl;
    std::cout << "\"mreg [type]:[instance]\" to print module peer registry of a module" << &std::endl;
    std::cout << "\"setas [type]:[instance]\" to set additional state of a module" << &std::endl;
    std::cout << "\"sub [type]:[instance]\" to add test subscription" << &std::endl;
    std::cout << "\"pub [type]:[instance]\" to add test publish" << &std::endl;
    std::cout << "\"ren [t]:[i] [tR]:[iR]\" to remote enable an existing module [tR]:[iR] by module [t]:[i]" <<
    &std::endl;
    std::cout << "\"rdis [t]:[i] [tR]:[iR]\" to remote disable an existing module [tR]:[iR] by module [t]:[i]" <<
    &std::endl;
    std::cout << "\"rstop [t]:[i] [tR]:[iR]\" to remote stop an existing instance [tR]:[iR] by module [t]:[i]" <<
    &std::endl;
}


void DemoModuleConsole::createNewDemoModule(uint64_t moduleInstanceId, uint16_t moduleVersion, bool mockModule) {

    // Create module
    DemoModule* newDemoModule = new DemoModule(moduleInstanceId, moduleVersion);
    std::shared_ptr<zmf::AbstractModule> newModule = std::shared_ptr<DemoModule>(newDemoModule);
    // Create and start ZMF instance with module
    std::shared_ptr<zmf::IZmfInstanceController> newZmfInstance;
    if (mockModule) {
        //    newZmfInstance = zmf_mock::ZmfMock::startInstance(newModule, true,
        //                                                      true, false);
    }
    else {
        newZmfInstance = zmf::instance::ZmfInstance::startInstance(
                newModule, {});
    }

    demoModules_.insert(
            std::pair<zmf::data::ModuleUniqueId, DemoModule*>(newModule->getUniqueId(), newDemoModule));
    allModules_.insert(
            std::pair<zmf::data::ModuleUniqueId, std::shared_ptr<zmf::AbstractModule>>(newModule->getUniqueId(),
                                                                                       newModule));
    zmfInstances_.insert(
            std::pair<zmf::data::ModuleUniqueId, std::shared_ptr<zmf::IZmfInstanceController>>(newModule->getUniqueId(),
                                                                                               newZmfInstance));


    std::cout << ">>New module created: " << newModule->getNameInstanceString() << &std::endl;
}

void DemoModuleConsole::createModule(std::shared_ptr<zmf::AbstractModule> newModule, bool mockModule) {

    // Create module
    // Create and start ZMF instance with module
    std::shared_ptr<zmf::IZmfInstanceController> newZmfInstance;
    if (mockModule) {
        //newZmfInstance = zmf_mock::ZmfMock::startInstance(newModule, true, true, false);
    }
    else {
        newZmfInstance = zmf::instance::ZmfInstance::startInstance(newModule, {});
    }

    allModules_.insert(
            std::pair<zmf::data::ModuleUniqueId, std::shared_ptr<zmf::AbstractModule>>(newModule->getUniqueId(),
                                                                                       newModule));
    zmfInstances_.insert(
            std::pair<zmf::data::ModuleUniqueId, std::shared_ptr<zmf::IZmfInstanceController>>(newModule->getUniqueId(),
                                                                                               newZmfInstance));

    std::cout << ">>New module created: " << newModule->getNameInstanceString() << &std::endl;
}


bool DemoModuleConsole::readModuleInstanceId(uint64_t& readRef) {
    std::cout << ">>Enter module instance ID" << &std::endl;
    std::string line;
    std::getline(std::cin, line);
    readRef = (uint64_t) std::stoi(line);
    return true;
}

bool DemoModuleConsole::readModuleVersion(uint16_t& readRef) {
    std::cout << ">>Enter module version" << &std::endl;
    std::string line;
    std::getline(std::cin, line);
    readRef = (uint16_t) std::stoi(line);
    return true;
}

bool DemoModuleConsole::readModuleUniqueId(zmf::data::ModuleUniqueId& readRef) {
    std::cout << ">>Enter module ID with format [type]:[version]" << &std::endl;

    std::string line;
    std::getline(std::cin, line);
    Poco::StringTokenizer moduleIdPSplit(line, ":", 0);
    if (moduleIdPSplit.count() != 2) {
        std::cerr << ">>Invalid type identifier. Use format [type]:[version]" << &std::endl;
        return false;
    }
    readRef = zmf::data::ModuleUniqueId((uint16_t) std::stoi(moduleIdPSplit[0]),
                                        (uint64_t) std::stoi(moduleIdPSplit[1]));
    return true;
}

bool DemoModuleConsole::tryStringToModuleId(std::string str, zmf::data::ModuleUniqueId& outRef) {

    // TODO Catch exception Check exists
    Poco::StringTokenizer moduleIdPSplit(str, ":", 0);
    if (moduleIdPSplit.count() != 2) {
        std::cerr << ">>Invalid type identifier. Use format [type]:[version]" << &std::endl;
        return false;
    }
    outRef = zmf::data::ModuleUniqueId((uint16_t) std::stoi(moduleIdPSplit[0]),
                                       (uint64_t) std::stoi(moduleIdPSplit[1]));
    return true;
}
