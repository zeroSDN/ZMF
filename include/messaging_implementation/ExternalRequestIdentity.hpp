#ifndef ZMF_EXTERNALREQUESTIDENTITY_HPP
#define ZMF_EXTERNALREQUESTIDENTITY_HPP

#include "../data/ModuleUniqueId.hpp"

namespace zmf {
    namespace messaging {

        /**
         * @details Data structure representing ID of an externally requesting peer
         * @author Jan Strauß
         * @date created on 8/2/15.
         */
        class ExternalRequestIdentity {
        public:
            zmf::data::ModuleUniqueId senderId;
            uint64_t requestId;


            ExternalRequestIdentity() { }

            ExternalRequestIdentity(const data::ModuleUniqueId senderId,
                                    const uint64_t requestId) : senderId(senderId),
                                                                requestId(requestId) { }

            bool operator<(const ExternalRequestIdentity& r) const {
                if (senderId < r.senderId) { return true; }
                if (senderId > r.senderId) { return false; }

                if (requestId < r.requestId) { return true; }
                if (requestId > r.requestId) { return false; }

                return false;
            }
        };
    }
}

#endif //ZMF_EXTERNALREQUESTIDENTITY_HPP
