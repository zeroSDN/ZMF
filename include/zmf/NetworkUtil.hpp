#ifndef ZMF_NETWORKUTIL_H
#define ZMF_NETWORKUTIL_H

#include <Poco/Net/NetworkInterface.h>

namespace zmf {
    namespace util {
        /**
         * @details Network utilities for finding out information about the network environment (local IP)
         * @author Jan Strauß
         * @date created on 7/7/15.
         */
        class NetworkUtil {
        public:
            static Poco::Net::NetworkInterface getNetworkInterface(Poco::Net::IPAddress::Family ip_version);

            static Poco::Net::NetworkInterface getNetworkInterface(std::string name,
                                                                   Poco::Net::IPAddress::Family ip_version);
        };
    }
}


#endif //ZMF_NETWORKUTIL_H
