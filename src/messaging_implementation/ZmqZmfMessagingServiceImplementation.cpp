/**
 * @author Jan Strauß
 * @date created on 6/25/15.
*/

#include "ZmqZmfMessagingServiceImplementation.hpp"
#include "../data/ModuleHandleInternal.hpp"
#include "NetworkUtil.hpp"

namespace zmf {
    namespace messaging {

        ZmqZmfMessagingServiceImplementation::ZmqZmfMessagingServiceImplementation(zmf::data::ModuleUniqueId moduleId)
                :
                logger_main(zmf::logging::ZmfLogging::getLogger(
                        moduleId.getString() + " # ZmqZmfMessagingServiceImplementation.Class")),
                logger_poller(zmf::logging::ZmfLogging::getLogger(
                        moduleId.getString() + " # ZmqZmfMessagingServiceImplementation.Poller")) {
        }

        ZmqZmfMessagingServiceImplementation::~ZmqZmfMessagingServiceImplementation() {
            if (alive.load()) {
                stop();
            }
        }


        bool ZmqZmfMessagingServiceImplementation::start(IZmfMessagingCoreInterface* const corePtr,
                                                         std::shared_ptr<zmf::data::ModuleHandle> selfHandle,
                                                         std::shared_ptr<zmf::config::IConfigurationProvider> configPtr) {
            if (alive.load()) {
                poco_warning(logger_main, "start called but already alive, abort start");
                return false;
            }

            core = corePtr;
            self = selfHandle;
            selfSenderId.set_instanceid(self->UniqueId.InstanceId);
            selfSenderId.set_typeid_(self->UniqueId.TypeId);
            config = configPtr;

            // Load configs
            int32_t cfgTmp;
            if (config->getAsInt("ZMF_ZMQ_ZMQ_RCVBUF", cfgTmp)) {
                ZMF_ZMQ_ZMQ_RCVBUF = cfgTmp;
            }
            logger_main.debug("ZMF_ZMQ_ZMQ_RCVBUF configuration: " + std::to_string(ZMF_ZMQ_ZMQ_RCVBUF));

            if (config->getAsInt("ZMF_ZMQ_ZMQ_RCVHWM", cfgTmp)) {
                ZMF_ZMQ_ZMQ_RCVHWM = cfgTmp;
            }
            logger_main.debug("ZMF_ZMQ_ZMQ_RCVHWM configuration: " + std::to_string(ZMF_ZMQ_ZMQ_RCVHWM));

            if (config->getAsInt("ZMF_ZMQ_ZMQ_SNDBUF", cfgTmp)) {
                ZMF_ZMQ_ZMQ_SNDBUF = cfgTmp;
            }
            logger_main.debug("ZMF_ZMQ_ZMQ_SNDBUF configuration: " + std::to_string(ZMF_ZMQ_ZMQ_SNDBUF));

            if (config->getAsInt("ZMF_ZMQ_ZMQ_SNDHWM", cfgTmp)) {
                ZMF_ZMQ_ZMQ_SNDHWM = cfgTmp;
            }
            logger_main.debug("ZMF_ZMQ_ZMQ_SNDHWM configuration: " + std::to_string(ZMF_ZMQ_ZMQ_SNDHWM));

            context = zmqpp::context();
            context.set(zmqpp::context_option::max_sockets, 32768);
            context.set(zmqpp::context_option::io_threads, 1);

            socket_pub.reset(new zmqpp::socket(context, zmqpp::socket_type::pub));
            socket_sub.reset(new zmqpp::socket(context, zmqpp::socket_type::sub));

            socket_rep.reset(new zmqpp::socket(context, zmqpp::socket_type::router));
            socket_notify_send.reset(new zmqpp::socket(context, zmqpp::socket_type::push));
            socket_notify_recv.reset(new zmqpp::socket(context, zmqpp::socket_type::pull));

            socket_pub->set(zmqpp::socket_option::send_buffer_size, ZMF_ZMQ_ZMQ_SNDBUF);
            socket_rep->set(zmqpp::socket_option::receive_buffer_size, ZMF_ZMQ_ZMQ_RCVBUF);
            socket_sub->set(zmqpp::socket_option::receive_buffer_size, ZMF_ZMQ_ZMQ_RCVBUF);

            socket_pub->set(zmqpp::socket_option::send_high_water_mark, ZMF_ZMQ_ZMQ_SNDHWM);
            socket_rep->set(zmqpp::socket_option::receive_high_water_mark, ZMF_ZMQ_ZMQ_RCVHWM);
            socket_sub->set(zmqpp::socket_option::receive_high_water_mark, ZMF_ZMQ_ZMQ_RCVHWM);

            socket_pub->set(zmqpp::socket_option::linger, 0);
            socket_sub->set(zmqpp::socket_option::linger, 0);
            socket_rep->set(zmqpp::socket_option::linger, 0);
            socket_notify_recv->set(zmqpp::socket_option::linger, 0);
            socket_notify_send->set(zmqpp::socket_option::linger, 0);

            socket_notify_recv->bind(this->NOTIFY_ADDRESS);
            socket_notify_send->connect(this->NOTIFY_ADDRESS);

            try {
                socket_pub->bind(ENDPOINT_STRING);
                static_cast<zmf::data::ModuleHandleInternal*>(self.get())->selfSetPubPort(extractPort(socket_pub));
            }
            catch (const zmqpp::exception& e) {
                poco_critical(logger_main,
                              std::string("failed to start zmq service, binding pub socket failed: ") +
                              e.what());

                closeSockets();
                context.terminate();
                return false;
            }

            try {
                socket_rep->bind(ENDPOINT_STRING);
                static_cast<zmf::data::ModuleHandleInternal*>(self.get())->selfSetRepPort(extractPort(socket_rep));

            } catch (const zmqpp::exception& e) {
                poco_critical(logger_main,
                              std::string("failed to start zmq service, binding rep socket failed: ") +
                              e.what());
                closeSockets();
                context.terminate();
                return false;
            }

            alive.store(true);

            poller.reset(new zmqpp::poller());
            poller->add(*socket_sub, ZMQ_POLLIN);
            poller->add(*socket_rep, ZMQ_POLLIN);
            poller->add(*socket_notify_recv, ZMQ_POLLIN);

            poll_thread.reset(new std::thread(&ZmqZmfMessagingServiceImplementation::pollerLoop, this));


            logger_main.information("started zmq service");
            return true;

        }

