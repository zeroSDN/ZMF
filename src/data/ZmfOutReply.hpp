#ifndef ZMF_ZMFOUTREPLY_HPP
#define ZMF_ZMFOUTREPLY_HPP

#include <stdint.h>
#include "ZmfMessage.hpp"
#include "../messaging/IZmfMessagingSendReplyHandler.hpp"

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
        struct ZmfOutReply {

            /**
             * Type of the reply
             */
            const enum ReplyType {
                /// Send reply immediately
                        IMMEDIATE_REPLY,
                /// Send reply in the future
                        FUTURE_REPLY,
                /// Send no reply
                        NO_REPLY
            } type;


            /**
             * Used to send a future reply
             * @param message Reply message
             */
            void sendFutureReply(ZmfMessage message) {
                if (state == FINISHED) {
                    return;
                }

                std::unique_lock<std::mutex> lock(mutex);
                while (service == nullptr) {
                    cond_var.wait(lock);
                }

                service->sendReply(request_id, message);
                state = FINISHED;
            }

            /**
             * Creates and returns a NoReply reply
             */
            static ZmfOutReply createNoReply() {
                return ZmfOutReply(NO_REPLY, FINISHED);
            }

            /**
             * Creates and returns a ImmediateReply reply
             */
            static ZmfOutReply createImmediateReply(ZmfMessage immediate) {
                return ZmfOutReply(immediate);
            }

            /**
             * Creates and returns a FutureReply reply
             */
            static ZmfOutReply createFutureReply() {
                return ZmfOutReply(FUTURE_REPLY, OPEN);
            }


            void injectService(zmf::messaging::IZmfMessagingSendReplyHandler* service,
                               zmf::messaging::ExternalRequestIdentity identity) {
                std::unique_lock<std::mutex> lock(mutex);
                this->service = service;
                this->request_id = identity;
                cond_var.notify_one();
            }

            ZmfOutReply(zmf::data::ZmfOutReply&& other) : type(other.type), state(other.state), mutex(), cond_var() { }

            const ZmfMessage reply_immediate;
        private:

            enum State {
                OPEN, FINISHED
            } state;

            zmf::messaging::ExternalRequestIdentity request_id;
            zmf::messaging::IZmfMessagingSendReplyHandler* service = nullptr;

            std::mutex mutex;
            std::condition_variable cond_var;

            /**
             * Private constructor, use create methods to construct instance
             */
            ZmfOutReply(ZmfMessage& reply_immediate) : type(IMMEDIATE_REPLY),
                                                       reply_immediate(reply_immediate), state(FINISHED), mutex(),
                                                       cond_var() { }

            /**
             * Private constructor, use create methods to construct instance
             */
            ZmfOutReply(ReplyType type, State state) : type(type), state(state), mutex(), cond_var() { }


        };
    }
}
#endif //ZMF_ZMFOUTREPLY_HPP
