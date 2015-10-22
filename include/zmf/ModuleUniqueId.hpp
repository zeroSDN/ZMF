#ifndef ZMF_MODULEUNIQUEID_HPP
#define ZMF_MODULEUNIQUEID_HPP

#include <string>
#include <stdint.h>
#include <Poco/DynamicAny.h>
//#include "../proto/FrameworkProto.pb.h"

namespace zmf {
    namespace data {
        /**
         * ID information of a module in the ZMF system.
         * Consists of Type ID (UInt16) and Instance ID (UInt64)
         * @author Jonas Grunert
         * @date created on 7/20/15.
        */
        struct ModuleUniqueId {

            /** ID describing the module type */
            uint16_t TypeId;
            /** ID describing the module instance */
            uint64_t InstanceId;
            //zmf::proto::SenderId senderId;

            ModuleUniqueId() : TypeId(0), InstanceId(0) { }

            ModuleUniqueId(uint16_t typeId, uint64_t instanceId) : TypeId(typeId), InstanceId(instanceId) {
                //zmf::proto::SenderI senderId.set_instanceid(InstanceId);
                //senderId.set_typeid_(TypeId);
            }

            /*ModuleUniqueId(zmf::proto::SenderId& senderId) {
                senderId = senderId;
                TypeId = (uint16_t) senderId.typeid_();
                InstanceId = senderId.instanceid();
            }

            ModuleUniqueId(std::string rawProto) {
                senderId.ParseFromString(rawProto);
                TypeId = (uint16_t) senderId.typeid_();
                InstanceId = senderId.instanceid();
            }*/

            bool operator<(const ModuleUniqueId& o) const {
                return (TypeId < o.TypeId || (TypeId == o.TypeId && InstanceId < o.InstanceId));
            }

            bool operator>(const ModuleUniqueId& o) const {
                return (TypeId > o.TypeId || (TypeId == o.TypeId && InstanceId > o.InstanceId));
            }

            bool operator==(const ModuleUniqueId& o) const {
                return TypeId == o.TypeId && InstanceId == o.InstanceId;
            }

            bool operator!=(const ModuleUniqueId& o) const {
                return TypeId != o.TypeId || InstanceId != o.InstanceId;
            }

            std::string getString() const {
                return std::to_string(TypeId) + std::string(":") + std::to_string(InstanceId);
            }

            /*std::string getSenderId() const {
                return senderId.SerializeAsString();
            }*/
        };
    }
}
#endif //ZMF_MODULEUNIQUEID_HPP