        void ZmqZmfMessagingServiceImplementation::stop() {
            bool expected = true;
            if (!alive.compare_exchange_strong(expected, false)) {
                logger_main.warning("stop called but alive flag not set, aborting stop");
                return;
            }

            notifyPoller(NotifyType::SHUTDOWN);

            if (poll_thread->joinable()) {
                poco_trace(logger_main, "ZmfCore::stopInstance: joining zmfInstanceThread_ now");
                try {
                    poll_thread->join();
                } catch (...) {  // Catch all
                    poco_error(logger_main, "Failed to join zmfInstanceThread_");
                }
                poco_trace(logger_main, "ZmfCore::stopInstance: joined zmfInstanceThread_");
            }

            closeSockets();

            context.terminate();

            lock_map_sockets_req.lock();
            map_sockets_req.clear();
            lock_map_sockets_req.unlock();

            lock_map_requests.lock();
            outstanding_requests.clear();
            lock_map_requests.unlock();

            lock_queue_membership_changes.lock();
            queue_membership_changes.empty();
            lock_queue_membership_changes.unlock();

            lock_queue_subscription_changes.lock();
            queue_subscription_changes.empty();
            lock_queue_subscription_changes.unlock();

            lock_map_replies.lock();
            outstanding_replies.empty();
            lock_map_replies.unlock();

            logger_main.information("stopped zmq service");
        }

        void ZmqZmfMessagingServiceImplementation::closeSockets() {
            socket_pub->close();
            socket_sub->close();


            lock_map_sockets_req.lock();
            for (auto& map_entry : map_sockets_req) {
                map_entry.second->close();
            }
            lock_map_sockets_req.unlock();

            socket_rep->close();
            socket_notify_send->close();
            socket_notify_recv->close();
        }

