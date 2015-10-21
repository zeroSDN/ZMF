//
// Created by zsdn on 6/25/15.
//

#include <config/IConfigurationProvider.hpp>
#include <config/ConfigurationProvider.hpp>
#include <data/ModuleHandle.hpp>
#include <data/ModuleHandleInternal.hpp>
#include <ZmfConsole.hpp>
#include <Poco/Path.h>
#include <data/ZmfOutReply.hpp>
#include "zmfTests.h"
#include "DummyModule.h"
#include <instance/ZmfInstance.hpp>


//  override SetUp from TestFixture
void zmfTests::setUp() {

}

//  override tearDown from TestFixture
void zmfTests::tearDown() {

}


void zmfTests::testInstance() {

    auto moduleA = std::make_shared<DummyModule>(zmf::data::ModuleUniqueId(13, 37));
    auto moduleB = std::make_shared<DummyModule>(zmf::data::ModuleUniqueId(44, 20));

    auto coreA = zmf::instance::ZmfInstance::startInstance(moduleA, true, true, false, false, false);
    auto coreB = zmf::instance::ZmfInstance::startInstance(moduleB, true, true, false, false, false);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    moduleA->doPublish();
    moduleB->doPublish();

    moduleA->doRequest(moduleA->getUniqueId());
    moduleA->doRequest(moduleB->getUniqueId());

    moduleB->doRequest(moduleB->getUniqueId());
    moduleB->doRequest(moduleA->getUniqueId());

    std::this_thread::sleep_for(std::chrono::milliseconds(5000));

}


void zmfTests::testMessageTypeByteshift() {
    zmf::data::MessageType messageType;
    zmf::data::MessageType messageType2;
    // Max Values
    uint8_t int8 = 255;
    uint16_t int16 = 65535;
    uint32_t int32 = 4294967295;
    uint64_t int64 = 1844674407370955160;


    messageType.appendMatch8(int8);
    CPPUNIT_ASSERT(messageType.getMatchRaw()[0] == int8);
    messageType.appendMatch8(int8);
    CPPUNIT_ASSERT(messageType.getMatchRaw()[1] == int8);
    messageType.appendMatch16(int16);
    CPPUNIT_ASSERT(messageType.getMatchRaw()[2] == int8);
    CPPUNIT_ASSERT(messageType.getMatchRaw()[3] == int8);
    messageType.appendMatch32(int32);
    CPPUNIT_ASSERT(messageType.getMatchRaw()[4] == int8);
    CPPUNIT_ASSERT(messageType.getMatchRaw()[5] == int8);
    CPPUNIT_ASSERT(messageType.getMatchRaw()[6] == int8);
    CPPUNIT_ASSERT(messageType.getMatchRaw()[7] == int8);

}

void zmfTests::testMessageTypeConstructorOverride() {
    //  Start test == and !=
    {
        zmf::data::MessageType messageType;
        zmf::data::MessageType messageType2;

        CPPUNIT_ASSERT(messageType == messageType2);
        CPPUNIT_ASSERT(messageType2 == messageType);

        messageType.appendMatch8(123);
        CPPUNIT_ASSERT(messageType != messageType2);
        CPPUNIT_ASSERT(messageType2 != messageType);

        messageType2.appendMatch8(123);
        CPPUNIT_ASSERT(messageType == messageType2);
        CPPUNIT_ASSERT(messageType2 == messageType);

        //  different types by design, so it shouldnt be "equal"
        messageType.appendMatch8(12);
        messageType.appendMatch16(12);
        CPPUNIT_ASSERT(messageType != messageType2);
        CPPUNIT_ASSERT(messageType2 != messageType);

        messageType.appendMatch8(123);
        messageType.appendMatch8(12);
        CPPUNIT_ASSERT(messageType != messageType2);
        CPPUNIT_ASSERT(messageType2 != messageType);

    }
}

void zmfTests::testMessageTypeMethods() {

    //  Start test overridePrefixWith
    {
        zmf::data::MessageType messageType;
        zmf::data::MessageType messageType2;

        messageType.appendMatch8(123);
        messageType2.appendMatch8(1);
        messageType2.appendMatch16(2);
        messageType2.overridePrefixWith(messageType);
        CPPUNIT_ASSERT(messageType.containsTopic(messageType2));

        messageType.appendMatch16(999);
        messageType2.overridePrefixWith(messageType);
        CPPUNIT_ASSERT(messageType == messageType2);

        //messageType.appendMatch8(456);
        // catch int because methods just throw "1"
        //CPPUNIT_ASSERT_THROW(messageType2.overridePrefixWith(messageType), int);
    }
    //  End test overridePrefixWith


    //  Start test containsTopic
    {
        zmf::data::MessageType messageType;
        zmf::data::MessageType staticMessageType;

        //  both have no specified topic
        CPPUNIT_ASSERT(messageType.containsTopic(staticMessageType) == true);

        messageType.appendMatch8(123);
        staticMessageType.appendMatch8(123);
        //  both have same topic
        CPPUNIT_ASSERT(messageType.containsTopic(staticMessageType) == true);

        messageType.appendMatch16(1231);
        //  messageType has more exact topic
        CPPUNIT_ASSERT(messageType.containsTopic(staticMessageType) == false);

        staticMessageType.appendMatch16(1231);
        staticMessageType.appendMatch16(2525);
        //  messageType has less exact topic
        CPPUNIT_ASSERT(messageType.containsTopic(staticMessageType) == true);

    }
    //  End test containsTopic

}


