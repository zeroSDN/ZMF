#ifndef ZMF_IZMFINSTANCEACCESS_H
#define ZMF_IZMFINSTANCEACCESS_H

#include <atomic>
#include <future>
#include <list>
#include <memory>
#include <map>
#include "IConfigurationProvider.hpp"
#include "PeerRegistry.hpp"
#include "ModuleHandle.hpp"
#include "MessageType.hpp"
#include "ZmfMessage.hpp"
#include "ModuleUniqueId.hpp"
#include "ZmfInReply.hpp"


namespace zmf {

    /**
     * @details Interface to access a ZMF instance from a module
     * @author Jonas Grunert
     * @date created on 6/25/15.
     */
    class IZmfInstanceAccess {

    public:

        /**
         * Subscription handle representing a specific subscription.
         * Can be used to unsubscribe from a subscription
         */
        class SubscriptionHandle {
        public:
            /**
             * Interface offering possibility to unsubscribe from a subscription
             */
            class ISubscriptionHandler {
            public:
                /**
                 * Unsubscribes from the given handle
                 * @param handle Handle to unsubscribe
                 */
                virtual void unsubscribe(SubscriptionHandle* handle) = 0;

                /// Returns a new, instance unique subscription ID
                virtual uint32_t getSubId() = 0;
            };


            SubscriptionHandle(ISubscriptionHandler* unsubscribeHandler, zmf::data::MessageType topic,
                               std::function<void(const zmf::data::ZmfMessage&,
                                                  const zmf::data::ModuleUniqueId& sender)> callback) :
                    SubId(unsubscribeHandler->getSubId()),
                    unsubscribeHandler_(unsubscribeHandler), subTopic(topic),
                    callback_(callback) { }

            /**
             * Unsubscribe from this subscription
             * @throws Exception
             */
            void unsubscribe() {
                unsubscribeHandler_->unsubscribe(this);
            }

            /**
             * @return Topic of this subscription
             */
            inline const zmf::data::MessageType& getTopic() { return subTopic; }

            /**
             * @return Subscription handler callback
             */
            inline std::function<void(const zmf::data::ZmfMessage& msg,
                                      const zmf::data::ModuleUniqueId& sender)>& getCallback() { return callback_; }

            const uint32_t SubId;
        private:
            ISubscriptionHandler* unsubscribeHandler_;
            const zmf::data::MessageType subTopic;
            std::function<void(const zmf::data::ZmfMessage& msg, const zmf::data::ModuleUniqueId& sender)> callback_;
        };


        /**
         * Returns the configuration provider interface which allows to access configuration values
         * @return Interface to configuration provider
         */
        virtual const std::shared_ptr<zmf::config::IConfigurationProvider>& getConfigurationProvider() = 0;

        /**
         * @return The public interface of the PeerRegistry which offers information about all known peers
         */
        virtual const std::shared_ptr<zmf::discovery::PeerRegistry>& getPeerRegistry() = 0;


        /**
         * Sends a request to a given node and returns a future_ to await the response.
         */
        virtual zmf::data::ZmfInReply sendRequest(const zmf::data::ModuleUniqueId& target,
                                                  const zmf::data::ZmfMessage& msg) = 0;

        /**
         * Starts a subscription to the given topic. Received subscription events will be sent to the given handler callback.
         */
        virtual IZmfInstanceAccess::SubscriptionHandle subscribe(const zmf::data::MessageType& topic,
                                                                 std::function<void(const zmf::data::ZmfMessage& msg,
                                                                                    const zmf::data::ModuleUniqueId& sender)> handler) = 0;

        /**
         * Publishes an event to the messaging message bus.
         */
        virtual void publish(const zmf::data::ZmfMessage& msg) = 0;


        /**
         * Called when the additional state of a module was changed.
         * Will not trigger automatically trigger a state broadcast.
         */
        virtual void onModuleAdditionalStateChanged(std::vector<uint8_t> additionalState) = 0;

        /**
         * Triggers a state broadcast for the module of this instance.
         */
        virtual void forceStateBroadcast() = 0;


        /**
         * Requests to initiate disabling of the module managed by this ZMF core
         */
        virtual void requestDisableModule() = 0;

        /**
         * Requests to initiate stopping this ZMF core and its module
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
    };

}

#endif //ZMF_IZMFINSTANCEACCESS_H
