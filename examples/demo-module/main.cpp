#include <iostream>

#include <memory>
#include <Poco/Exception.h>
#include <zmf/AbstractModule.hpp>
//#include <ZmfMock.hpp>
#include <zmf/ZmfConsole.hpp>
#include "DemoModule.hpp"
#include "DemoModuleConsole.hpp"

// TODO Comments

/**
 * Runs a single ZMF instance and a console to control it
 */
void main_Zmf_Single_Console() {
    // TODO
}


/**
 * Runs a single ZMF Mock instance and without console
 */
void main_ZmfMock_Single_NoConsole() {
    // Create module
    std::shared_ptr<zmf::AbstractModule> module = std::shared_ptr<zmf::AbstractModule>(new DemoModule(0, 0));
    // Create and start ZMF instance with module
    //std::shared_ptr<zmf::IZmfInstanceController> zmfInstance = zmf_mock::ZmfMock::startInstance(module, true, true,
    //                                                                                            false);
    // Wait until intstance stopped
    //zmfInstance->joinExecution();
}

/**
 * Runs a single ZMF Mock instance and without console
 */
void main_ZmfMock_Single_Console() {
    // Create module
    std::shared_ptr<zmf::AbstractModule> module = std::shared_ptr<zmf::AbstractModule>(new DemoModule(0, 0));
    // Create and start ZMF instance with module
    //std::shared_ptr<zmf::IZmfInstanceController> zmfInstance = zmf_mock::ZmfMock::startInstance(module, true, true,
    //                                                                                            false);
    // Start console
    //zmf::ZmfConsole console(zmfInstance);
    //console.startConsole();
}

/**
 * Runs a DemoModuleConsole that allows multiple modules
 */
void main_ZmfMock_Multi_Console() {
    DemoModuleConsole console;
    console.startConsole();
}


int main() {

    try {
        main_ZmfMock_Multi_Console();
    }
    catch (Poco::Exception exc) { // Catch Exceptions
        std::cerr << "Failed to run module: " << exc.message() << &std::endl;
        return 1;
    }
    catch (...) {  // Catch all
        std::cerr << "Failed to run module: Unknown reason" << &std::endl;
        return 1;
    }

    return 0;
}