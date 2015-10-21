//
// Created by Jonas Grunert on 7/13/15.
//

#ifndef ZMF_EVENTDISPATCHER_H
#define ZMF_EVENTDISPATCHER_H

#include <thread>
#include <map>
#include <list>
#include <Poco/Logger.h>
#include <Poco/FileChannel.h>
#include <Poco/ConsoleChannel.h>
#include <Poco/SplitterChannel.h>
#include <Poco/FormattingChannel.h>
#include <Poco/PatternFormatter.h>
#include <Poco/NotificationQueue.h>
#include <AbstractModule.hpp>
#include <IZmfInstanceAccess.hpp>
#include <IZmfInstanceController.hpp>
#include <config/IConfigurationProvider.hpp>
#include "../messaging/IZmfMessagingService.hpp"

namespace zmf {
    namespace core {
        /**
         * @details Event dispatching class receiving ZMQ messages (events, requests) and PeerStateChanges.
         * The events are queued in a Poco notification queue and are delivered by one processing thread.
         * This guarantees singlethreaded calls to module implementations
         * @author Jonas Grunert
         * @author Jan Strauß
         * @date created on 7/13/15.
        */
        class ModuleEventDispatcher
                : public zmf::messaging::IZmfMessagingCoreInterface,
                  public IZmfInstanceAccess::SubscriptionHandle::ISubscriptionHandler {
        public:
            ModuleEventDispatcher(zmf::data::ModuleUniqueId id,
                                  std::shared_ptr<zmf::messaging::IZmfMessagingService> msgService,
                                  std::shared_ptr<zmf::config::IConfigurationProvider> config,
                                  zmf::IZmfInstanceController* core);

            bool start(std::shared_ptr<AbstractModule> selfModule_,
                       std::shared_ptr<zmf::data::ModuleHandle> selfModuleHandle,
                       std::shared_ptr<zmf::config::IConfigurationProvider> configPtr);

            void stop();

            /** Called when disabling a module - clears all subscriptions and stops processing thread.*/
            void onDisable();

            /** Called when enabling a module - starts processing thread. */
            void onEnable();

            // --------------------- From ZMQ  --------------------- //
            virtual void onSubMsgReceived(const zmf::data::ZmfMessage& message,
                                          const zmf::data::ModuleUniqueId& sender);

            virtual void onRequestMsgReceived(const zmf::messaging::ExternalRequestIdentity id,
                                              const zmf::data::ZmfMessage& message,
                                              const zmf::data::ModuleUniqueId& sender);


            // --------------------- From Core  --------------------- //

            void onPeerChange(std::shared_ptr<zmf::data::ModuleHandle> module,
                              data::ModuleState newState, zmf::data::ModuleState lastState);

            IZmfInstanceAccess::SubscriptionHandle subscribe(const zmf::data::MessageType& topic,
                                                             std::function<void(const zmf::data::ZmfMessage& msg,
                                                                                const zmf::data::ModuleUniqueId& sender)> handler);

            void unsubscribe(IZmfInstanceAccess::SubscriptionHandle* handle);

            void publish(const zmf::data::ZmfMessage& msg);

            zmf::data::ZmfInReply sendRequest(const zmf::data::ModuleUniqueId& target,
                                              const zmf::data::ZmfMessage& msg);


            virtual uint32_t getSubId();

        private:

            class MetaNotification : public Poco::Notification {

            public:
                const enum DeliveryMethod {
                    DELIVERY_EVENT, DELIVERY_REQUEST, DELIVERY_STATE
                } type;

                MetaNotification(const DeliveryMethod& type) : type(type) { }
            };

            class EventNotification : public Poco::Notification {
            public:

                EventNotification(const data::ZmfMessage& message,
                                  const data::ModuleUniqueId& peerIdentity) : message(message),
                                                                              peerIdentity(peerIdentity) { }

                const zmf::data::ZmfMessage message;
                const zmf::data::ModuleUniqueId peerIdentity;
            };

            class RequestNotification : public Poco::Notification {
            public:


                RequestNotification(const data::ZmfMessage& message,
                                    const data::ModuleUniqueId& peerIdentity,
                                    const messaging::ExternalRequestIdentity& message_id) : message(message),
                                                                                            peerIdentity(peerIdentity),
                                                                                            message_id(message_id) { }

                const zmf::data::ZmfMessage message;
                const zmf::data::ModuleUniqueId peerIdentity;
                const zmf::messaging::ExternalRequestIdentity message_id;
            };

            class StateNotification : public Poco::Notification {
            public:


                StateNotification(const std::shared_ptr<data::ModuleHandle>& module,
                                  const data::ModuleState& newState,
                                  const data::ModuleState& lastState) : module(module),
                                                                        newState(newState),
                                                                        lastState(lastState) { }

                const std::shared_ptr<zmf::data::ModuleHandle> module;
                const zmf::data::ModuleState newState;
                const zmf::data::ModuleState lastState;
            };


            Poco::Logger& logger_;

            /** Module to be operated by this instance */
            std::shared_ptr<AbstractModule> selfModule_;
            /** Module state handle of this instances module */
            std::shared_ptr<zmf::data::ModuleHandle> selfModuleHandle_;

            std::shared_ptr<zmf::config::IConfigurationProvider> config;

            /** All subscriptions of the module operated by this instance */
            std::map<uint32_t, IZmfInstanceAccess::SubscriptionHandle> moduleSubscriptionHandlers_;
            std::mutex moduleSubHandlerMutex_;

            /** Service operating the message bus */
            std::shared_ptr<zmf::messaging::IZmfMessagingService> msgService_;

            std::unique_ptr<std::thread> thread_;

            std::atomic_bool alive_ = ATOMIC_VAR_INIT(false);

            /** Poco notification queue queueing messages and notifications to be delivered */
            Poco::NotificationQueue metaQueue_;
            Poco::NotificationQueue eventQueue_;
            Poco::NotificationQueue requestQueue_;
            Poco::NotificationQueue stateQueue_;


            int32_t ZMF_INMSG_BUFFER_SIZE = 100000;

            std::atomic_uint subCounter_;

            zmf::IZmfInstanceController* core;

            zmf::data::MessageType SYSTEM_REQUEST = zmf::data::MessageType("\x03\xff");
            std::string SYSTEM_REQUEST_ENABLE = "\x03";
            std::string SYSTEM_REQUEST_DISABLE = "\x01";
            std::string SYSTEM_REQUEST_STOP = "\x02";

            /**
             * Check before inserting into message queue.
             * Based on configuration do nothing, wait or drop.
             * @return True if keep packet, false if drop
             */
            bool checkQueueSizeEventInsert();

            /**
             * The loop that runs in a thread that will keep track of every incoming event
             */
            void deliveryLoop();

            void checkAlive();

            void deliverEvent();

            void deliverRequest();

            void deliverState();

            void handleSystemMessage(const messaging::ExternalRequestIdentity id,
                                     const data::ZmfMessage& message);
        };
    }
}

#endif //ZMF_EVENTDISPATCHER_H
