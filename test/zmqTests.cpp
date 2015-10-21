#include "zmqTests.h"
#include <data/ModuleHandleInternal.hpp>
#include <config/ConfigurationProvider.hpp>


int receivedSub = 0;
bool receivedReq1 = false;
bool receivedReq2 = false;

int perf_receivedSub = 0;

class ZmqCoreDummy : public zmf::messaging::IZmfMessagingCoreInterface {

private:
    zmf::messaging::ZmqZmfMessagingServiceImplementation* service;

public:


    ZmqCoreDummy(zmf::messaging::ZmqZmfMessagingServiceImplementation* service) : service(service) {
    }

    virtual void onSubMsgReceived(const zmf::data::ZmfMessage& message, const zmf::data::ModuleUniqueId& sender) {
        receivedSub++;
        std::cout << ">>>>> received sub msg: topic:" << message.getType().getMatch() << " | content: " <<
        message.getData() << std::endl;
    }

    virtual void onRequestMsgReceived(const zmf::messaging::ExternalRequestIdentity id,
                                      const zmf::data::ZmfMessage& message,
                                      const zmf::data::ModuleUniqueId& sender) {

        std::cout << ">>>>> received req msg on topic: " << message.getType().getMatch();

        if (message.getType().getMatch() == "TESTTYPE") {
            std::cout << ": " << message.getData() << std::endl;

            service->sendReply(id, zmf::data::ZmfMessage(message.getType(),
                                                         "RARE PEPE RARE PEPE"));
            receivedReq1 = true;
        }
        else if (message.getType().getMatch() == "RMS") {
            std::cout << ": {BINARY} length: " << message.getData().size() << std::endl;
            service->sendReply(id, message);
            receivedReq2 = true;
        }
    }
};

class ZmqCorePerfDummy : public zmf::messaging::IZmfMessagingCoreInterface {

private:
    zmf::messaging::ZmqZmfMessagingServiceImplementation* service;

public:


    ZmqCorePerfDummy(zmf::messaging::ZmqZmfMessagingServiceImplementation* service) : service(service) {
    }

    virtual void onSubMsgReceived(const zmf::data::ZmfMessage& message, const zmf::data::ModuleUniqueId& sender) {
        perf_receivedSub++;
    }

    virtual void onRequestMsgReceived(const zmf::messaging::ExternalRequestIdentity id,
                                      const zmf::data::ZmfMessage& message,
                                      const zmf::data::ModuleUniqueId& sender) {
        service->sendReply(id, message);
    }
};


std::string loadTestData() {
    std::stringstream dataStrm;
    dataStrm << std::ifstream("test_img.jpg", std::ios::binary).rdbuf();
    return dataStrm.str();
}


void zmqTests::testZmq() {
    std::cout << "Start testZmq" << &std::endl;
    auto x = zmf::data::ModuleUniqueId(1, 33);
    zmf::messaging::ZmqZmfMessagingServiceImplementation service(x);

    zmf::messaging::IZmfMessagingCoreInterface* dank = new ZmqCoreDummy(&service);

    std::shared_ptr<zmf::data::ModuleHandle> meme = std::shared_ptr<zmf::data::ModuleHandle>(
            new zmf::data::ModuleHandleInternal(x, 7, "meme", true));

    std::shared_ptr<zmf::config::IConfigurationProvider> config = std::shared_ptr<zmf::config::IConfigurationProvider>(
            new zmf::config::ConfigurationProvider());

    CPPUNIT_ASSERT(service.start(dank, meme, config));

    std::cout << "PUB=" << static_cast<zmf::data::ModuleHandleInternal*>(meme.get())->self_ZmqPubPort << " | REP=" <<
    static_cast<zmf::data::ModuleHandleInternal*>(meme.get())->self_ZmqRepPort << std::endl;

    service.peerJoin(meme);

    zmf::data::MessageType match("aa");

    service.subscribe(match);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    const std::string data2(loadTestData());

    service.publish(zmf::data::ZmfMessage(zmf::data::MessageType("aaa"), "event aaa"));
    service.publish(zmf::data::ZmfMessage(zmf::data::MessageType("aab"), "event aab"));
    service.publish(zmf::data::ZmfMessage(zmf::data::MessageType("a"), "event a"));
    service.publish(zmf::data::ZmfMessage(zmf::data::MessageType("b"), "event b"));
    service.publish(zmf::data::ZmfMessage(zmf::data::MessageType("aa"), "event aa"));
    service.publish(zmf::data::ZmfMessage(zmf::data::MessageType(""), "event "));

    auto q = service.sendRequest(meme->UniqueId,
                                 zmf::data::ZmfMessage(zmf::data::MessageType("TESTTYPE"), "IS THIS ARRIVING?"));
    auto w = service.sendRequest(meme->UniqueId,
                                 zmf::data::ZmfMessage((zmf::data::MessageType("RMS")), data2));
    auto e = service.sendRequest(meme->UniqueId, zmf::data::ZmfMessage((zmf::data::MessageType("NULL")), ""));


    bool data_eq = w.get().getData() == data2;


    std::cout << "REPLY ON KIZ REQUEST= " << q.get().getData() << std::endl;
    std::cout << "REPLY ON RMS REQUEST= " << data_eq << std::endl;


    std::future_status res = e.wait_for(std::chrono::milliseconds(250));

    CPPUNIT_ASSERT(res == std::future_status::timeout);
    e.cancelRequest();

    service.stop();

    CPPUNIT_ASSERT(data_eq);
    CPPUNIT_ASSERT_EQUAL(receivedSub, 3);
    CPPUNIT_ASSERT(receivedReq1);
    CPPUNIT_ASSERT(receivedReq2);

    assert(service.start(dank, meme, config));

    service.stop();

    delete dank;
    std::cout << "End testZmq" << &std::endl;
}


