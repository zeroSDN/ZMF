/**
 * @author Jan Strauß
 * @date created on 7/7/15.
*/

#include "ConfigurationProvider.hpp"

namespace zmf {
    namespace config {
        bool ConfigurationProvider::getAsString(const std::string& key, std::string& value) const {
            if (!hasConfig) {
                return false;
            }

            const bool present = internalConfig->hasProperty(key);

            if (present) {
                value = internalConfig->getString(key);
            }

            return present;
        }

        bool ConfigurationProvider::getAsBoolean(const std::string& key, bool& value) const {
            if (!hasConfig) {
                return false;
            }

            const bool present = internalConfig->hasProperty(key);

            if (present) {
                try {
                    value = internalConfig->getBool(key);
                } catch (const Poco::SyntaxException& e) {
                    logger.warning("failed to get " + key + " as bool");
                    return false;
                }
            }

            return present;
        }

        bool ConfigurationProvider::getAsInt(const std::string& key, int& value) const {
            if (!hasConfig) {
                return false;
            }
            const bool present = internalConfig->hasProperty(key);

            if (present) {
                try {
                    value = internalConfig->getInt(key);
                } catch (const Poco::SyntaxException& e) {
                    logger.warning("failed to get " + key + " as int");
                    return false;
                }
            }

            return present;
        }

        bool ConfigurationProvider::getAsDouble(const std::string& key, double& value) const {
            if (!hasConfig) {
                return false;
            }

            const bool present = internalConfig->hasProperty(key);

            if (present) {
                value = internalConfig->getDouble(key);
                try {
                    value = internalConfig->getDouble(key);
                } catch (const Poco::SyntaxException& e) {
                    logger.warning("failed to get " + key + " as double");
                    return false;
                }
            }

            return present;
        }
    }
}