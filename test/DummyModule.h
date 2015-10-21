//
// Created by zsdn on 8/6/15.
//

#ifndef ZMF_UT_DUMMYMODULE_H
#define ZMF_UT_DUMMYMODULE_H

#include <AbstractModule.hpp>

/**
 * @brief Simple test module class for testing
 * @author Jan Strauß
 * @date created on 7/9/15.
*/
class DummyModule : public zmf::AbstractModule {

    zmf::data::ModuleUniqueId id;
public:

    DummyModule(const zmf::data::ModuleUniqueId& id) : AbstractModule(id, 420, "_______DummyModule",
                                                                      std::vector<zmf::ModuleDependency>()) { }


    bool enable() {
        getLogger().information("enable");
        getZmf()->subscribe(zmf::data::MessageType("k"), [=](const zmf::data::ZmfMessage& event,
                                                             const zmf::data::ModuleUniqueId& sender) {
            getLogger().information("received sub from " + sender.getString());
        });
        return true;
    }

    void doPublish() {
        getZmf()->publish(zmf::data::ZmfMessage(zmf::data::MessageType("kek"), "hoho haha"));
    }

    void doRequest(zmf::data::ModuleUniqueId target) {
        getZmf()->sendRequest(target, zmf::data::ZmfMessage(zmf::data::MessageType("kek"), "hoho haha")).get();
        getLogger().information("finished request");
    }

    void disable() {
        getLogger().information("disable");
    }

    zmf::data::ZmfOutReply handleRequest(const zmf::data::ZmfMessage& message,
                                         const zmf::data::ModuleUniqueId& sender) {
        getLogger().information("handleRequest");
        return zmf::data::ZmfOutReply::createImmediateReply(message);
    }
};

#endif //ZMF_UT_DUMMYMODULE_H