void zmfTests::testZMFDataTypes() {
    //  Start test MessageType
    {
        zmf::data::MessageType messageType;
        uint8_t* dataRaw = new uint8_t[123];
        for (int i = 0; i < 123; i++) {
            dataRaw[i] = (uint8_t) i;
        }
        std::string data = std::string((char*) dataRaw, 123);

        zmf::data::ZmfMessage zmfMessage1;
        zmf::data::ZmfMessage zmfMessage2(messageType, data);

        CPPUNIT_ASSERT(zmfMessage2.getData() == data);
        //CPPUNIT_ASSERT(zmfMessage2.IsInvalid == false);
        CPPUNIT_ASSERT(zmfMessage2.getDataLength() == data.length());
        CPPUNIT_ASSERT(zmfMessage2.getType() == messageType);

        //CPPUNIT_ASSERT(zmfMessage1.IsInvalid == true);
        CPPUNIT_ASSERT(zmfMessage1.getDataLength() == 0);
        CPPUNIT_ASSERT(zmfMessage1.getType() == zmf::data::MessageType());

        delete[] dataRaw;
    }
    //  End test MessageType

    //  Start test MessageType
    {
        zmf::data::ZmfMessage zmfMessage1;
        //zmf::data::ZmfMessage zmfMessage2;
        std::shared_ptr<zmf::data::ZmfMessage> zmfMessageSharedPtr;
        std::promise<std::shared_ptr<zmf::data::ZmfMessage>> promise;
        //std::future<std::shared_ptr<zmf::data::ZmfMessage>> fut = promise.get_future();
        promise.set_value(zmfMessageSharedPtr);


        zmf::data::ZmfOutReply immediateResponse = zmf::data::ZmfOutReply::createImmediateReply(zmfMessage1);
        //zmf::ZmfOutReply* futureResponse = zmf::ZmfOutReply::createFutureResponse(&fut);

        CPPUNIT_ASSERT(immediateResponse.type == zmf::data::ZmfOutReply::ReplyType::IMMEDIATE_REPLY);
        CPPUNIT_ASSERT(immediateResponse.reply_immediate == zmfMessage1);
        //CPPUNIT_ASSERT(immediateResponse.FutureResponse == nullptr);

        // Sorry - NIY
        //CPPUNIT_ASSERT(futureResponse->IsImmediateResponse == zmf::ZmfOutReply::ResponseType::FUTURE_RESPONSE);
        //CPPUNIT_ASSERT(futureResponse->ImmediateResponse == nullptr);
        //CPPUNIT_ASSERT(futureResponse->FutureResponse == &fut);
    }
    //  End test MessageType

    //  Start test ModuleUniqueId
    {
        uint16_t typeID = 10;
        uint64_t instanceID = 100;

        //zmf::data::ModuleUniqueId moduleUniqueIdBlank;
        zmf::data::ModuleUniqueId moduleUniqueIdFilled(10, 100);

        CPPUNIT_ASSERT(moduleUniqueIdFilled.InstanceId == instanceID);
        CPPUNIT_ASSERT(moduleUniqueIdFilled.TypeId == typeID);


        std::string completeString = std::to_string(typeID) + std::string(":") + std::to_string(instanceID);
        CPPUNIT_ASSERT(moduleUniqueIdFilled.getString() == completeString);


        zmf::data::ModuleUniqueId moduleUniqueId(20, 20);
        zmf::data::ModuleUniqueId moduleUniqueId2(50, 50);
        zmf::data::ModuleUniqueId moduleUniqueId3(50, 50);

        zmf::data::ModuleUniqueId moduleUniqueId4(51, 51);
        zmf::data::ModuleUniqueId moduleUniqueId5(50, 51);
        zmf::data::ModuleUniqueId moduleUniqueId6(51, 50);


        CPPUNIT_ASSERT(moduleUniqueId < moduleUniqueId2);
        CPPUNIT_ASSERT(moduleUniqueId2 == moduleUniqueId3);
        CPPUNIT_ASSERT(!(moduleUniqueId2 < moduleUniqueId3));
        CPPUNIT_ASSERT(!(moduleUniqueId4 == moduleUniqueId6));
        CPPUNIT_ASSERT(!(moduleUniqueId4 < moduleUniqueId5));
        CPPUNIT_ASSERT(moduleUniqueId5 < moduleUniqueId4);
        CPPUNIT_ASSERT(moduleUniqueId6 < moduleUniqueId4);

    }
    //  End test ModuleUniqueId
}


void zmfTests::testConfig() {

    zmf::config::ConfigurationProvider config(Poco::Path::current() + "/myTestConfig.config");

    int intValue = 0;
    bool boolValue = false;
    double doubleValue = 0.0;
    std::string stringValue = "";

    bool boolPresent;
    bool intPresent;
    bool doublePresent;
    bool stringPresent;

    boolPresent = config.getAsBoolean("boolValue", boolValue);
    intPresent = config.getAsInt("intValue", intValue);
    doublePresent = config.getAsDouble("doubleValue", doubleValue);
    stringPresent = config.getAsString("stringValue", stringValue);

    CPPUNIT_ASSERT(boolPresent);
    CPPUNIT_ASSERT(intPresent);
    CPPUNIT_ASSERT(doublePresent);
    CPPUNIT_ASSERT(stringPresent);

    CPPUNIT_ASSERT(intValue == 1337);
    CPPUNIT_ASSERT(doubleValue == 4.20);
    CPPUNIT_ASSERT(stringValue == "dankMemes");
    CPPUNIT_ASSERT(boolValue);


}



