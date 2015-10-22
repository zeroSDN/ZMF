//
// Created by Jonas Grunert on 6/26/15.
// Edited by Andre Kutzleb on 6/27/15.
//

#ifndef ZMF_MESSAGETYPE_HPP
#define ZMF_MESSAGETYPE_HPP

#include <stdint.h>
#include <Poco/Exception.h>

namespace zmf {

    namespace data {
        static const uint8_t MESSAGE_ID_BYTES = 32;

        /**
         * @details Data structure discribing the type of a message.
         * Consists of a string of match up to 32 bytes describing the message.
         * Is used for message filtering in ZMQ.
         * Can be builded by appending bytes to the match.
         * @author Jonas Grunert
         * @author Andre Kutzleb
         * @date created on 6/26/15.
         * @date modified on 6/27/15.
        */
        struct MessageType {
        private:
            std::string match_;

            void checkSize() {
                if (match_.size() >= MESSAGE_ID_BYTES) {
                    throw Poco::Exception("type length exceeds max type length of 32 bytes");
                }
            }

        public:
            /**
             * Constructs a MessageType from a string match.
             *
             * @param bytes
             * 		the match to construct this MessageType from
             */
            MessageType(const std::string bytes) : match_(bytes) {
                checkSize();
            }

            /**
             * Constructs an empty MessageType
             */
            MessageType() { }


            /**
             * Returns the raw inner byte array of this match as string.
             *
             * @return the raw inner byte array
             */
            const inline std::string& getMatch() const {
                return match_;
            }

            /**
             * Returns the length of raw inner byte array of this match.
             *
             * @return the raw inner byte array
             */
            const inline std::size_t getMatchLength() const {
                return match_.length();
            }

            /**
             * Returns the raw inner byte array of this match as raw bytes.
             *
             * @return the raw inner byte array
             */
            const inline uint8_t* getMatchRaw() const {
                return (uint8_t*) match_.data();
            }

            /**
             * Returns a String representation of this MessageType in the form of  "x1.x2.x3.xn|n" where x1 .. xn are the bytes of this match and n is the number of bytes
             *
             * @return a string containing the match in decimals split by a dot followed by the bytearray length
             */
            std::string toString() const {
                std::string result;
                for (char i : match_) {
                    result += std::to_string((unsigned char) i) + ".";
                }
                result.pop_back();
                result += '|';
                result += std::to_string(match_.size());
                return result;
            }

            const inline void trimToLength(uint8_t length) {
                match_.resize(length);
            }

            /**
             * Appends one byte to the match.
             * Checks size, increases match length and appends byte.
             */
            const inline void appendMatch8(uint8_t match) {
                match_ += match;
                checkSize();
            }

            /**
             * Appends 2 bytes to the match.
             * Checks size, increases match length and appends bytes.
             */
            const inline void appendMatch16(uint16_t match) {
                appendMatch8((uint8_t) (match >> 8));
                appendMatch8((uint8_t) (match));
            }

            /**
             * Appends 4 bytes to the match.
             * Checks size, increases match length and appends bytes.
             */
            const inline void appendMatch32(uint32_t match) {
                appendMatch16((uint16_t) (match >> 16));
                appendMatch16((uint16_t) (match));
            }

            /**
             * Appends 8 bytes to the match.
             * Checks size, increases match length and appends bytes.
             */
            const inline void appendMatch64(uint64_t match) {
                appendMatch32((uint32_t) (match >> 32));
                appendMatch32((uint32_t) (match));
            }

            /**
             * Changes the prefix of this messageType with the given MessageType.
             * For Example if this MessageType contains "AABBCC" and the given MessageType consists of "QQE", this MessageType will end up containing "QQEBCC".
             *
             * @param other
             * 		the new prefix
             * @throws IllegalArgumentException
             * 		if the given MessageType is longer than this MessageType
             */
            void overridePrefixWith(MessageType& other) {
                if (other.match_.size() > this->match_.size()) {
                    throw 1;
                }
                for (unsigned long i = 0; i < other.match_.size(); i++) {
                    this->match_[i] = other.match_[i];
                }
            }

            bool operator==(const MessageType& b) const {
                return match_ == b.match_;
            }

            bool operator!=(const MessageType& b) const {
                return match_ != b.match_;
            }

            /**
             * Checks if this message type contains the other message type.
             * A Contains B if and only if:
             * A.length smaller equals B.length and all indices in A and B for the full range of A are the same
             *
             * @param other
             * 		the other message type to check if it is contained in this messageType.
             * @return true if this message type "contains" the given message type, false otherwise
             */
            bool containsTopic(const MessageType& b) const {
                return match_.size() <= b.match_.size() && std::equal(match_.begin(), match_.end(), b.match_.begin());
            }
        };

    }
}

#endif //ZMF_MESSAGETYPE_HPP
