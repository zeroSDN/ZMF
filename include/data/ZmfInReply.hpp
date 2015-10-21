#ifndef ZMF_ZMFINREPLY_HPP
#define ZMF_ZMFINREPLY_HPP

#include <string>
#include <stdint.h>
#include <future>
#include "ZmfMessage.hpp"
#include "../messaging/IZmfMessagingCancelRequestHandler.hpp"

namespace zmf {
    namespace data {
        /**
         * Represents an incoming reply.
         * Offers Java Future functions to wait on incoming reply.
         * @author Jonas Grunert
         * @author Jan Strauß
         * @date created on 7/7/15.
        */
        struct ZmfInReply {

        private:
            std::shared_ptr<uint64_t> requestId_;
            std::future<zmf::data::ZmfMessage> future_;
            zmf::messaging::IZmfMessagingCancelRequestHandler* zmqService_;

        public:

            ZmfInReply(uint64_t requestId_, std::future<ZmfMessage> future_,
                       zmf::messaging::IZmfMessagingCancelRequestHandler* zmqService_) : requestId_(
                    std::make_shared<uint64_t>(requestId_)),
                                                                                         future_(std::move(future_)),
                                                                                         zmqService_(zmqService_) { }

            ZmfInReply(const ZmfInReply&) = delete;

            ZmfInReply(ZmfInReply&& other) : requestId_(other.requestId_), future_(std::move(other.future_)),
                                             zmqService_(other.zmqService_) { }

            virtual ~ZmfInReply() {
                if (requestId_.unique()) {
                    zmqService_->cancelRequest(*requestId_, false);
                }
            }

            /**
             * Waits until receiving reply (future.get)
             * @return Received Reply
             */
            zmf::data::ZmfMessage get() {
                return future_.get();
            }

            /**
             * Waits until receiving reply with timeut
             * @param duration Specifies timeout
             * @return Received Reply
             */
            template<class Rep, class Period>
            std::future_status wait_for(const std::chrono::duration<Rep, Period>& duration) {
                return future_.wait_for(duration);
            }

            /**
             * Does future.wait on reply future
             */
            void wait() {
                future_.wait();
            }

            /**
             * Cancels ZMQ request and request future
             */
            void cancelRequest() {
                zmqService_->cancelRequest(*requestId_, true);
            }

        };
    }
}
#endif //ZMF_ZMFINREPLY_HPP
