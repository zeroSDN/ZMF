cmake_minimum_required(VERSION 3.2)
project(zmf)

option(NoTests "NoTests" OFF)

include(${CMAKE_CURRENT_SOURCE_DIR}/util/zmf-dir_cmake.txt)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${ZMF_DIR}/build)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -D_DEBUG")

include_directories("${ZMF_DIR}/dependencies/include")
include_directories("${ZMF_DIR}/include/zmf")
link_directories("${ZMF_DIR}/dependencies/lib")

FILE(GLOB INCLUDE_DIR RELATIVE ${ZMF_DIR}
        include/zmf/*.*)

add_library(zmf SHARED
    ${INCLUDE_DIR}
    src/AbstractModule.cpp
    src/config/ConfigurationProvider.cpp
    src/config/ConfigurationProvider.hpp
    src/core/ZmfCore.cpp
    src/core/ZmfCore.hpp
    src/core/ModuleEventDispatcher.cpp
    src/core/ModuleEventDispatcher.hpp
    src/discovery/IPeerDiscoveryCoreInterface.hpp
    src/discovery/IPeerDiscoveryService.hpp
    src/discovery_implementation/PeerDiscoveryService.cpp
    src/discovery_implementation/PeerDiscoveryService.hpp
    src/discovery_implementation/PeerDiscoveryService.hpp
    src/logging/ZmfLogging.cpp
    src/messaging/IZmfMessagingCoreInterface.hpp
    src/messaging/IZmfMessagingService.hpp
    src/messaging_implementation/ZmqZmfMessagingServiceImplementation.cpp
    src/messaging_implementation/ZmqZmfMessagingServiceImplementation.hpp
    src/ZmfConsole.cpp
    src/instance/ZmfInstance.cpp
    src/discovery/PeerRegistryInternal.cpp 
    src/discovery/PeerRegistryInternal.hpp
    src/data/ModuleHandleInternal.hpp
    src/proto/FrameworkProto.pb.cc
    src/proto/FrameworkProto.pb.h
    src/util/NetworkUtil.cpp)


target_link_libraries(zmf protobuf PocoFoundation PocoNet zmq zmqpp)

if(NOT NoTests)
    # Build unittest post-build
    message("### BUILDING UNITTESTS")
    add_custom_command(TARGET zmf POST_BUILD WORKING_DIRECTORY ${ZMF_DIR}/test COMMAND cmake .)
    add_custom_command(TARGET zmf POST_BUILD WORKING_DIRECTORY ${ZMF_DIR}/test COMMAND cmake --build .)
    # Execute unittests afterwards
    message("### RUNNING UNITTESTS")
    add_custom_command(TARGET zmf POST_BUILD WORKING_DIRECTORY ${ZMF_DIR}/test COMMAND ./zmf_UT )
    message("### FINISHED RUN UNITTESTS")
else()
    message("### SKIPPING UNITTESTS")
endif()
