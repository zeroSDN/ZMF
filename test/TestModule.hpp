//
// Created by zsdn on 8/7/15.
//

#ifndef ZMF_UT_TESTMODULE_HPP
#define ZMF_UT_TESTMODULE_HPP

#include <AbstractModule.hpp>
#include <ZmfInstance.hpp>
#include "zmfInteroptTest.h"


class TestModule : public zmf::AbstractModule {

public:
    TestModule(const zmf::data::ModuleUniqueId& uniqueId, uint16_t version, const std::string& name,
               const std::vector<zmf::ModuleDependency>& dependencies) : AbstractModule(uniqueId, version, name,
                                                                                        dependencies) { }

    virtual bool enable() {
        std::cout << "enable" << std::endl;
        doSub();
        return true;
    }

    virtual void disable() {
        std::cout << "disable" << std::endl;
    }

    void onSub(const zmf::data::ZmfMessage& msg, const zmf::data::ModuleUniqueId& sender) {
        std::cout << "sub received: " << msg.getType().getMatch() << " | " << msg.getData() << " | from " <<
        sender.getString() << std::endl;

    }

    void doSub() {
        getZmf()->subscribe(zmf::data::MessageType("ayy"), std::bind(&TestModule::onSub, this, std::placeholders::_1,
                                                                     std::placeholders::_2));
    }

    void doPub() {
        getZmf()->publish(zmf::data::ZmfMessage("ayyy", "ebin"));
    }

    void doReq() {
        auto handle = getZmf()->getPeerRegistry()->getAnyPeerWithType(13, true);
        if (handle != nullptr) {
            zmf::data::ZmfMessage request = zmf::data::ZmfMessage("simbly", "ebin");
            zmf::data::ZmfMessage reply = getZmf()->sendRequest(handle->UniqueId, request).get();
            std::cout << "received reply! " << reply.getType().getMatch() << " | " << reply.getData() << std::endl;
        }
    }


    virtual zmf::data::ZmfOutReply handleRequest(const zmf::data::ZmfMessage& msg,
                                                 const zmf::data::ModuleUniqueId& sender) override {

        return zmf::data::ZmfOutReply::createImmediateReply(msg);
    }
};

#endif //ZMF_UT_TESTMODULE_HPP
