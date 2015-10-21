#ifndef ZMF_ZMFLOGGING_HPP
#define ZMF_ZMFLOGGING_HPP

#include <iostream>
#include <Poco/FileChannel.h>
#include <Poco/AutoPtr.h>
#include <Poco/ConsoleChannel.h>
#include <Poco/SplitterChannel.h>
#include <Poco/PatternFormatter.h>
#include <Poco/FormattingChannel.h>
#include <Poco/Logger.h>
#include <Poco/Util/LoggingConfigurator.h>
#include <Poco/Util/PropertyFileConfiguration.h>
#include <Poco/Path.h>
#include <atomic>
#include <memory>
#include "ZmfLoggingContainer.hpp"

namespace zmf {
    namespace logging {


        /**
         * @details This class provides access to the logging functionality of ZMF.
         * Every component that wants to log stuff should obtains its logger via the getLogger method instead of
         * directly obtaining it from Poco::Logger::get(), as getLogger ensures that logging is initialized and
         * therefore the channels and formatter are set up accordingly.
         * @author Jan Strauß
         * @date created on 7/7/15.
        */
        class ZmfLogging {
        public:
            /**
             * In most cases calling this method is not necessary as ZMF will invoke this method in startInstance()
             * and also getLogger() will ensure everything is initialized.
             *
             * The Logging will first be configured to a hardcoded default. Then the defaults will be overwritten
             * by the values loaded from the given config file if present.
             */
            static void initializeLogging(const std::string& moduleName, const std::string& path = "");

            /**
             * retrieves the logger identified by the given name.
             *
             * @param name the name of the logger to get
             * @return a reference to the logger requested
             */
            static Poco::Logger& getLogger(std::string name);

            static zmf::logging::ZmfLoggingContainer* getLoggerContainer(std::string name);

        private:
            /**
             * flag storing if the logging init has been performed
             */
            static std::atomic_bool initialized;
        };
    }
}


#endif //ZMF_ZMFLOGGING_HPP
