# ┌──────────────────────────────────────────────────────────────────┐
#   Hello, World!
#   This is reverse engineering tool inspired by Cheat Engine.
#   Feel free to contribute.
# └──────────────────────────────────────────────────────────────────┘
# todo[critical]: CMakeLists.txt: find and set lowest possible cmake version, compare CMP's and write missing functions
cmake_minimum_required(VERSION 3.11...3.14.3)

include(${CMAKE_SOURCE_DIR}/cmake/properties.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/common.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/dependencies.cmake)
#include(${CMAKE_SOURCE_DIR}/cmake/PVS-Studio.cmake) # TODO[critical]: CMakeLists.txt: what if PVS-Studio.lic not exists?
include(${CMAKE_SOURCE_DIR}/cmake/versioning.cmake)


# ┌──────────────────────────────────────────────────────────────────┐
#   Main project
# └──────────────────────────────────────────────────────────────────┘
project(pwner VERSION 0.1.0 LANGUAGES CXX)
version_print(${CMAKE_PROJECT_NAME})


# ┌──────────────────────────────────────────────────────────────────┐
#   Settings
# └──────────────────────────────────────────────────────────────────┘
# Tip: clear CMake's cache to apply new default option
option(PWN_RELEASE              "Release" OFF) # FIXME[high]: CMakeLists.txt: to remove? после освоения CPack
option(PWN_TEST                 "Build tests for library" ON)
option(PWN_BINDINGS             "Build bindings" ON)
option(PWN_BINDINGS_PYBIND11    "Build pybind11 bindings" ON)
option(PWN_TOOL                 "Build tools" ON)
option(PWN_TOOL_FAKEMEM         "Build FAKEMEM" ON)
option(PWN_TOOL_FAKEGAME        "Build FAKEGAME tutorial" ON)


# ┌──────────────────────────────────────────────────────────────────┐
#   Project structure
# └──────────────────────────────────────────────────────────────────┘
add_subdirectory(lib)
add_subdirectory(tools)
add_subdirectory(test)

# TODO[critical]: add .idea/misc.xml to git to mark folders as source root directories!!!
