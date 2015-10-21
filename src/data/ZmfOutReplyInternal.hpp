/**
 * @details TODO Description
 * @author zsdn 
 * @date created on 10/21/15
*/

#ifndef ZMF_ZMFOUTREPLYINTERNAL_H
#define ZMF_ZMFOUTREPLYINTERNAL_H

#include "../messaging/IZmfMessagingSendReplyHandler.hpp"
#include <data/ZmfOutReply.hpp>


namespace zmf {
    namespace data {
        /**
         * @details Represents an outgoing reply message.
         * Can be NO_REPLY (no reply will be sent),
         * IMMEDIATE_REPLY (reply now) or
         * FUTURE_REPLY (allows replying in the future)
         * @author Jonas Grunert
         * @date created on 7/20/15.
        */
        class ZmfOutReplyInternal : ZmfOutReply {

        public:


            /**
             * Creates and returns a NoReply reply
             */
            static ZmfOutReply createNoReply() {
                return ZmfOutReplyInternal(NO_REPLY, FINISHED);
            }

            /**
             * Creates and returns a ImmediateReply reply
             */
            static ZmfOutReply createImmediateReply(ZmfMessage immediate) {
                return ZmfOutReplyInternal(immediate);
            }

            /**
             * Creates and returns a FutureReply reply
             */
            static ZmfOutReply createFutureReply() {
                return ZmfOutReplyInternal(FUTURE_REPLY, OPEN);
            }


            /**
             * Private constructor, use create methods to construct instance
             */
            ZmfOutReply(ZmfMessage
            & reply_immediate) :

            type(IMMEDIATE_REPLY),
            reply_immediate(reply_immediate), state(FINISHED), mutex(),
            cond_var() { }

            /**
             * Private constructor, use create methods to construct instance
             */
            ZmfOutReply(ReplyType
            type,
            State state
            ) :

            type(type), state(state), mutex(), cond_var() { }


            void injectService(zmf::messaging::IZmfMessagingSendReplyHandler* service,
                               zmf::messaging::ExternalRequestIdentity identity) {
                std::unique_lock <std::mutex> lock(mutex);
                this->service = service;
                this->request_id = identity;
                cond_var.notify_one();
            }


            /**
             * Used to send a future reply
             * @param message Reply message
             */
            virtual void sendFutureReply(ZmfMessage message) {
                if (state == FINISHED) {
                    return;
                }

                std::unique_lock <std::mutex> lock(mutex);
                while (service == nullptr) {
                    cond_var.wait(lock);
                }

                service->sendReply(request_id, message);
                state = FINISHED;
            }

        private:

            zmf::messaging::ExternalRequestIdentity request_id;
            zmf::messaging::IZmfMessagingSendReplyHandler* service = nullptr;

        };
    }
}

#endif //ZMF_ZMFOUTREPLYINTERNAL_H