        void ZmqZmfMessagingServiceImplementation::peerJoin(std::shared_ptr<zmf::data::ModuleHandle> module) {
            checkAlive();

            internalConnect(module->UniqueId,
                            static_cast<zmf::data::ModuleHandleInternal*>(module.get())->ZmqRepAddr,
                            static_cast<zmf::data::ModuleHandleInternal*>(module.get())->ZmqPubAddr);

            logger_main.trace("peer join: " + module->UniqueId.getString());
        }

        void ZmqZmfMessagingServiceImplementation::internalConnect(zmf::data::ModuleUniqueId identity,
                                                                   std::string rep_addr,
                                                                   std::string pub_addr) {
            lock_map_sockets_req.lock();
            if (map_sockets_req.count(identity) == 0) {
                std::unique_ptr<zmqpp::socket> socket = std::unique_ptr<zmqpp::socket>(
                        new zmqpp::socket(context, zmqpp::socket_type::dealer));
                socket->set(zmqpp::socket_option::linger, 0);
                socket->set(zmqpp::socket_option::send_buffer_size, ZMF_ZMQ_ZMQ_SNDBUF);
                socket->set(zmqpp::socket_option::send_high_water_mark, ZMF_ZMQ_ZMQ_SNDHWM);
                socket->connect(rep_addr);


                std::string ip = getIp();
                std::string self_rep_addr = "tcp://" + ip + ":" + std::to_string(
                        static_cast<zmf::data::ModuleHandleInternal*>(self.get())->self_ZmqRepPort);
                std::string self_pub_addr = "tcp://" + ip + ":" + std::to_string(
                        static_cast<zmf::data::ModuleHandleInternal*>(self.get())->self_ZmqPubPort);

                zmqpp::message hello;
                hello.add(MESSAGE_TYPE_HELLO); // msg type
                hello.add(selfSenderId.SerializeAsString()); // identity
                hello.add(self_rep_addr); // pub addr
                hello.add(self_pub_addr); // sub addr
                socket->send(hello);
                map_sockets_req[identity] = std::move(socket);

                lock_queue_membership_changes.lock();
                queue_membership_changes.push(std::make_pair(pub_addr, true));
                lock_queue_membership_changes.unlock();

                notifyPoller(NotifyType::MEMBERSHIP_CHANGE);
            } else {
                logger_main.debug("tried to add peer already known: " + identity.getString());
            }

            lock_map_sockets_req.unlock();
        }

        void ZmqZmfMessagingServiceImplementation::peerLeave(std::shared_ptr<zmf::data::ModuleHandle> module) {
            checkAlive();

            zmf::data::ModuleUniqueId key = module->UniqueId;
            lock_map_sockets_req.lock();
            if (map_sockets_req.count(key) > 0) {
                map_sockets_req[key]->close();
                map_sockets_req.erase(key);

                std::string pub_addr = static_cast<zmf::data::ModuleHandleInternal*>(module.get())->ZmqPubAddr;

                lock_queue_membership_changes.lock();
                queue_membership_changes.push(std::make_pair(pub_addr, false));
                lock_queue_membership_changes.unlock();

                notifyPoller(NotifyType::MEMBERSHIP_CHANGE);
            }
            lock_map_sockets_req.unlock();

            //logger_main.trace("peer leave");
        }

        void ZmqZmfMessagingServiceImplementation::subscribe(const zmf::data::MessageType& topic) {
            checkAlive();
            lock_queue_subscription_changes.lock();
            queue_subscription_changes.push(std::make_pair(topic, true));
            lock_queue_subscription_changes.unlock();

            notifyPoller(NotifyType::SUBSCRIPTION_CHANGE);

            //logger_main.trace("add subscribe");
        }

        void ZmqZmfMessagingServiceImplementation::unsubscribe(const zmf::data::MessageType& topic) {
            checkAlive();
            lock_queue_subscription_changes.lock();
            queue_subscription_changes.push(std::make_pair(topic, false));
            lock_queue_subscription_changes.unlock();

            notifyPoller(NotifyType::SUBSCRIPTION_CHANGE);

            //logger_main.trace("remove subscribe");
        }

