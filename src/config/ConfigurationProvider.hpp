#ifndef ZMF_CONFIGURATIONPROVIDER_H
#define ZMF_CONFIGURATIONPROVIDER_H

#include "IConfigurationProvider.hpp"
#include "ZmfLogging.hpp"
#include <string>
#include <Poco/Util/PropertyFileConfiguration.h>
#include <Poco/AutoPtr.h>
#include <Poco/Logger.h>
#include <Poco/Path.h>
#include <iostream>

namespace zmf {
    namespace config {
        /**
         * @details Implementation of the IConfigurationProvider interface
         * @author Jan Strauß
         * @date created on 7/7/15.
        */
        class ConfigurationProvider : public IConfigurationProvider {

        public:
            /**
             * default constructor.
             * @param path to load the config from, will default to empty config
             */
            ConfigurationProvider(const std::string& path = "") :
                    internalConfig(new Poco::Util::PropertyFileConfiguration()),
                    logger(Poco::Logger::get("ConfigurationProvider")) {
                try {
                    if (path.empty()) {
                        logger.debug("no config path provided, config loading skipped");
                        return;
                    }
                    this->internalConfig->load(Poco::Path::expand(path));
                    hasConfig = true;
                    logger.debug("Loaded configuration file from " + path);
                } catch (const Poco::Exception& e) {
                    logger.error("load config from " + path + " failed:");
                    logger.log(e);
                    hasConfig = false;
                }
            }

            virtual ~ConfigurationProvider() {
            }

            virtual bool getAsString(const std::string& key, std::string& value) const;

            virtual bool getAsBoolean(const std::string& key, bool& value) const;

            virtual bool getAsInt(const std::string& key, int& value) const;

            virtual bool getAsDouble(const std::string& key, double& value) const;

        private:
            Poco::AutoPtr<Poco::Util::PropertyFileConfiguration> internalConfig;
            Poco::Logger& logger;
            bool hasConfig = false;
        };
    }
}

#endif //ZMF_CONFIGURATIONPROVIDER_H
