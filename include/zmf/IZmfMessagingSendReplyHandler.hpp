#ifndef ZMF_IZMFMESSAGINGSENDREPLYHANDLER_HPP
#define ZMF_IZMFMESSAGINGSENDREPLYHANDLER_HPP

#include <stdint.h>
#include "ZmfMessage.hpp"
#include "ExternalRequestIdentity.hpp"

namespace zmf {
    namespace messaging {
        /**
         * @details Interface class necessary to avoid circular dependencies
         * @author Jan Strauß
         * @date created on 7/20/15.
         */
        class IZmfMessagingSendReplyHandler {

        public:

            /**
             * Sends the given reply to the sender of the request with the given id. Should be wrapped from the module.
             *
             * @param requestID the id of the request to reply to
             * @param response the response to send
             */
            virtual void sendReply(ExternalRequestIdentity requestID, const zmf::data::ZmfMessage& reply) = 0;
        };
    }
}


#endif //ZMF_IZMFMESSAGINGSENDREPLYHANDLER_HPP
