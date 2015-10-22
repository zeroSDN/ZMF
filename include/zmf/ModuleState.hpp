#ifndef ZMF_MODULESTATE_HPP
#define ZMF_MODULESTATE_HPP

namespace zmf {
    namespace data {
        /**
         * @details Lifecycle state of a module, dead (shut down), inactive (standby) or active
         * @author Jonas Grunert
         * @date created on 8/8/15.
        */
        enum ModuleState {
            Dead = 0,
            Inactive = 1,
            Active = 2,
        };
    }
}
#endif //ZMF_MODULESTATE_HPP
