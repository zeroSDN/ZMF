#ifndef ZMF_ZMFLOGGINGCONTAINER_H
#define ZMF_ZMFLOGGINGCONTAINER_H

#include <Poco/Logger.h>

namespace zmf {
    namespace logging {
        /**
         * @details Container class to offer access to get a Poco logger
         * @author Jan Strauß
         * @date created on 7/7/15.
        */
        class ZmfLoggingContainer {
        public:

            ZmfLoggingContainer(Poco::Logger& logger) : logger(logger) { }

            Poco::Logger& getLogger() const {
                return logger;
            }

        private:
            Poco::Logger& logger;
        };
    }
}

#endif //ZMF_ZMFLOGGINGCONTAINER_H


