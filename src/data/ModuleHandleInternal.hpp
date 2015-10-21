#ifndef ZMF_MODULEHANDLEINTERNAL_HPP
#define ZMF_MODULEHANDLEINTERNAL_HPP

#include <data/ModuleHandle.hpp>

namespace zmf {
    namespace data {
        /**
         * Internal representation of ID information of a module in the ZMF system.
         * Used to store known peers in the system and to offer public access it via ModuleHandle interface.
         * @author Jonas Grunert
         * @date created on 7/7/15.
        */
        class ModuleHandleInternal : public ModuleHandle {

        public:

            /** local ZMQ socket port, could be a short but since protobuf only knows uint32... to be set via method */
            uint32_t self_ZmqPubPort;
            /** local ZMQ socket port, could be a short but since protobuf only knows uint32... to be set via method */
            uint32_t self_ZmqRepPort;

            /** ZMQ publish socket address for this module */
            std::string ZmqPubAddr;
            /** ZMQ reply socket address for this module */
            std::string ZmqRepAddr;

            uint32_t PeerTimeout = 0;

            const bool self;

            ModuleHandleInternal(zmf::data::ModuleUniqueId moduleId, uint16_t version, std::string name,
                                 bool isSelfHandle) :
                    ModuleHandle(moduleId, version, name), self(isSelfHandle) { }


            ModuleHandleInternal(zmf::data::ModuleUniqueId
                                 moduleId,
                                 uint16_t version,
                                 std::string name,
                                 std::string
                                 zmqPubAddr,
                                 std::string zmqRepAddr,
                                 bool isSelfHandle) :
                    ModuleHandle(moduleId, version, name),
                    ZmqPubAddr(zmqPubAddr),
                    ZmqRepAddr(zmqRepAddr),
                    self(isSelfHandle) { }


            void selfSetPubPort(uint16_t port) {
                if (!self) { return; }
                self_ZmqPubPort = port;
                ZmqPubAddr = "tcp://127.0.0.1:" + std::to_string(port);
            }

            void selfSetRepPort(uint16_t port) {
                if (!self) { return; }
                self_ZmqRepPort = port;
                ZmqRepAddr = "tcp://127.0.0.1:" + std::to_string(port);
            }
        };
    }
}

#endif //ZMF_MODULEHANDLEINTERNAL_HPP
