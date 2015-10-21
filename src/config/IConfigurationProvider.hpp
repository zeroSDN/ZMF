

#ifndef ZMF_ICONFIGURATIONPROVIDER_HPP
#define ZMF_ICONFIGURATIONPROVIDER_HPP

#include <string>

namespace zmf {
    namespace config {
        /**
         * @details This interface specifies the configuration access (read only) methods exposed to the module.
         * @author Jan Strauß
         * @date created on 7/7/15.
        */
        class IConfigurationProvider {

        public:
            /**
             * Gets the value of the config entry identified by key as string.
             *
             * @param key the key of the value to retrieve
             * @param value reference to the value to set
             * @return true if the value was found, false otherwise
             * @throws SyntaxException if the value couldn't be converted to string
             */
            virtual bool getAsString(const std::string& key, std::string& value) const = 0;

            /**
             * Gets the value of the config entry identified by key as bool.
             *
             * @param key the key of the value to retrieve
             * @param value reference to the value to set
             * @return true if the value was found, false otherwise
             * @throws SyntaxException if the value couldn't be converted to bool
             */
            virtual bool getAsBoolean(const std::string& key, bool& value) const = 0;

            /**
             * Gets the value of the config entry identified by key as integer.
             *
             * @param key the key of the value to retrieve
             * @param value reference to the value to set
             * @return true if the value was found, false otherwise
             * @throws SyntaxException if the value couldn't be converted to int
             */
            virtual bool getAsInt(const std::string& key, int& value) const = 0;

            /**
             * Gets the value of the config entry identified by key as double.
             *
             * @param key the key of the value to retrieve
             * @param value reference to the value to set
             * @return true if the value was found, false otherwise
             * @throws SyntaxException if the value couldn't be converted to double
             */
            virtual bool getAsDouble(const std::string& key, double& value) const = 0;
        };
    }
}

#endif //ZMF_ICONFIGURATIONPROVIDER_HPP
