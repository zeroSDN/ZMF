/**
* @author Jonas Grunert
* @author Jan Strauß
* @date created on 7/13/15.
*/

#include <sstream>
#include "ModuleEventDispatcher.hpp"

namespace zmf {
    namespace core {
        zmf::core::ModuleEventDispatcher::ModuleEventDispatcher(zmf::data::ModuleUniqueId id,
                                                                std::shared_ptr<zmf::messaging::IZmfMessagingService> msgService,
                                                                std::shared_ptr<zmf::config::IConfigurationProvider> config,
                                                                zmf::IZmfInstanceController* core)
                :
                logger_(zmf::logging::ZmfLogging::getLogger(id.getString() + " # ModuleEventDispatcher")),
                msgService_(msgService),
                config(config), core(core), subCounter_(0) {
        }

        // --------------------- State changes  --------------------- //

        bool zmf::core::ModuleEventDispatcher::start(std::shared_ptr<AbstractModule> selfModule,
                                                     std::shared_ptr<zmf::data::ModuleHandle> selfModuleHandle,
                                                     std::shared_ptr<zmf::config::IConfigurationProvider> configPtr) {
            selfModule_ = selfModule;
            selfModuleHandle_ = selfModuleHandle;


            // Load configs
            int32_t cfgTmp;
            if (config->getAsInt("ZMF_INMSG_BUFFER_SIZE", cfgTmp)) {
                ZMF_INMSG_BUFFER_SIZE = cfgTmp;
            }
            logger_.debug("ZMF_INMSG_BUFFER_SIZE configuration: " + std::to_string(ZMF_INMSG_BUFFER_SIZE));

            // Start
            bool zmqStart = msgService_->start(this, selfModuleHandle, config);

            if (zmqStart) {
                msgService_->peerJoin(selfModuleHandle);
            }

            return zmqStart;
        }

        void zmf::core::ModuleEventDispatcher::stop() {
            onDisable();
            msgService_->stop();
        }

        void zmf::core::ModuleEventDispatcher::onEnable() {
            bool expected = false;
            if (!alive_.compare_exchange_strong(expected, true)) { return; }
            thread_.reset(new std::thread(&ModuleEventDispatcher::deliveryLoop, this));
        }

        void zmf::core::ModuleEventDispatcher::onDisable() {

            // Check and set alive flag
            bool expected = true;
            if (!alive_.compare_exchange_strong(expected, false)) { return; }

            // Wake up queue and wait for thread to terminate
            metaQueue_.wakeUpAll();
            if (thread_->joinable()) {
                poco_trace(logger_, "ModuleEventDispatcher::onDisable: joining ModuleEventDispatcherThread now");
                try {
                    thread_->join();
                } catch (...) {  // Catch all
                    poco_error(logger_, "Failed to join ModuleEventDispatcherThread");
                }
                poco_trace(logger_, "ModuleEventDispatcher::onDisable: joined ModuleEventDispatcherThread");
            }
            metaQueue_.clear();
            eventQueue_.clear();
            stateQueue_.clear();
            requestQueue_.clear();

            // Unsubscribe all subscriptions
            moduleSubHandlerMutex_.lock();
            for (auto& handler : moduleSubscriptionHandlers_) {
                msgService_->unsubscribe(handler.second.getTopic());
            }
            moduleSubscriptionHandlers_.clear();
            moduleSubHandlerMutex_.unlock();

            // Disable Message service
            msgService_->onDisable();
        }


        // --------------------- From ZMQ  --------------------- //
        void zmf::core::ModuleEventDispatcher::onSubMsgReceived(const zmf::data::ZmfMessage& message,
                                                                const zmf::data::ModuleUniqueId& sender) {
            if (!alive_.load()) { return; }

            if (checkQueueSizeEventInsert()) {
                // Return if dropping message necessary
                return;
            }

            // Queue message to event queue
            eventQueue_.enqueueNotification(new EventNotification(message, sender));
            metaQueue_.enqueueNotification(new MetaNotification(MetaNotification::DELIVERY_EVENT));
        }

        void zmf::core::ModuleEventDispatcher::onRequestMsgReceived(const zmf::messaging::ExternalRequestIdentity id,
                                                                    const zmf::data::ZmfMessage& message,
                                                                    const zmf::data::ModuleUniqueId& sender) {
            if (SYSTEM_REQUEST.containsTopic(message.getType())) {
                handleSystemMessage(id, message);
                logger_.information("handled request as system message");
                return;
            }

            if (!alive_.load()) { return; }

            // Queue message to event queue
            requestQueue_.enqueueNotification(new RequestNotification(message, sender, id));
            metaQueue_.enqueueUrgentNotification(new MetaNotification(MetaNotification::DELIVERY_REQUEST));
        }