void zmqTests::perfTestReqRep() {
    std::cout << "Start perfTestReqRep" << &std::endl;
    auto x = zmf::data::ModuleUniqueId(1, 33);
    auto y = zmf::data::ModuleUniqueId(4, 20);

    zmf::messaging::ZmqZmfMessagingServiceImplementation serviceA(x);
    zmf::messaging::ZmqZmfMessagingServiceImplementation serviceB(y);

    zmf::messaging::IZmfMessagingCoreInterface* dankA = new ZmqCorePerfDummy(&serviceA);
    zmf::messaging::IZmfMessagingCoreInterface* dankB = new ZmqCorePerfDummy(&serviceB);

    std::shared_ptr<zmf::data::ModuleHandle> memeA = std::shared_ptr<zmf::data::ModuleHandle>(
            new zmf::data::ModuleHandleInternal(x, 7, "memeA", true));
    std::shared_ptr<zmf::data::ModuleHandle> memeB = std::shared_ptr<zmf::data::ModuleHandle>(
            new zmf::data::ModuleHandleInternal(y, 7, "memeB", true));


    std::shared_ptr<zmf::config::IConfigurationProvider> config = std::shared_ptr<zmf::config::IConfigurationProvider>(
            new zmf::config::ConfigurationProvider());

    serviceA.start(dankA, memeA, config);
    serviceB.start(dankB, memeB, config);

    serviceA.peerJoin(memeA);
    serviceA.peerJoin(memeB);

    serviceB.peerJoin(memeA);
    serviceB.peerJoin(memeB);

    uint NUM_REQ = 100000;

    std::vector<zmf::data::ZmfInReply> replies;
    replies.reserve(NUM_REQ);

    std::chrono::time_point<std::chrono::steady_clock> start, end_send, end_received;

    start = std::chrono::steady_clock::now();

    for (uint i = 0; i < NUM_REQ; ++i) {
        replies.push_back(serviceA.sendRequest(memeB->UniqueId, zmf::data::ZmfMessage(zmf::data::MessageType("perf"),
                                                                                      std::to_string(i))));
    }

    end_send = std::chrono::steady_clock::now();

    for (uint i = 0; i < NUM_REQ; ++i) {
        replies.at(i).get();
    }

    end_received = std::chrono::steady_clock::now();

    auto dur_send = std::chrono::duration_cast<std::chrono::milliseconds>(end_send - start).count();
    auto dur_recv = std::chrono::duration_cast<std::chrono::milliseconds>(end_received - start).count();

    std::cout << dur_send << " | " << dur_recv << " " << (NUM_REQ / dur_recv) << std::endl;

    serviceA.stop();
    serviceB.stop();
    std::cout << "End perfTestReqRep" << &std::endl;
}


