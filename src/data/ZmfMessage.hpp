#ifndef ZMF_ZMFMESSAGE_HPP
#define ZMF_ZMFMESSAGE_HPP

#include <string>
#include <stdint.h>
#include "MessageType.hpp"

namespace zmf {
    namespace data {
        /**
         * A message to send or received, a pair of message type and data
         * @author Jonas Grunert
         * @date created on 7/20/15.
        */
        struct ZmfMessage {
        private:
            const MessageType type_;
            const std::string data_;

        public:
            ZmfMessage() : type_(), data_() { }

            ZmfMessage(std::string type, std::string data) : type_(type), data_(data) { }

            ZmfMessage(MessageType type, std::string data) : type_(type), data_(data) { }

            ZmfMessage(MessageType type, uint8_t* data, size_t dataLength) : type_(type),
                                                                             data_((char*) data, dataLength) { }


            const inline MessageType& getType() const {
                return type_;
            }

            const inline std::string& getData() const {
                return data_;
            }

            const inline std::size_t getDataLength() const {
                return data_.length();
            }

            const inline uint8_t* getDataRaw() const {
                return (uint8_t*) data_.data();
            }


            bool operator==(const ZmfMessage& b) const {
                return type_ == b.type_ && data_ == b.data_;
            }
        };
    }
}

#endif //ZMF_ZMFMESSAGE_HPP
