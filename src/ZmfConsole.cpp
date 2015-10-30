/**
* @author Jonas Grunert
* @date created on 7/7/15.
*/

#include "ZmfConsole.hpp"
#include <Poco/StringTokenizer.h>
#include <iostream>

namespace zmf {

    ZmfConsole::ZmfConsole(std::shared_ptr<IZmfInstanceController> instanceController) :
            instanceController_(instanceController) {
    }

    void ZmfConsole::startConsole() {

        std::cout << &std::endl;
        std::cout << "---------- ZMF Console ----------" << &std::endl;
        printHelpText();

        std::string line;

        std::cin.ignore();

        while (!instanceController_->isStopped()) {
            std::getline(std::cin, line);

            // Split commands
            Poco::StringTokenizer lineCommandSplit(line, " ", 0);
            if (lineCommandSplit.count() == 0) {
                std::cerr << ">>Nothing entered" << &std::endl;
                continue;
            }

            if (line.compare("en") == 0 || line.compare("enable") == 0) {
                std::cout << "Requesting module enable" << &std::endl;
                instanceController_->requestEnableModule();
            }
            else if (line.compare("dis") == 0 || line.compare("disable") == 0) {
                std::cout << "Requesting module disable" << &std::endl;
                instanceController_->requestDisableModule();
            }
            else if (line.compare("stop") == 0) {
                std::cout << "Requesting instance stop" << &std::endl;
                instanceController_->requestStopInstance();
                instanceController_->joinExecution();
                return;
            }
            else if (lineCommandSplit[0].compare("ren") == 0 || lineCommandSplit[0].compare("renable") == 0) {
                zmf::data::ModuleUniqueId moduleIdR;
                if (!tryStringToModuleId(lineCommandSplit[1], moduleIdR)) {
                    continue;
                }
                // Remote enable
                std::cout << "Requesting remote enable " << moduleIdR.getString() << &std::endl;
                bool result = instanceController_->requestEnableRemoteInstance(moduleIdR, 1000);
                if (result) {
                    std::cout << "Remote Enable Success" << &std::endl;
                } else {
                    std::cout << "Remote Enable Failed" << &std::endl;
                }
            }
            else if (lineCommandSplit[0].compare("rdis") == 0 || lineCommandSplit[0].compare("rdisable") == 0) {
                zmf::data::ModuleUniqueId moduleIdR;
                if (!tryStringToModuleId(lineCommandSplit[1], moduleIdR)) {
                    continue;
                }
                // Remote disable
                std::cout << "Requesting remote disable " << moduleIdR.getString() << &std::endl;
                bool result = instanceController_->requestDisableRemoteInstance(moduleIdR, 1000);
                if (result) {
                    std::cout << "Remote Disable Success" << &std::endl;
                } else {
                    std::cout << "Remote Disable Failed" << &std::endl;
                }
            }
            else if (lineCommandSplit[0].compare("rstop") == 0) {
                zmf::data::ModuleUniqueId moduleIdR;
                if (!tryStringToModuleId(lineCommandSplit[1], moduleIdR)) {
                    continue;
                }
                // Remote stop
                std::cout << "Requesting remote stop " << moduleIdR.getString() << &std::endl;
                bool result = instanceController_->requestStopRemoteInstance(moduleIdR, 1000);
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

    void ZmfConsole::printHelpText() {

        std::cout << "Commands" << &std::endl;
        std::cout << "\"en\" or \"enable\" to enable this module" << &std::endl;
        std::cout << "\"dis\" or \"disable\" to disable this module" << &std::endl;
        std::cout << "\"stop\" to stop this instance" << &std::endl;
        std::cout << "\"ren [type]:[instance]\" to remote enable an existing module with ID [type]:[instance]" <<
        &std::endl;
        std::cout << "\"rdis [type]:[instance]\" to remote disable an existing module with ID [type]:[instance]" <<
        &std::endl;
        std::cout << "\"rstop [type]:[instance]\" to remote stop an existing instance with ID [type]:[instance]" <<
        &std::endl;
        std::cout << &std::endl;
    }

    bool ZmfConsole::tryStringToModuleId(std::string str, zmf::data::ModuleUniqueId& outRef) {

        try {
            Poco::StringTokenizer moduleIdPSplit(str, ":", 0);
            if (moduleIdPSplit.count() != 2) {
                std::cerr << ">>Invalid type identifier. Use format [type]:[version]" << &std::endl;
                return false;
            }
            outRef = zmf::data::ModuleUniqueId((uint16_t) std::stoi(moduleIdPSplit[0]),
                                               (uint64_t) std::stoi(moduleIdPSplit[1]));
            return true;
        }
        catch (...) {
            std::cerr << "Invalid module identifier" << &std::endl;
            return false;
        }
    }
}
