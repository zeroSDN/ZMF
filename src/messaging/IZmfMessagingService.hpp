#ifndef ZMF_IZMQSERVICE_H
#define ZMF_IZMQSERVICE_H


#include "AbstractModule.hpp"
#include "ModuleHandle.hpp"
#include "IZmfMessagingCoreInterface.hpp"
#include "ZmfOutReply.hpp"
#include "IZmfMessagingSendReplyHandler.hpp"
#include "IZmfMessagingCancelRequestHandler.hpp"

namespace zmf {
    namespace messaging {

        /**
         * @details This class provides the interface definition for the ZMQ-module of ZMF. The IZmfMessagingCoreInterface class provides the logical counterpart.
         * The Implementation
         * @author Jonas Grunert
         * @date created on 6/25/15.
         */
        class IZmfMessagingService
                : public IZmfMessagingSendReplyHandler, public zmf::messaging::IZmfMessagingCancelRequestHandler {
        public:
            /**
             * Starts the ZMQ Service. After the service successfully started, the other methods may be called and the implementation will start to deliver incoming events to the provided IZmfMessagingCoreInterface.
             *
             * @param core pointer to the IZmfMessagingCoreInterface implementation that provides methods implementations of this class will interact with.
             * @param selfHandle shared_ptr to the ModuleHandle describing the module instance running on top of this ZMF instance
             * @return Returns true if started successfully, false otherwise
             */
            virtual bool start(IZmfMessagingCoreInterface* const corePtr,
                               std::shared_ptr<zmf::data::ModuleHandle> selfHandle,
                               std::shared_ptr<zmf::config::IConfigurationProvider> config) = 0;

            /**
             * Stops the ZMQ Service. After the service is stopped, calling any method except the start method will not have any effect. Also the implementation will remove any state (peer information, outstanding messages) currently stored.
             */
            virtual void stop() = 0;

            /**
             * To be called when ZMF learns about a new peer.
             * @param module the ModuleHandle of the joining peer
             */
            virtual void peerJoin(std::shared_ptr<zmf::data::ModuleHandle> module) = 0;

            /**
             * To be called when a previously known peer leaves.
             * @param module the ModuleHandle of the leaving peer
             */
            virtual void peerLeave(std::shared_ptr<zmf::data::ModuleHandle> module) = 0;

            /**
             * Adds a subscription for the given topic.
             * Semantics are equivalent to the ZMQ subscription ones.
             *
             * @param topic the MessageType to subscribe on
             */
            virtual void subscribe(const zmf::data::MessageType& topic) = 0;

            /**
             * Removes a subscription for the given topic.
             * Semantics are equivalent to the ZMQ subscription ones.
             *
             * @param topic the MessageType to unsubscribe from
             */
            virtual void unsubscribe(const zmf::data::MessageType& topic) = 0;

            /**
             * Publishes the given ZmfMessage as event under the topic of the message's MessageType
             *
             * @param msg the message to publish
             */
            virtual void publish(const zmf::data::ZmfMessage& msg) = 0;

            /**
             * Sends the given message to the given peer. A ZmfInReply is returned providing a handle for the outstanding response.
             *
             * @param target the identity of the receiver
             * @param msg the Message to send
             * @return ZmfInReply a handle for the outstanding response. May be passed to the module.
             */
            virtual zmf::data::ZmfInReply sendRequest(const zmf::data::ModuleUniqueId& target,
                                                      const zmf::data::ZmfMessage& msg) = 0;

            /**
             * Removes the internal resources mapped to the given requestID. To be called internally from a ZmfInReply if it is cancelled.
             *
             * @param requestID the id of the request to cancel
             */
            virtual void cancelRequest(uint64_t requestID, bool manual) = 0;

            /**
             * Sends the given reply to the sender of the request with the given id. Should be wrapped from the module.
             *
             * @param requestID the id of the request to reply to
             * @param response the response to send
             */
            virtual void sendReply(ExternalRequestIdentity requestID, const zmf::data::ZmfMessage& reply) = 0;

            /**
             * to be called when the module is disabled.
             */
            virtual void onDisable() = 0;
        };
    }
}


#endif //ZMF_IZMQSERVICE_H
