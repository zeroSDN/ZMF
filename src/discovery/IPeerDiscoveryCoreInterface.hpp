#ifndef ZMF_IPEERDISCOVERYCOREINTERFACE_H
#define ZMF_IPEERDISCOVERYCOREINTERFACE_H

#include <memory>
#include <Poco/Logger.h>
#include "ModuleHandle.hpp"


namespace zmf {
    namespace discovery {

        /**
         * @details Interface to ZmfCore for peerStateChange events from peer discovery
         * @author Jonas Grunert
         * @date created on 6/25/15.
         */
        class IPeerDiscoveryCoreInterface {

        public:

            virtual void onPeerChange(std::shared_ptr<zmf::data::ModuleHandle> module,
                                      zmf::data::ModuleState newState,
                                      zmf::data::ModuleState lastState) = 0;
        };
    }
}


#endif //ZMF_IPEERDISCOVERYCOREINTERFACE_H
