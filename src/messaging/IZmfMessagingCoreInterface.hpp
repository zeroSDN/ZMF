#ifndef ZMF_IZMQCOREINTERFACE_H
#define ZMF_IZMQCOREINTERFACE_H

#include <Poco/Logger.h>
#include "../data/ZmfMessage.hpp"
#include "../data/ModuleUniqueId.hpp"

namespace zmf {
    namespace messaging {

        /**
         * @details Counterpart of the IZmfMessagingService interface. The implementation of the IZmfMessagingService will deliver incoming events to the ZMF core via this interface.
         * Additionally this class provides access to the logger to be used by the IZmfMessagingService implementation.
         * @author Jonas Grunert
         * @date created on 6/25/15.
         */
        class IZmfMessagingCoreInterface {

        public:

            /**
             * The IZmfMessagingService implementation will call this method when a message was received on a topic that matched the subscriptions.
             *
             * @param message the received message
             * @param sender the identity of the sender/publisher
             */
            virtual void onSubMsgReceived(const zmf::data::ZmfMessage& message,
                                          const zmf::data::ModuleUniqueId& sender) = 0;

            /**
             * The IZmfMessagingService implementation will call this method when a request message was received.
             *
             * @param id the request id
             * @param message the received request
             * @param sender the identity of the sender
             */
            virtual void onRequestMsgReceived(const zmf::messaging::ExternalRequestIdentity id,
                                              const zmf::data::ZmfMessage& message,
                                              const zmf::data::ModuleUniqueId& sender) = 0;
        };
    }
}


#endif //ZMF_IZMQCOREINTERFACE_H
