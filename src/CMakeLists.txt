cmake_minimum_required(VERSION 3.2)
project(zmf)

option(NoTests "NoTests" OFF)

include(${CMAKE_CURRENT_SOURCE_DIR}/../util/zmf-dir_cmake.txt)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../build)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -D_DEBUG")

include_directories("${ZSDN_DIR}/dependencies/include")
link_directories("${ZSDN_DIR}/dependencies/lib")

add_library(zmf SHARED
    AbstractModule.cpp
    AbstractModule.hpp
    config/ConfigurationProvider.cpp
    config/ConfigurationProvider.hpp
    config/IConfigurationProvider.hpp
    core/ZmfCore.cpp
    core/ZmfCore.hpp
    core/ModuleEventDispatcher.cpp
    core/ModuleEventDispatcher.hpp
    data/MessageType.hpp
    data/ModuleHandle.hpp
    data/ModuleUniqueId.hpp
    data/ZmfInReply.hpp
    data/ZmfMessage.hpp
    data/ZmfOutReply.hpp
    discovery/IPeerDiscoveryCoreInterface.hpp
    discovery/IPeerDiscoveryService.hpp
    discovery/PeerRegistry.hpp
    discovery_implementation/PeerDiscoveryService.cpp
    discovery_implementation/PeerDiscoveryService.hpp
    discovery_implementation/PeerDiscoveryService.hpp
    IZmfInstanceAccess.hpp
    IZmfInstanceController.hpp
    logging/ZmfLogging.cpp
    logging/ZmfLogging.hpp
    messaging/IZmfMessagingCancelRequestHandler.hpp
    messaging/IZmfMessagingCoreInterface.hpp
    messaging/IZmfMessagingSendReplyHandler.hpp
    messaging/IZmfMessagingService.hpp
    messaging_implementation/ZmqZmfMessagingServiceImplementation.cpp
    messaging_implementation/ZmqZmfMessagingServiceImplementation.hpp
    ZmfConsole.cpp
    ZmfConsole.hpp
    instance/ZmfInstance.cpp
    instance/ZmfInstance.hpp
    logging/ZmfLoggingContainer.hpp
    messaging_implementation/ExternalRequestIdentity.hpp
    discovery/PeerRegistryInternal.cpp 
    discovery/PeerRegistryInternal.hpp
    data/ModuleHandleInternal.hpp
    proto/FrameworkProto.pb.cc
    proto/FrameworkProto.pb.h
    util/NetworkUtil.cpp
    util/NetworkUtil.hpp 
    data/ModuleState.hpp)


target_link_libraries(zmf protobuf PocoFoundation PocoNet zmq zmqpp)

if(NOT NoTests)
    # Build unittest post-build
    message("### BUILDING UNITTESTS")
    add_custom_command(TARGET zmf POST_BUILD WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../test COMMAND cmake .)
    add_custom_command(TARGET zmf POST_BUILD WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../test COMMAND cmake --build .)
    # Execute unittests afterwards
    message("### RUNNING UNITTESTS")
    add_custom_command(TARGET zmf POST_BUILD WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../test COMMAND ./zmf_UT )
    message("### FINISHED RUN UNITTESTS")
else()
    message("### SKIPPING UNITTESTS")
endif()