        void ZmqZmfMessagingServiceImplementation::publish(const zmf::data::ZmfMessage& msg) {
            checkAlive();

            zmqpp::message event;

            event.add(msg.getType().getMatch());
            event.add(selfSenderId.SerializeAsString());
            event.add(msg.getData());

            lock_pub_socket.lock();
            socket_pub->send(event);
            lock_pub_socket.unlock();
        }

        zmf::data::ZmfInReply ZmqZmfMessagingServiceImplementation::sendRequest(const zmf::data::ModuleUniqueId& target,
                                                                                const zmf::data::ZmfMessage& msg) {
            checkAlive();
            lock_ids.lock();
            uint64_t id = next_request_id++;
            lock_ids.unlock();

            lock_map_requests.lock();
            outstanding_requests[id] = std::promise<zmf::data::ZmfMessage>();
            auto future = outstanding_requests[id].get_future();
            lock_map_requests.unlock();

            zmqpp::message request;

            request.add(MESSAGE_TYPE_REQUEST); // type
            request.add(id); // msg_id
            request.add(selfSenderId.SerializeAsString()); // sender
            request.add(msg.getType().getMatch()); // match
            request.add(msg.getData()); // payload

            lock_map_sockets_req.lock();
            if (map_sockets_req.count(target) > 0) {
                map_sockets_req[target]->send(request);
            } else {
                lock_map_requests.lock();
                outstanding_requests[id].set_exception(
                        std::make_exception_ptr(std::runtime_error(
                                "zmf::messaging::target_unknown on send request to" + target.getString())));
                outstanding_requests.erase(id);
                lock_map_requests.unlock();
            }
            lock_map_sockets_req.unlock();

            //logger_main.trace("send request");

            return zmf::data::ZmfInReply(id, std::move(future), this);
        }

        void ZmqZmfMessagingServiceImplementation::sendReply(ExternalRequestIdentity requestID,
                                                             const zmf::data::ZmfMessage& reply_internal) {
            checkAlive();
            zmqpp::message reply;
            zmf::data::ModuleUniqueId addr;

            lock_map_replies.lock();
            auto found(outstanding_replies.count(requestID));
            if (found) {
                addr = outstanding_replies[requestID];
                outstanding_replies.erase(requestID);
            }
            lock_map_replies.unlock();

            if (!found) {
                return;
            }

            reply.add(MESSAGE_TYPE_REPLY); // request/reply
            reply.add(requestID.requestId); // msg_id
            reply.add(reply_internal.getType().getMatch()); // match
            reply.add(reply_internal.getData()); // payload

            lock_map_sockets_req.lock();
            if (map_sockets_req.count(addr) > 0) {
                map_sockets_req[addr]->send(reply);
            } else {
                logger_main.error("zmf::messaging::target_unknown");
            }
            lock_map_sockets_req.unlock();

            // logger_main.trace("send reply");
        }

        void ZmqZmfMessagingServiceImplementation::notifyPoller(const NotifyType type) {
            lock_notify_send.lock();
            socket_notify_send->send_raw(reinterpret_cast<const char*>(&type), sizeof(type));
            lock_notify_send.unlock();
        }

        void ZmqZmfMessagingServiceImplementation::pollerLoop() { // ########## POLLER THREAD ##########
            logger_poller.information("starting poller loop");

            while (alive.load()) {

                poller->poll(); // wait until one of the sockets has input

                if (poller->has_input(*socket_sub)) { // handle sub
                    handleSubIn();
                }
                else if (poller->has_input(*socket_notify_recv)) { // handle local events
                    handleNotify();
                }
                else if (poller->has_input(*socket_rep)) { // handle rep
                    handleRepIn();
                }
            }

            logger_poller.information("leaving poller loop");
        }

