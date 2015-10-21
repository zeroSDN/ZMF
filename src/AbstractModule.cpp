/**
* @author Jonas Grunert
* @date created on 6/25/15.
*/

#include "AbstractModule.hpp"

namespace zmf {


    AbstractModule::~AbstractModule() {
        if (moduleState_ == ENABLED) {
            INTERNAL_internalDisable();
        }
    }

    bool AbstractModule::INTERNAL_internalEnable(IZmfInstanceAccess* const zmfInj) {

        ModuleState disabledMode = DISABLED;
        if (!moduleState_.compare_exchange_strong(disabledMode, ENABLING)) {
            poco_warning(getLogger(), "Tried to enable module twice - ignoring request");
            return true;
        }

        this->logger_.reset(
                zmf::logging::ZmfLogging::getLoggerContainer(getUniqueId().getString() + " # " + getModuleName()));

        if (zmfInj == nullptr) {
            poco_fatal(getLogger(), "ZmfInstance == null - never call INTERNAL_internalEnable directly");
        }
        this->zmfRef_ = zmfInj;

        poco_information(getLogger(), "Enabling module " + getNameInstanceString());

        if (enable()) {
            poco_information(getLogger(), "Enabled module " + getNameInstanceString());
            moduleState_ = ENABLED;
            return true;
        }
        else {
            poco_error(getLogger(), "Failed to enable " + getNameInstanceString());
            moduleState_ = DISABLED;
            return false;
        }
    }

    void AbstractModule::INTERNAL_internalDisable() {

        ModuleState enabledMode = ENABLED;
        if (!moduleState_.compare_exchange_strong(enabledMode, DISABLING)) {
            poco_warning(getLogger(), "Tried to disable module that is not enabled - ignoring request");
            return;
        }

        poco_information(getLogger(), "Disabling module " + getNameInstanceString());
        disable();
        poco_information(getLogger(), "Disabled module " + getNameInstanceString());

        moduleState_ = DISABLED;
    }


    std::string AbstractModule::getNameInstanceString() {
        return name_ + ":" + std::to_string(uniqueId_.InstanceId);
    }

    std::string AbstractModule::getNameTypeInstanceString() {
        return getModuleName() + "(" + std::to_string(uniqueId_.TypeId) + "):" +
               std::to_string(uniqueId_.InstanceId);
    }

    std::string AbstractModule::getNameTypeInstanceVersionString() {
        return getNameInstanceString() + "_v" + std::to_string(version_);
    }
}