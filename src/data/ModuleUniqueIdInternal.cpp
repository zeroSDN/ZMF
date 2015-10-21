/**
 * @details TODO Description
 * @author zsdn 
 * @date created on 10/21/15
*/

#include "ModuleUniqueIdInternal.hpp"

namespace zmf {
    namespace data {
        ModuleUniqueIdInternal::ModuleUniqueIdInternal(zmf::proto::SenderId
                                                       & senderId) {
            senderId = senderId;
            TypeId = (uint16_t) senderId.typeid_();
            InstanceId = senderId.instanceid();
        }

        ModuleUniqueIdInternal::ModuleUniqueIdInternal(std::string
                                                       rawProto) {
            senderId.ParseFromString(rawProto);
            TypeId = (uint16_t) senderId.typeid_();
            InstanceId = senderId.instanceid();
        }

        ModuleUniqueIdInternal::ModuleUniqueIdInternal(uint16_t typeId, uint64_t instanceId) : TypeId(typeId),
                                                                                               InstanceId(instanceId) {
            senderId.set_instanceid(InstanceId);
            senderId.set_typeid_(TypeId);
        }

        std::string ModuleUniqueIdInternal::getSenderId() const {
            return senderId.SerializeAsString();
        }
    };
}
}