        bool ModuleEventDispatcher::checkQueueSizeEventInsert() {
            return ZMF_INMSG_BUFFER_SIZE > 0 && eventQueue_.size() >= ZMF_INMSG_BUFFER_SIZE;
        }

        uint32_t ModuleEventDispatcher::getSubId() {
            return subCounter_.fetch_add(1);
        }

        IZmfInstanceAccess::SubscriptionHandle zmf::core::ModuleEventDispatcher::subscribe(
                const zmf::data::MessageType& topic,
                std::function<void(
                        const zmf::data::ZmfMessage& msg,
                        const zmf::data::ModuleUniqueId& sender)> handler) {
            checkAlive();
            IZmfInstanceAccess::SubscriptionHandle subHandle(this, topic, handler);
            moduleSubHandlerMutex_.lock();
            moduleSubscriptionHandlers_.insert(
                    std::pair<uint32_t, IZmfInstanceAccess::SubscriptionHandle>(subHandle.SubId, subHandle));
            moduleSubHandlerMutex_.unlock();
            msgService_->subscribe(topic);
            poco_debug(logger_, "subscribed on topic: " + topic.toString());
            return subHandle;
        }

        void zmf::core::ModuleEventDispatcher::unsubscribe(IZmfInstanceAccess::SubscriptionHandle* handle) {
            checkAlive();
            moduleSubHandlerMutex_.lock();
            moduleSubscriptionHandlers_.erase(handle->SubId);
            moduleSubHandlerMutex_.unlock();
            msgService_->unsubscribe(handle->getTopic());
            poco_debug(logger_, "unsubscribed on topic: " + handle->getTopic().toString());
        }

        void zmf::core::ModuleEventDispatcher::publish(const zmf::data::ZmfMessage& msg) {
            checkAlive();
            msgService_->publish(msg);
        }

        zmf::data::ZmfInReply zmf::core::ModuleEventDispatcher::sendRequest(
                const zmf::data::ModuleUniqueId& target, const zmf::data::ZmfMessage& msg) {
            checkAlive();
            return msgService_->sendRequest(target, msg);
        }

        // --------------------- Loop  --------------------- //
        void ModuleEventDispatcher::deliveryLoop() {
            poco_information(logger_, "entering delivery loop");
            while (alive_.load()) {
                Poco::AutoPtr<Poco::Notification> incomingMessage(metaQueue_.waitDequeueNotification());
                MetaNotification* container = dynamic_cast<MetaNotification*>(incomingMessage.get());
                if (container) {
                    switch (container->type) {

                        case MetaNotification::DELIVERY_EVENT:
                            deliverEvent();
                            break;

                        case MetaNotification::DELIVERY_REQUEST:
                            deliverRequest();
                            break;

                        case MetaNotification::DELIVERY_STATE:
                            deliverState();
                            break;
                    }
                }
            }
            poco_information(logger_, "leaving delivery loop");
        }

        // --------------------- Util  --------------------- //
        void ModuleEventDispatcher::checkAlive() {
            if (!alive_.load()) {
                throw std::runtime_error("ModuleEventDispatcher is not started or has been stopped");
            }
        }

        void ModuleEventDispatcher::deliverEvent() {
            Poco::AutoPtr<Poco::Notification> eventNotification(eventQueue_.waitDequeueNotification());
            EventNotification* container = dynamic_cast<EventNotification*>(eventNotification.get());
            moduleSubHandlerMutex_.lock();
            bool found_sub = false;
            for (auto& it : moduleSubscriptionHandlers_) {

                const zmf::data::MessageType& sub_topic = it.second.getTopic();
                const zmf::data::MessageType& msg_topic = container->message.getType();

                if (sub_topic.containsTopic(msg_topic)) {
                    selfModule_->INTERNAL_getInternalMutex().lock();
                    // Try to deliver event
                    if (selfModule_->isEnabled()) {
                        try {
                            it.second.getCallback()(container->message, container->peerIdentity);
                        }
                        catch (Poco::Exception exc) { // Catch Exceptions
                            logger_.error("Exception when calling subscription callback while DELIVERY_EVENT");
                            logger_.log(exc);
                        }
                        catch (...) {  // Catch all
                            logger_.error("Exception when calling subscription callback while DELIVERY_EVENT");
                        }
                    }
                    selfModule_->INTERNAL_getInternalMutex().unlock();
                    found_sub = true;
                }
            }

            if (!found_sub && logger_.debug()) {
                std::ostringstream msg;
                msg << "received event topic(" << container->message.getType().toString() <<
                ") does not match any sub topic.";
                logger_.debug(msg.str());
            }

            moduleSubHandlerMutex_.unlock();
            poco_trace(logger_, "handled DELIVERY_EVENT");
        }

