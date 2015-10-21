/**
 * @details TODO Description
 * @author zsdn 
 * @date created on 10/21/15
*/

#ifndef ZMF_MODULEUNIQUEIDINTERNAL_H
#define ZMF_MODULEUNIQUEIDINTERNAL_H

#include <data/ModuleUniqueId.hpp>
#include "../proto/FrameworkProto.pb.h"


namespace zmf {
    namespace data {
        /**
         * ID information of a module in the ZMF system.
         * Consists of Type ID (UInt16) and Instance ID (UInt64)
         * @author Jonas Grunert
         * @date created on 7/20/15.
        */
        class ModuleUniqueIdInternal : ModuleUniqueId {

        private:
            zmf::proto::SenderId senderId;

        public:
            ModuleUniqueIdInternal(zmf::proto::SenderId
                                   & senderId);

            ModuleUniqueIdInternal(std::string
                                   rawProto);

            virtual ModuleUniqueIdInternal(uint16_t typeId, uint64_t instanceId);


            std::string getSenderId() const;
        };
    }
}

#endif //ZMF_MODULEUNIQUEIDINTERNAL_H
