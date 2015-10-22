//
// Created by zsdn on 7/27/15.
//

#include <config/ConfigurationProvider.hpp>
#include <data/ModuleHandleInternal.hpp>
#include <ZmfInstance.hpp>
#include <core/ZmfCore.hpp>
#include "zmfMessagingTests.h"

class MsgServiceDummy : public zmf::messaging::IZmfMessagingService {

private:
    Poco::Logger& logger;
public:
    MsgServiceDummy() : logger(zmf::logging::ZmfLogging::getLogger("MsgServiceDummy")) { }

    bool start(zmf::messaging::IZmfMessagingCoreInterface* const corePtr,
               std::shared_ptr<zmf::data::ModuleHandle> selfHandle,
               std::shared_ptr<zmf::config::IConfigurationProvider> cfg) {
        logger.information("start()");
        return true;
    }

    void stop() {
        logger.information("stop()");
    }

    void peerJoin(std::shared_ptr<zmf::data::ModuleHandle> module) {
        logger.information("peerJoin()" + module->UniqueId.getString());
    }

    void peerLeave(std::shared_ptr<zmf::data::ModuleHandle> module) {
        logger.information("peerLeave()");
    }

    void subscribe(const zmf::data::MessageType& topic) {
        logger.information("subscribe()");
    }

    void unsubscribe(const zmf::data::MessageType& topic) {
        logger.information("unsubscribe()");
    }

    void publish(const zmf::data::ZmfMessage& msg) {
        logger.information("publish()");
    }

    zmf::data::ZmfInReply sendRequest(const zmf::data::ModuleUniqueId& target,
                                      const zmf::data::ZmfMessage& msg) {
        logger.information("sendRequest()");
        return zmf::data::ZmfInReply(0, std::future<zmf::data::ZmfMessage>(), this);
    }

    void cancelRequest(uint64_t requestID, bool manual) {
        logger.information("cancelRequest()");
    }

    void sendReply(zmf::messaging::ExternalRequestIdentity requestID, const zmf::data::ZmfMessage& reply) {
        logger.information("sendReply()");
    }

    void onDisable() {
        logger.information("onDisable()");
    }
};

class ModuleDummy : public zmf::AbstractModule {
public:

    ModuleDummy() : AbstractModule(zmf::data::ModuleUniqueId(0, 0), 14, "DummyModule",
                                   std::vector<zmf::ModuleDependency>()) { }


    virtual zmf::data::ZmfOutReply handleRequest(const zmf::data::ZmfMessage& message,
                                                 const zmf::data::ModuleUniqueId& sender) {
        getLogger().information("handleRequest()");
        return zmf::data::ZmfOutReply::createNoReply();
    }

    virtual bool enable() {
        getLogger().information("enable()");
        return true;
    }

    virtual void disable() {
        getLogger().information("disable()");
    }


};

void zmfMessagingTests::onSub(const zmf::data::ZmfMessage& msg, const zmf::data::ModuleUniqueId& sender) {
    std::cout << "onSub(" << msg.getType().getMatch() << ":" << msg.getData() << ", " << sender.getString() << ")" <<
    std::endl;
    {
        std::unique_lock<std::mutex> lk(m);
        subReceived = true;
    }
    cv.notify_one();
}

void zmfMessagingTests::testMessaging() {

    std::shared_ptr<zmf::messaging::IZmfMessagingService> msgBus = std::shared_ptr<zmf::messaging::IZmfMessagingService>(
            new MsgServiceDummy());
    std::shared_ptr<zmf::AbstractModule> module = std::shared_ptr<zmf::AbstractModule>(new ModuleDummy());
    std::shared_ptr<zmf::data::ModuleHandle> handle = std::shared_ptr<zmf::data::ModuleHandle>(
            new zmf::data::ModuleHandleInternal(zmf::data::ModuleUniqueId(1, 33), 7, "test1", true));

    std::shared_ptr<zmf::config::IConfigurationProvider> config = std::shared_ptr<zmf::config::IConfigurationProvider>(
            new zmf::config::ConfigurationProvider());
    zmf::core::ModuleEventDispatcher zmfMessaging(handle->UniqueId, msgBus, config, nullptr);


    std::shared_ptr<zmf::data::ModuleHandle> qqq = std::shared_ptr<zmf::data::ModuleHandle>(
            new zmf::data::ModuleHandleInternal(zmf::data::ModuleUniqueId(4, 20), 1, "test2", false));

    //zmf::core::ZmfCore* dummyCore = new ;
    //std::shared_ptr<zmf::IZmfInstanceController> modHandle = zmf::instance::ZmfInstance::startInstance(module,
    //                                                                                                   false,
    //                                                                                                   false,
    //                                                                                                   true).get();
    bool en = module->INTERNAL_internalEnable(nullptr);

    std::cout << en << std::endl;

    zmfMessaging.start(module, handle, config);
    zmfMessaging.onEnable();

    zmfMessaging.onPeerChange(qqq, zmf::data::ModuleState::Active, zmf::data::ModuleState::Dead);

    auto x = zmfMessaging.subscribe(zmf::data::MessageType("ay"),
                                    std::bind(&zmfMessagingTests::onSub, this, std::placeholders::_1,
                                              std::placeholders::_2));

    zmfMessaging.onRequestMsgReceived(zmf::messaging::ExternalRequestIdentity(handle->UniqueId, 12),
                                      zmf::data::ZmfMessage(zmf::data::MessageType("top"), "kek"),
                                      handle->UniqueId);

    zmfMessaging.onSubMsgReceived(zmf::data::ZmfMessage(zmf::data::MessageType("ayyy"), "lmao"), handle->UniqueId);

    std::cout << "pre_mutex" << std::endl;
    {
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, [=] { return subReceived; });
    }
    std::cout << "post_mutex" << std::endl;

    x.unsubscribe();


    //qqq->CurrentState = zmf::data::ModuleState::Dead;
    //handle->CurrentState = zmf::data::ModuleState::Dead;

    zmfMessaging.onPeerChange(qqq, zmf::data::ModuleState::Active, zmf::data::ModuleState::Dead);
    zmfMessaging.onPeerChange(handle, zmf::data::ModuleState::Active, zmf::data::ModuleState::Dead);


    zmfMessaging.onDisable();
    zmfMessaging.stop();

    module->INTERNAL_internalDisable();
}


