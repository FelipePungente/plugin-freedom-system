cmake_minimum_required(VERSION 3.22)

# Set deployment target before project()
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13" CACHE STRING "Minimum macOS deployment version")

# Universal Binary: arm64 + x86_64
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "macOS target architectures")

project(JUCEPlugins VERSION 1.0.0)

# JUCE: use local install when available, otherwise fetch from GitHub
if(EXISTS "/Applications/JUCE/CMakeLists.txt")
    add_subdirectory(/Applications/JUCE JUCE)
else()
    include(FetchContent)
    FetchContent_Declare(JUCE
        GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
        GIT_TAG        8.0.7
        GIT_SHALLOW    TRUE
    )
    FetchContent_MakeAvailable(JUCE)
endif()

# Auto-discover plugins
file(GLOB PLUGIN_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/plugins/*")
foreach(PLUGIN_DIR ${PLUGIN_DIRS})
    if(IS_DIRECTORY ${PLUGIN_DIR} AND EXISTS "${PLUGIN_DIR}/CMakeLists.txt")
        add_subdirectory(${PLUGIN_DIR})
    endif()
endforeach()