        void ZmqZmfMessagingServiceImplementation::handleNotify() { // ########## POLLER THREAD ##########

            NotifyType notify_type;
            size_t length = sizeof(notify_type);
            socket_notify_recv->receive_raw(reinterpret_cast<char*>(&notify_type), length);

            switch (notify_type) {

                case SUBSCRIPTION_CHANGE:
                    applySubscriptionChange();
                    break;

                case MEMBERSHIP_CHANGE:
                    applyMembershipChange();
                    break;

                case SHUTDOWN:
                    logger_poller.information("received shutdown");
                    break;
            }

            //logger_poller.trace("handled notify: " + std::to_string(notify_type));
        }

        void ZmqZmfMessagingServiceImplementation::applyMembershipChange() { // ########## POLLER THREAD ##########
            lock_queue_membership_changes.lock();

            if (!queue_membership_changes.empty()) {
                auto change = queue_membership_changes.front();
                queue_membership_changes.pop();

                //logger_poller.debug("membership change " + std::to_string(change.second) + " | " + change.first);

                try {
                    if (change.second) {
                        socket_sub->connect(change.first);
                    } else {
                        socket_sub->disconnect(change.first);
                    }
                } catch (const zmqpp::exception& e) {
                    std::string what = e.what();
                    std::stringstream error_msg;
                    error_msg << "failed to change sub socket: " << change.second << " with peer " << change.first <<
                    " reason: " << what;
                    std::string error = error_msg.str();
                    logger_poller.error(error);
                }


            }

            lock_queue_membership_changes.unlock();
        }

        void ZmqZmfMessagingServiceImplementation::applySubscriptionChange() { // ########## POLLER THREAD ##########
            lock_queue_subscription_changes.lock();
            if (!queue_subscription_changes.empty()) {
                auto change = queue_subscription_changes.front();
                queue_subscription_changes.pop();

                poco_trace(logger_poller, "applying subscription change..");
                try {
                    if (change.second) {
                        socket_sub->subscribe(change.first.getMatch());
                    } else {
                        socket_sub->unsubscribe(change.first.getMatch());
                    }
                } catch (const zmqpp::exception& e) {
                    logger_poller.error(
                            std::string("[failed to apply subscription change: ").append(e.what()));
                }
            }
            lock_queue_subscription_changes.unlock();
        }

        void ZmqZmfMessagingServiceImplementation::handleRepIn() { // ########## POLLER THREAD ##########
            while (true) {
                zmqpp::message message;

                if (!socket_rep->receive(message, true)) { break; }

                uint8_t type;
                message.get(type, 1);

                switch (type) {
                    case MESSAGE_TYPE_REQUEST:
                        handleRequestReceived(message);
                        break;

                    case MESSAGE_TYPE_REPLY:
                        handleReplyReceived(message);
                        break;

                    case MESSAGE_TYPE_HELLO:
                        handleHelloReceived(message);
                        break;

                    default:
                        logger_poller.warning("received unkown message type: " + std::to_string(type));
                        break;
                }
            }
        }

        void ZmqZmfMessagingServiceImplementation::handleRequestReceived(zmqpp::message& message) {
            // get sender identity
            std::string rawProto = message.get(3);
            zmf::proto::SenderId senderId;
            senderId.ParseFromString(rawProto);
            zmf::data::ModuleUniqueId request_identity = zmf::data::ModuleUniqueId((uint16_t) senderId.typeid_(),
                                                                                   senderId.instanceid());

            // get id
            uint64_t id;
            message.get(id, 2);
            ExternalRequestIdentity identity(request_identity, id);

            // build message
            zmf::data::ZmfMessage request_internal(message.get(4), message.get(5));

            // store peer addr
            lock_map_replies.lock();
            outstanding_replies[identity] = request_identity;
            lock_map_replies.unlock();


            core->onRequestMsgReceived(identity, request_internal, request_identity);
        }

