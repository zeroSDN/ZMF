/**
 * @author Jonas Grunert
 * @author Jan Strauß
 * @author Andre Kutzleb
* @date created on 7/7/15.
 */
#include <instance/ZmfInstance.hpp>
#include "../config/ConfigurationProvider.hpp"
#include "../discovery/IPeerDiscoveryService.hpp"
#include "../discovery_implementation/PeerDiscoveryService.hpp"
#include "../messaging_implementation/ZmqZmfMessagingServiceImplementation.hpp"
#include "../core/ZmfCore.hpp"

std::shared_ptr<zmf::IZmfInstanceController> zmf::instance::ZmfInstance::startInstance(
        std::shared_ptr<zmf::AbstractModule> instanceModule,
        bool trackModuleStates,
        bool moduleAutoEnable,
        bool exitWhenEnableFail,
        bool peerDiscoveryWait,
        bool disableEqualModuleInterconnect,
        const std::string& cfgPath) {

    if (trackModuleStates) {
        std::cout << "Warning: deprecated flag trackModuleStates set" << &std::endl;
    }


    zmf::logging::ZmfLogging::initializeLogging(instanceModule->getModuleName(), cfgPath);

    auto configProvicer = std::make_shared<zmf::config::ConfigurationProvider>(std::string(cfgPath));
    auto peerDiscoveryServ = std::make_shared<zmf::discovery::PeerDiscoveryService>(instanceModule->getUniqueId());
    auto zmqServ = std::make_shared<zmf::messaging::ZmqZmfMessagingServiceImplementation>(
            instanceModule->getUniqueId());

    // Create core
    auto core = std::make_shared<zmf::core::ZmfCore>(configProvicer, instanceModule, peerDiscoveryServ, zmqServ);

    // Try to start core
    if (!core->startInstance(moduleAutoEnable, exitWhenEnableFail, peerDiscoveryWait, disableEqualModuleInterconnect)) {
        // Cancel when start failed
        std::cerr << "Failed to start ZMF instance - canceling start" << &std::endl;
        throw Poco::Exception("Failed to start ZMF instance");
    }

    // std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    // Return interface reference to core
    return core;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

std::shared_ptr<zmf::IZmfInstanceController> zmf::instance::ZmfInstance::startInstance(
        std::shared_ptr<zmf::AbstractModule> instanceModule,
        std::vector<StartOption> startOptions,
        const std::string& cfgPath) {

    bool moduleAutoEnable =
            startOptions.end() == std::find(startOptions.begin(), startOptions.end(), StartOption::NO_AUTO_ENABLE);
    bool exitWhenEnableFail = startOptions.end() == std::find(startOptions.begin(), startOptions.end(),
                                                              StartOption::NO_EXIT_WHEN_ENABLE_FAILED);

    bool peerDiscoveryWait = startOptions.end() ==
                             std::find(startOptions.begin(), startOptions.end(), StartOption::NO_PEER_DISCOVERY_WAIT);

    bool disableEqualModuleInterconnect = startOptions.end() != std::find(startOptions.begin(), startOptions.end(),
                                                                          StartOption::NO_EQUAL_MODULE_INTERCONNECT);


    std::cout << "startInstance: flags=" << false << moduleAutoEnable << exitWhenEnableFail << peerDiscoveryWait <<
    disableEqualModuleInterconnect << " cfg-path=" << cfgPath << std::endl;

    return startInstance(instanceModule, false, moduleAutoEnable, exitWhenEnableFail, peerDiscoveryWait,
                         disableEqualModuleInterconnect, cfgPath);
}

#pragma GCC diagnostic pop