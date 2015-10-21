/**
 * @details TODO Description
 * @author zsdn 
 * @date created on 10/21/15
*/

#ifndef ZMF_ZMFINREPLYINTERNAL_H
#define ZMF_ZMFINREPLYINTERNAL_H

#include <data/ZmfInReply.hpp>
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
        class ZmfInReplyInternal : ZmfInReply {

        private:
            zmf::messaging::IZmfMessagingCancelRequestHandler* zmqService_;

        public:

            ZmfInReplyInternal(uint64_t requestId_, std::future<ZmfMessage> future_,
                               zmf::messaging::IZmfMessagingCancelRequestHandler* zmqService_) : requestId_(
                    std::make_shared<uint64_t>(requestId_)),
                                                                                                 future_(std::move(
                                                                                                         future_)),
                                                                                                 zmqService_(
                                                                                                         zmqService_) { }

            ZmfInReplyInternal(const ZmfInReplyInternal&) = delete;

            ZmfInReplyInternal(ZmfInReplyInternal&& other) : requestId_(other.requestId_),
                                                             future_(std::move(other.future_)),
                                                             zmqService_(other.zmqService_) { }

            virtual ~ZmfInReplyInternal() {
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
            virtual void cancelRequest() {
                zmqService_->cancelRequest(*requestId_, true);
            }

        };
    }
}


#endif //ZMF_ZMFINREPLYINTERNAL_H
