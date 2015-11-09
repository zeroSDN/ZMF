cmake_minimum_required(VERSION 3.2)


# Specify project
set(PROJECT_NAME demo-module)
project(${PROJECT_NAME})

# Set ZSDN root folder path relative to project (needed for module cmake files)
include(ZMF_DIR "${CMAKE_CURRENT_SOURCE_DIR}/zmf-dir_cmake.txt")

project(${PROJECT_NAME})

option(NoTests "NoTests" ON)
option(PiTarget "PiTarget" OFF)
option(Verbose "Verbose" OFF)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3 -Wl,--no-as-needed -pthread")
if(Verbose)
	message("# Verbose enabled")
  	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -v")
endif()


# Set module cmake files folder path
set(MODULE_CMAKE_FOLDER "${ZSDN_DIR_RELATIVE}/modules/cpp/cmake")


# Include from external file
include(${MODULE_CMAKE_FOLDER}/cmake_module_header.txt)






add_executable(${PROJECT_NAME} main.cpp
        ${ADDITIONAL_DEPENDENCIES}
        DemoModule.cpp
        DemoModule.hpp
        DemoModuleConsole.cpp
        DemoModuleConsole.hpp

        #../../modules/ExampleModuleA/ExampleModuleA.cpp
        #../../modules/ExampleModuleA/ExampleModuleA.hpp
        #../../modules/ExampleModuleB/ExampleModuleB.cpp
        #../../modules/ExampleModuleB/ExampleModuleB.hpp
)


# Link from external file
include(${MODULE_CMAKE_FOLDER}/cmake_module_link.txt)


# Custom from external file
include(${MODULE_CMAKE_FOLDER}/cmake_module_custom.txt)