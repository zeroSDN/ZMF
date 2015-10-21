#ifndef ZMF_MODULEHANDLE_H
#define ZMF_MODULEHANDLE_H


#include <stdint.h>
#include <string>
#include <vector>
#include <mutex>
#include "MessageType.hpp"
#include "ZmfMessage.hpp"
#include "ModuleUniqueId.hpp"
#include "ZmfInReply.hpp"
#include "ModuleState.hpp"

namespace zmf {
    namespace data {
        /**
         * Interface to ID information of a module in the ZMF system.
         * Used to access known peers in the system.
         * @author Jonas Grunert
         * @date created on 6/25/15.
        */
        class ModuleHandle {

        public:

            /** Unique identifier of the module (type+instance id) */
            const zmf::data::ModuleUniqueId UniqueId;
            /** Version of this module */
            const uint16_t Version;
            /** Name of module (human readable) */
            const std::string Name;

            bool operator<(const ModuleHandle& o) const {
                return UniqueId < o.UniqueId;
            }

            bool operator==(const ModuleHandle& o) const {
                return UniqueId == o.UniqueId;
            }

            bool operator!=(const ModuleHandle& o) const {
                return UniqueId != o.UniqueId;
            }

        protected:
            ModuleHandle(zmf::data::ModuleUniqueId moduleId, uint16_t version, std::string name) : UniqueId(moduleId),
                                                                                                   Version(version),
                                                                                                   Name(name) { }
        };

    }
}


#endif //ZMF_MODULEHANDLE_H
