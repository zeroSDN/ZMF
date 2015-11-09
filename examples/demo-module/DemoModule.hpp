//
// Created by Jonas Grunert on 6/25/15.
//

#ifndef DEMO_MODULE_DEMOMODULE_H
#define DEMO_MODULE_DEMOMODULE_H

#include <zmf/AbstractModule.hpp>


class DemoModule : public zmf::AbstractModule {

public:
    DemoModule(uint64_t instanceId, uint16_t instanceVersion);


    virtual void handleModuleStateChange(std::shared_ptr<zmf::data::ModuleHandle> changedModule,
                                         zmf::data::ModuleState newState, zmf::data::ModuleState lastState);


    virtual zmf::data::ZmfOutReply handleRequest(const zmf::data::ZmfMessage& message,
                                                 const zmf::data::ModuleUniqueId& sender) override;

    // Testing
    void printModulePeerRegistry();

    void setAdditionalState(uint64_t state);

    void testRequestReply(const zmf::data::ModuleUniqueId& target);

    // TODO Const?
    void testSubscribe(zmf::data::MessageType& subTopic);

    void testPublish(zmf::data::ZmfMessage& msg);

    void testHandleSub(const zmf::data::ZmfMessage& msg, const zmf::data::ModuleUniqueId& sender);


protected:
    virtual bool enable();

    virtual void disable();


private:
    //static const uint16_t MODULE_TYPE = 0xfff1;
    static const uint16_t MODULE_TYPE = 1;
};


#endif //DEMO_MODULE_DEMOMODULE_H
