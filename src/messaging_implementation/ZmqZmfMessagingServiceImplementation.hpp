#ifndef ZMF_ZMQ_SERVICE_H
#define ZMF_ZMQ_SERVICE_H

#include <data/ModuleHandle.hpp>
#include "../messaging/IZmfMessagingService.hpp"
#include <messaging_implementation/ExternalRequestIdentity.hpp>
#include "../messaging/IZmfMessagingCoreInterface.hpp"
#include "../proto/FrameworkProto.pb.h"
#include <zmqpp/message.hpp>
#include <zmqpp/poller.hpp>
#include <zmqpp/socket.hpp>
#include <zmqpp/context.hpp>
#include <queue>
#include <mutex>
#include <map>
#include <future>
#include <Poco/DynamicAny.h>
#include <sstream>
#include <iostream>
#include <stdint.h>

namespace zmf {
    namespace messaging {

        /**
         * @details "real" implementation of the IZmfMessagingService. Public interface matches IZmfMessagingService
         * @author Jan Strauß
         * @date created on 7/7/15.
         */
        class ZmqZmfMessagingServiceImplementation : public IZmfMessagingService {
        public:

            ZmqZmfMessagingServiceImplementation(zmf::data::ModuleUniqueId moduleId);

            virtual ~ZmqZmfMessagingServiceImplementation();

            virtual bool start(IZmfMessagingCoreInterface* const corePtr,
                               std::shared_ptr<zmf::data::ModuleHandle> selfHandle,
                               std::shared_ptr<zmf::config::IConfigurationProvider> config);

            virtual void stop();

            virtual void peerJoin(std::shared_ptr<zmf::data::ModuleHandle> module);

            virtual void peerLeave(std::shared_ptr<zmf::data::ModuleHandle> module);

            virtual void subscribe(const zmf::data::MessageType& topic);

            virtual void unsubscribe(const zmf::data::MessageType& topic);

            virtual void publish(const zmf::data::ZmfMessage& msg);

            virtual zmf::data::ZmfInReply sendRequest(
                    const zmf::data::ModuleUniqueId& target, const zmf::data::ZmfMessage& msg);

            virtual void cancelRequest(uint64_t requestID, bool manual);

            virtual void sendReply(ExternalRequestIdentity requestID, const zmf::data::ZmfMessage& response);

            virtual void onDisable();

        private:
            /**
             * address of the inproc notification channel
             */
            const std::string NOTIFY_ADDRESS = "inproc://zmf_zmq_notify";
            /**
             * address to bind the pub and rep sockets on
             */
            const std::string ENDPOINT_STRING = "tcp://*:*";

            static const uint8_t MESSAGE_TYPE_REQUEST = 0;
            static const uint8_t MESSAGE_TYPE_REPLY = 1;
            static const uint8_t MESSAGE_TYPE_HELLO = 2;

            /**
             * enum containing the possible notification types
             */
            enum NotifyType {
                SUBSCRIPTION_CHANGE,
                MEMBERSHIP_CHANGE,
                SHUTDOWN,
            };

            /**
             * pointer to the counterpart interface
             */
            IZmfMessagingCoreInterface* core;

            /**
             * pointer to the config
             */
            std::shared_ptr<config::IConfigurationProvider> config;

            Poco::Logger& logger_main;
            Poco::Logger& logger_poller;

            /**
             * pointer to the selfHandle
             */
            std::shared_ptr<zmf::data::ModuleHandle> self;
            zmf::proto::SenderId selfSenderId;

            /**
             * pointer to the internal thread handling socket IO
             */
            std::unique_ptr<std::thread> poll_thread;

            /**
             * boolean flag indicating the current state of the service (started/stopped)
             */
            std::atomic_bool alive = ATOMIC_VAR_INIT(false);

            /**
             * ZMQ context
             */
            zmqpp::context_t context;

            /**
             * ZMQ publish socket
             */
            std::unique_ptr<zmqpp::socket> socket_pub;
            /**
             * ZMQ Subscribe socket
             */
            std::unique_ptr<zmqpp::socket> socket_sub;
            /**
             * ZMQ Request socket (router)
             */
            std::map<zmf::data::ModuleUniqueId, std::unique_ptr<zmqpp::socket>> map_sockets_req;
            /**
             * ZMQ Reply socket (router)
             */
            std::unique_ptr<zmqpp::socket> socket_rep;
            /**
             * ZMQ Push socket
             */
            std::unique_ptr<zmqpp::socket> socket_notify_send;
            /**
             * ZMQ Pull socket
             */
            std::unique_ptr<zmqpp::socket> socket_notify_recv;

            /**
             * ZMQ Poller
             */
            std::unique_ptr<zmqpp::poller> poller;

            /**
             * queue containing subscription changes
             */
            std::queue<std::pair<zmf::data::MessageType, bool>> queue_subscription_changes;
            /**
             * queue containing membership changes
             */
            std::queue<std::pair<std::string, bool>> queue_membership_changes;
            /**
             * map containing the promise of sent requests not yet answered by peer (or cancelled locally)
             */
            std::map<uint64_t, std::promise<zmf::data::ZmfMessage>> outstanding_requests;
            /**
             * map containing the reply-to address of requests not yet answered by own module
             */
            std::map<ExternalRequestIdentity, zmf::data::ModuleUniqueId> outstanding_replies;


            /**
             * next request id to use
             */
            uint64_t next_request_id = 0;

            /**
             * locks for the various maps/queues/..
             */
            std::mutex lock_notify_send;
            std::mutex lock_pub_socket;


            std::mutex lock_queue_subscription_changes;
            std::mutex lock_queue_membership_changes;

            std::mutex lock_map_replies;
            std::mutex lock_map_requests;
            std::mutex lock_map_sockets_req;


            std::mutex lock_ids;

            int32_t ZMF_ZMQ_ZMQ_RCVBUF = 0;
            int32_t ZMF_ZMQ_ZMQ_RCVHWM = 100000;
            int32_t ZMF_ZMQ_ZMQ_SNDBUF = 0;
            int32_t ZMF_ZMQ_ZMQ_SNDHWM = 100000;


            /**
             * Poller thread method: apply membership changes from queue
             */
            void applyMembershipChange();

            /**
             * Poller thread method: apply subscription changes from queue
             */
            void applySubscriptionChange();


            /**
             * Poller thread method: handle input on sub socket
             */
            void handleSubIn();

            /**
             * Poller thread method: handle input on notify socket
             */
            void handleNotify();

            /**
             * Poller thread method: handle input on rep socket
             */
            void handleRepIn();

            /**
             * util method to check if the alive flag is true
             */
            void checkAlive();

            /**
             * Poller thread method: main loop for the poller thread
             */
            void pollerLoop();

            /**
             * util method to close all zmq sockets
             */
            void closeSockets();

            /**
             * util method to put a notify msg in the notify queue
             */
            void notifyPoller(const NotifyType type);

            /**
             * util method to extract the port from the zmq socket option ĺast_endpoint
             */
            uint16_t extractPort(std::unique_ptr<zmqpp::socket>& socket) const;

            /**
             * util/debug method to print a zmq message
             */
            void printMessage(zmqpp::message& msg);

            void handleRequestReceived(zmqpp::message& message);

            void handleReplyReceived(zmqpp::message& message);

            void handleHelloReceived(zmqpp::message& message);

            void internalConnect(zmf::data::ModuleUniqueId identity, std::string rep_addr, std::string pub_addr);

            std::string getIp();
        };


    }
}

#endif //ZMF_ZMQ_SERVICE_H
