//
// Created by Jonas Grunert on 6/25/15.
//

#include "DemoModule.hpp"
#include <iostream>


DemoModule::DemoModule(uint64_t instanceId, uint16_t instanceVersion) :
        AbstractModule(zmf::data::ModuleUniqueId(MODULE_TYPE, instanceId), instanceVersion, "DemoModule",
                       std::vector<zmf::ModuleDependency>()) { }


bool DemoModule::enable() {
    std::cout << "Enabled DemoModule" << &std::endl;
    return true;
}

void DemoModule::disable() {
    std::cout << "Disabled DemoModule" << &std::endl;
}

void DemoModule::handleModuleStateChange(std::shared_ptr<zmf::data::ModuleHandle> changedModule,
                                         zmf::data::ModuleState newState, zmf::data::ModuleState lastState) {
    std::cout << this->getUniqueId().getString() << " registered Module " << changedModule->UniqueId.getString() <<
    "(" << changedModule->Name << ")" <<
    " state change from " << lastState << " to " << newState << &std::endl;
}


void DemoModule::printModulePeerRegistry() {

    std::cout << "-- Module registry of " << getUniqueId().TypeId << ":" << getUniqueId().InstanceId << &std::endl;
    getZmf()->getPeerRegistry()->printPeerRegistry();
}


void DemoModule::setAdditionalState(uint64_t state) {
    // TODO Better additional state infos
    std::vector<uint8_t> stateV;
    stateV.push_back(state);
    getZmf()->onModuleAdditionalStateChanged(stateV);
}


void DemoModule::testRequestReply(const zmf::data::ModuleUniqueId& target) {
    std::cout << "Sending request from " << getNameInstanceString() << " to " << target.getString() << &std::endl;

    // Send request
    zmf::data::ZmfInReply responseFut = getZmf()->sendRequest(target, zmf::data::ZmfMessage());

    // TODO Handle reply
    std::future_status status = responseFut.wait_for(std::chrono::milliseconds(1000));
    if (status == std::future_status::ready) {
        std::cout << "Received reply at " << getNameInstanceString() << " from " << target.getString() << &std::endl;
    } else {
        std::cerr << "Failed to get response" << &std::endl;
    }
}

zmf::data::ZmfOutReply DemoModule::handleRequest(const zmf::data::ZmfMessage& message,
                                                 const zmf::data::ModuleUniqueId& sender) {
    std::cout << "Repling at " << getNameInstanceString() << " to request from " << sender.getString() << &std::endl;
    return zmf::data::ZmfOutReply::createImmediateReply(
            zmf::data::ZmfMessage(zmf::data::MessageType("dank"), "response"));
}


void DemoModule::testHandleSub(const zmf::data::ZmfMessage& msg, const zmf::data::ModuleUniqueId& sender) {
    std::cout << "SubMsg received at " << getNameInstanceString() << &std::endl;
}

void DemoModule::testSubscribe(zmf::data::MessageType& subTopic) {

    //std::function <void(MessageType &, ZmfMessage &msg)>
    getZmf()->subscribe(subTopic, [this](const zmf::data::ZmfMessage& msg, const zmf::data::ModuleUniqueId& sender) {
        this->testHandleSub(msg, sender);
    });
}

void DemoModule::testPublish(zmf::data::ZmfMessage& msg) {
    getZmf()->publish(msg);
}