        void ZmqZmfMessagingServiceImplementation::handleReplyReceived(zmqpp::message& message) {

            // get msg id
            uint64_t id;
            message.get(id, 2);

            lock_map_requests.lock();
            if (outstanding_requests.count(id) > 0) {
                outstanding_requests.at(id).set_value(
                        zmf::data::ZmfMessage(zmf::data::MessageType(message.get(3)), message.get(4)));
                outstanding_requests.erase(id);
            } else {
                logger_poller.warning("received reply for unkown request id=" + std::to_string(id));
                printMessage(message);
            }
            lock_map_requests.unlock();
        }

        void ZmqZmfMessagingServiceImplementation::handleHelloReceived(zmqpp::message& message) {
            // identity
            std::string rawProto = message.get(2);
            zmf::proto::SenderId senderId;
            senderId.ParseFromString(rawProto);
            zmf::data::ModuleUniqueId identity = zmf::data::ModuleUniqueId((uint16_t) senderId.typeid_(),
                                                                           senderId.instanceid());

            std::string rep_addr;
            message.get(rep_addr, 3);

            std::string pub_addr;
            message.get(pub_addr, 4);


            internalConnect(identity, rep_addr, pub_addr);

            logger_poller.trace("handled hello msg");
        }

        void ZmqZmfMessagingServiceImplementation::handleSubIn() { // ########## POLLER THREAD ###########

            while (true) {

                // poco_trace(logger_poller, "sub received");
                zmqpp::message event;

                if (!socket_sub->receive(event, true)) { break; }

                //printMessage(event);

                // get sender id
                std::string rawProto = event.get(1);
                zmf::proto::SenderId senderId;
                senderId.ParseFromString(rawProto);
                zmf::data::ModuleUniqueId uniqueId = zmf::data::ModuleUniqueId((uint16_t) senderId.typeid_(),
                                                                               senderId.instanceid());

                // build msg
                zmf::data::ZmfMessage message(event.get(0), event.get(2));

                core->onSubMsgReceived(message, uniqueId);

            }
        }

        void ZmqZmfMessagingServiceImplementation::cancelRequest(uint64_t requestID, bool manual) {
            if (!alive.load()) { return; }

            lock_map_requests.lock();
            outstanding_requests.erase(requestID);
            lock_map_requests.unlock();

            if (manual) {
                logger_main.trace("canceled request with id=" + std::to_string(requestID));
            }
        }

        void ZmqZmfMessagingServiceImplementation::onDisable() {
            checkAlive();

            lock_map_requests.lock();
            outstanding_requests.clear();
            lock_map_requests.unlock();

            lock_map_replies.lock();
            outstanding_replies.clear();
            lock_map_replies.unlock();
        }

        void ZmqZmfMessagingServiceImplementation::checkAlive() {
            if (!alive.load()) {
                throw std::runtime_error("ZmqZmfMessagingServiceImplementation is not started or has been stopped");
            }
        }

        uint16_t ZmqZmfMessagingServiceImplementation::extractPort(std::unique_ptr<zmqpp::socket>& socket) const {
            std::string last_endpoint;
            socket->get(zmqpp::socket_option::last_endpoint, last_endpoint);
            size_t port_colon_pos = last_endpoint.find_last_of(':');
            return static_cast<uint16_t>(std::stoi(last_endpoint.substr(port_colon_pos + 1)));
        }


        void ZmqZmfMessagingServiceImplementation::printMessage(zmqpp::message& msg) {
            std::cout << "=== MSG BEGIN" << std::endl;
            for (size_t i = 0; i < msg.parts(); ++i) {
                std::cout << i << "= " << msg.get(i) << std::endl;
            }
            std::cout << "=== MSG END" << std::endl;
        }

        std::string ZmqZmfMessagingServiceImplementation::getIp() {

            auto ip_ver = Poco::Net::IPAddress::Family::IPv4;

            std::string ifaceName;
            if (config->getAsString("ZMF_NETWORK_INTERFACE_NAME", ifaceName) && ifaceName.size() != 0) {
                return zmf::util::NetworkUtil::getNetworkInterface(ifaceName, ip_ver).firstAddress(ip_ver).toString();
            } else {
                return zmf::util::NetworkUtil::getNetworkInterface(ip_ver).firstAddress(ip_ver).toString();
            }
        }
    }
}