void zmqTests::perfTestPubSub() {
    std::cout << "Start perfTestPubSub" << &std::endl;
    auto x = zmf::data::ModuleUniqueId(1, 33);
    // auto y = zmf::data::ModuleUniqueId(4, 20);

    zmf::messaging::ZmqZmfMessagingServiceImplementation serviceA(x);
    // zmf::messaging::ZmqZmfMessagingServiceImplementation serviceB(y);

    zmf::messaging::IZmfMessagingCoreInterface* dankA = new ZmqCorePerfDummy(&serviceA);
    // zmf::messaging::IZmfMessagingCoreInterface* dankB = new ZmqCorePerfDummy(&serviceB);

    std::shared_ptr<zmf::data::ModuleHandle> memeA = std::shared_ptr<zmf::data::ModuleHandle>(
            new zmf::data::ModuleHandleInternal(x, 7, "memeA", true));
    //std::shared_ptr<zmf::data::ModuleHandle> memeB = std::shared_ptr<zmf::data::ModuleHandle>(
    //         new zmf::data::ModuleHandleInternal(y, 7));

    std::shared_ptr<zmf::config::IConfigurationProvider> config = std::shared_ptr<zmf::config::IConfigurationProvider>(
            new zmf::config::ConfigurationProvider());

    serviceA.start(dankA, memeA, config);
    //serviceB.start(dankB, memeB, config);

    serviceA.peerJoin(memeA);
    // serviceA.peerJoin(memeB);

    //serviceB.peerJoin(memeA);
    //serviceB.peerJoin(memeB);

    serviceA.subscribe(zmf::data::MessageType(""));
    std::this_thread::sleep_for(std::chrono::seconds(1));

    perfRun(serviceA);
    perfRun(serviceA);
    perfRun(serviceA);
    perfRun(serviceA);
    perfRun(serviceA);
    perfRun(serviceA);

    //std::this_thread::sleep_for(std::chrono::seconds(5));

    serviceA.stop();
    //serviceB.stop();
    std::cout << "End perfTestPubSub" << &std::endl;
}


void zmqTests::perfRun(zmf::messaging::ZmqZmfMessagingServiceImplementation& serviceA) const {
    std::cout << "Start perfRun" << &std::endl;
    uint NUM_EVENTS = 1000000;
    perf_receivedSub = 0;

    std::chrono::time_point<std::chrono::_V2::steady_clock> start, end_send, end_received;

    start = std::chrono::_V2::steady_clock::now();

    auto msg = zmf::data::ZmfMessage(zmf::data::MessageType("perf"), "wasd");
    for (int i = 0; i < NUM_EVENTS; ++i) {
        serviceA.publish(msg);
    }

    end_send = std::chrono::_V2::steady_clock::now();

    while (perf_receivedSub < NUM_EVENTS) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    end_received = std::chrono::_V2::steady_clock::now();

    auto dur_send = std::chrono::duration_cast<std::chrono::milliseconds>(end_send - start).count();
    auto dur_recv = std::chrono::duration_cast<std::chrono::milliseconds>(end_received - start).count();

    std::cout << dur_send << " | " << dur_recv << " " << (NUM_EVENTS / dur_recv) << std::endl;

    try {
        serviceA.sendRequest(zmf::data::ModuleUniqueId(12, 33), msg).get();
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }
    std::cout << "End perfRun" << &std::endl;
}


void zmqTests::testUnsubBug() {
    std::cout << "Start testUnsubBug" << &std::endl;
    zmf::data::ModuleUniqueId x = zmf::data::ModuleUniqueId(1, 33);
    // auto y = zmf::data::ModuleUniqueId(4, 20);

    zmf::messaging::ZmqZmfMessagingServiceImplementation serviceA(x);
    // zmf::messaging::ZmqZmfMessagingServiceImplementation serviceB(y);

    zmf::messaging::IZmfMessagingCoreInterface* dankA = new ZmqCorePerfDummy(&serviceA);
    // zmf::messaging::IZmfMessagingCoreInterface* dankB = new ZmqCorePerfDummy(&serviceB);

    std::shared_ptr<zmf::data::ModuleHandle> memeA = std::shared_ptr<zmf::data::ModuleHandle>(
            new zmf::data::ModuleHandleInternal(x, 7, "memeA", true));
    //std::shared_ptr<zmf::data::ModuleHandle> memeB = std::shared_ptr<zmf::data::ModuleHandle>(
    //         new zmf::data::ModuleHandleInternal(y, 7));

    std::shared_ptr<zmf::config::IConfigurationProvider> config = std::shared_ptr<zmf::config::IConfigurationProvider>(
            new zmf::config::ConfigurationProvider());

    serviceA.start(dankA, memeA, config);
    //serviceB.start(dankB, memeB, config);

    serviceA.peerJoin(memeA);


    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    serviceA.peerLeave(memeA);

    std::this_thread::sleep_for(std::chrono::milliseconds(5000));

    serviceA.stop();

    std::cout << "End testUnsubBug" << &std::endl;

    delete dankA;
}
