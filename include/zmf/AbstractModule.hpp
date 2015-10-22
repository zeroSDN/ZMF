#ifndef ZMF_ABSTRACTMODULE_H
#define ZMF_ABSTRACTMODULE_H

#include <memory>
#include <stdint.h>
#include <vector>
#include "IZmfInstanceAccess.hpp"
#include "ZmfOutReply.hpp"
#include "ZmfLogging.hpp"
#include "ZmfLoggingContainer.hpp"
#include <Poco/Logger.h>


namespace zmf {

    /**
     * Specifies a module dependency, consists of dependency module type and dependency module version.
     * Created on 6/25/15.
     * @author Jonas Grunert
     */
    struct ModuleDependency {

        /** Type of the needed dependency */
        uint16_t ModuleTypeId;
        /** Version of the needed dependency */
        uint16_t ModuleVersion;

        ModuleDependency(uint16_t moduleTypeId, uint16_t moduleVersion) :
                ModuleTypeId(moduleTypeId), ModuleVersion(moduleVersion) { }
    };


    /**
     * Abstract base class of a ZMF module.
     * Defines methods to override and offers access to the underlying ZMF instance.
     * Every module must have an ID (type+instance), a version, a name and optionally dependencies.
     * The base class offers a logger and ZMF access via ZmfInstanceAccess.
     * @author Jonas Grunert
     * @date created on 6/25/15.
     */
    class AbstractModule {

    public:
        /** Enabled state of a module */
        enum ModuleState {
            /** Module is disabled */
                    DISABLED,
            /** Module currently enabling */
                    ENABLING,
            /** Module is enabled */
                    ENABLED,
            /** Module is currently disabling */
                    DISABLING
        };


        /**
         * Base class constructor for modules. Sets given parameters and allows ZMF to internally access their values.
         * @param uniqueId Unique identifier of this module
         * @param version Version of this module
         * @param name Name of this module
         * @param dependencies Dependencies needed by this module
         */
        AbstractModule(zmf::data::ModuleUniqueId uniqueId, uint16_t version, std::string name,
                       std::vector<ModuleDependency> dependencies) :
                logger_(zmf::logging::ZmfLogging::getLoggerContainer(name)),
                uniqueId_(uniqueId), version_(version), name_(name), dependencies_(dependencies) { }

        ~AbstractModule();


        /**
         * ONLY CALL FROM CORE, NOT FROM MODULE
         */
        bool INTERNAL_internalEnable(IZmfInstanceAccess* const zmfRefInj);

        /**
         * ONLY CALL FROM CORE, NOT FROM MODULE
         */
        void INTERNAL_internalDisable();


        /**
         * Optional: Handle incoming ZMQ requests from other modules
         * @return Response message if we want to respond on the request, nullptr otherwise
         */
        virtual zmf::data::ZmfOutReply handleRequest(const zmf::data::ZmfMessage& message,
                                                     const zmf::data::ModuleUniqueId& sender) {
            // Invalid response - no handler in derived class
            return zmf::data::ZmfOutReply::createNoReply();
        }

        virtual void handleModuleStateChange(std::shared_ptr<zmf::data::ModuleHandle> changedModule,
                                             zmf::data::ModuleState newState, zmf::data::ModuleState lastState) {
        }


        /** @return Unique identifier of this module */
        inline zmf::data::ModuleUniqueId getUniqueId() {
            return uniqueId_;
        }

        /** @return Version of this module */
        inline uint16_t getVersion() {
            return version_;
        }

        /** @return Name of this module */
        inline std::string getModuleName() {
            return name_;
        }

        /** @return Dependencies needed by this module */
        inline std::vector<ModuleDependency> getDependencies() {
            return dependencies_;
        }


        /**
         * @return String with name and instance ID [name]:[instanceId] eg. ModulX:1
         */
        std::string getNameInstanceString();

        /**
         * @return String with name, type and instance ID [name]([typeId]):[instanceId] eg. ModulX(2):1
         */
        std::string getNameTypeInstanceString();

        /**
         * @return String with name, type ID, instance ID and version [name]([typeId]):[instanceId]_v[version] eg. ModulX(2):1_v1
         */
        std::string getNameTypeInstanceVersionString();


        /** @return If true module is currently Enabled */
        inline bool isEnabled() const { return moduleState_ == ENABLED; }


        /** @return Interface to access ZMF functions */
        inline IZmfInstanceAccess* const getZmf() {
            if (moduleState_ == DISABLED) {
                throw std::runtime_error("Trying to access Zmf while disabled!");
            }
            return zmfRef_;
        }

        /** @return Common Logger for event logging */
        inline Poco::Logger& getLogger() { return logger_->getLogger(); }

        /**
         * ONLY CALL FROM CORE, NOT FROM MODULE
         */
        std::mutex& INTERNAL_getInternalMutex() {
            return internalMutex;
        }


    protected:
        /**
         * Enable method to override by module class.
         * Enables module eg. connecting, starting threads
         * ONLY CALL FROM CORE, NOT FROM MODULE
         * @return True if module enable succeeded
         */
        virtual bool enable() = 0;

        /**
         * Disable method to override by module class.
         * Disables module eg. disconnecting, stopping threads
         * ONLY CALL FROM CORE, NOT FROM MODULE
         */
        virtual void disable() = 0;

    private:
        /** Interface to access ZMF */
        IZmfInstanceAccess* zmfRef_;
        /** Common Logger for event logging */
        std::unique_ptr<zmf::logging::ZmfLoggingContainer> logger_;

        /** Enabled state of this module */
        std::atomic<ModuleState> moduleState_ = ATOMIC_VAR_INIT(ModuleState::DISABLED);

        /** Unique identifier of this module */
        const zmf::data::ModuleUniqueId uniqueId_;
        /** Name of this module */
        const std::string name_;
        /** Version of this module */
        const uint16_t version_;
        /** Dependencies needed by this module */
        const std::vector<ModuleDependency> dependencies_;

        /**
         * Internal Mutex to lock object by JMF
         * ONLY USE FROM CORE, NOT FROM MODULE
         */
        std::mutex internalMutex;
    };

}


#endif //ZMF_ABSTRACTMODULE_H
