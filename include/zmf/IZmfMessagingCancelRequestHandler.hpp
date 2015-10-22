#ifndef ZMF_IZMFMESSAGINGCANCELREQUESTHANDLER_HPP
#define ZMF_IZMFMESSAGINGCANCELREQUESTHANDLER_HPP

#include <stdint.h>

namespace zmf {


    namespace messaging {
        /**
         * @details Interface class necessary to avoid circular dependencies
         * @author Jan Strauß
         * @date created on 7/14/15.
         */
        class IZmfMessagingCancelRequestHandler {

        public:
            /**
             * marks the request with the given request_id as "canceled", which means any internal resources associated with the request can be freed.
             *
             * @param request_id the id of the request to cancel
             */
            virtual void cancelRequest(uint64_t request_id, bool manual) = 0;
        };
    }
}

#endif //ZMF_IZMFMESSAGINGCANCELREQUESTHANDLER_HPP
