#ifndef ZMF_IZMFINSTANCECONTROLLER_H
#define ZMF_IZMFINSTANCECONTROLLER_H

#include "AbstractModule.hpp"


namespace zmf {

    /**
     * @details Interface to control a ZMF instance.
     * @author Jonas Grunert
     * @date created on 6/25/15.
     */
    class IZmfInstanceController {

    public:
        /**
         * Requests trying to enable the module controlled by this instance controller as soon as possible.
         * Module will be enabled if all preconditions are satisfied (dependencies satisfied, module id unique)
         */
        virtual void requestEnableModule() = 0;

        /**
         * Requests disabling the module controlled by this instance controller as soon as possible.
         */
        virtual void requestDisableModule() = 0;


        /**
         * Requests to stop the instance as soon as possible.
         * Will not will block until the instance is shut down.
         */
        virtual void requestStopInstance() = 0;


        /**
         * Requests the remote instance to be enabled. Will wait for the given timeout (ms) and return true only if a reply was
         * received for the request and if the received reply was positive
         */
        virtual bool requestEnableRemoteInstance(zmf::data::ModuleUniqueId, long timeout) = 0;

        /**
         * Requests the remote instance to be disabled. Will wait for the given timeout (ms) and return true only if a reply was
         * received for the request and if the received reply was positive
         */
        virtual bool requestDisableRemoteInstance(zmf::data::ModuleUniqueId, long timeout) = 0;

        /**
         * Requests the remote instance to stop. Will wait for the given timeout (ms) and return true only if a reply was
         * received for the request and if the received reply was positive
         */
        virtual bool requestStopRemoteInstance(zmf::data::ModuleUniqueId, long timeout) = 0;

        /**
         * Stopps the instance immediately
         * Will not will block until the instance is shut down.
         */
        virtual void stopInstance() = 0;


        /**
         * Joins the execution of the ZMF instance and the module, will block until the instance is shut down.
         */
        virtual void joinExecution() = 0;


        /**
         * Returns the controlled module
         */
        virtual std::shared_ptr<zmf::AbstractModule> getModule() = 0;


        /**
         * Returns if zmf instance is started and no shutdown in progress
         */
        virtual bool isStarted() = 0;

        /**
         * Returns if instance not started or shutdown finished and no starting in progress
         */
        virtual bool isStopped() = 0;
    };

}

#endif //ZMF_IZMFINSTANCECONTROLLER_H
