cmake_minimum_required(VERSION 3.2)


# Specify project
set(PROJECT_NAME demo-module)
project(${PROJECT_NAME})


# Set ZSDN root folder path relative to project (needed for module cmake files)
set(ZSDN_DIR_RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/../../..")
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