        void ModuleEventDispatcher::deliverRequest() {
            Poco::AutoPtr<Poco::Notification> requestNotification(
                    requestQueue_.waitDequeueNotification());
            RequestNotification* container = dynamic_cast<RequestNotification*>(requestNotification.get());


            selfModule_->INTERNAL_getInternalMutex().lock();
            if (selfModule_->isEnabled()) {
                // Try to do request handling
                try {

                    // Try to deliver request
                    zmf::data::ZmfOutReply reply = selfModule_->handleRequest(container->message,
                                                                              container->peerIdentity);
                    selfModule_->INTERNAL_getInternalMutex().unlock();

                    // Try to handle reply. Catch exception to avoid double unlocking mutex in catch blocks
                    try {
                        switch (reply.type) {
                            case zmf::data::ZmfOutReply::IMMEDIATE_REPLY:
                                msgService_->sendReply(container->message_id, reply.reply_immediate);
                                break;

                            case zmf::data::ZmfOutReply::FUTURE_REPLY:
                                reply.injectService(msgService_.get(), container->message_id);
                                break;

                            case zmf::data::ZmfOutReply::NO_REPLY:
                                break;
                        }

                        poco_trace(logger_, "handled DELIVERY_REQUEST");
                    }
                    catch (...) {  // Catch all
                        logger_.error("Exception when handling request while DELIVERY_REQUEST");
                        return;
                    }
                }
                catch (Poco::Exception exc) { // Catch Exceptions
                    logger_.error("Exception when calling handleRequest while DELIVERY_REQUEST");
                    logger_.log(exc);
                    selfModule_->INTERNAL_getInternalMutex().unlock();
                    return;
                }
                catch (...) {  // Catch all
                    logger_.error("Exception when calling handleRequest while DELIVERY_REQUEST");
                    selfModule_->INTERNAL_getInternalMutex().unlock();
                    return;
                }
            }
        }

        void ModuleEventDispatcher::deliverState() {
            Poco::AutoPtr<Poco::Notification> stateNotification(stateQueue_.waitDequeueNotification());
            StateNotification* container = dynamic_cast<StateNotification*>(stateNotification.get());

            // Try to deliver state change
            try {
                selfModule_->INTERNAL_getInternalMutex().lock();
                if (selfModule_->isEnabled()) {
                    selfModule_->handleModuleStateChange(container->module, container->newState, container->lastState);
                }
                selfModule_->INTERNAL_getInternalMutex().unlock();
                poco_trace(logger_, "handled DELIVERY_STATE");
            }
            catch (Poco::Exception exc) { // Catch Exceptions
                logger_.error("Exception when calling module while DELIVERY_STATE");
                logger_.log(exc);
            }
            catch (...) {  // Catch all
                logger_.error("Exception when calling module while DELIVERY_STATE");
            }

        }

        void ModuleEventDispatcher::onPeerChange(std::shared_ptr<zmf::data::ModuleHandle> module,
                                                 data::ModuleState newState,
                                                 data::ModuleState lastState) {
            // Notify ZMQ service
            if (lastState == zmf::data::ModuleState::Dead) {
                msgService_->peerJoin(module);
            }
            else if (newState == zmf::data::ModuleState::Dead) {
                msgService_->peerLeave(module);
            }

            // Dont block peer state changes (like incoming messages) and treat them as urgent
            stateQueue_.enqueueNotification(new StateNotification(module, newState, lastState));
            metaQueue_.enqueueUrgentNotification(new MetaNotification(MetaNotification::DELIVERY_STATE));
        }

        void ModuleEventDispatcher::handleSystemMessage(const messaging::ExternalRequestIdentity id,
                                                        const data::ZmfMessage& message) {
            if (SYSTEM_REQUEST_ENABLE.compare(message.getData()) == 0) {
                msgService_->sendReply(id, zmf::data::ZmfMessage("\x04\xff", "\x03"));
                core->requestEnableModule();

            } else if (SYSTEM_REQUEST_DISABLE.compare(message.getData()) == 0) {
                msgService_->sendReply(id, zmf::data::ZmfMessage("\x04\xff", "\x01"));
                core->requestDisableModule();

            } else if (SYSTEM_REQUEST_STOP.compare(message.getData()) == 0) {
                msgService_->sendReply(id, zmf::data::ZmfMessage("\x04\xff", "\x02"));
                core->requestStopInstance();

            } else {
                msgService_->sendReply(id, zmf::data::ZmfMessage("\x04\xff", "unknown system messsage"));
                logger_.error("received unknown system message: " + message.getData());
            }
        }
    }
}