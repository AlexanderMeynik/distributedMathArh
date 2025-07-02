cmake_minimum_required(VERSION 3.22)
project(distributed_math_arh)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(utilities)

set(CMAKE_CXX_STANDARD 23)
SET(CMAKE_CXX_FLAGS "-std=gnu++2b")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()


string(REGEX MATCH "^Debug" IS_DEBUG_BUILD "${CMAKE_BUILD_TYPE}")
if (IS_DEBUG_BUILD)
    message("Configuring debug build: ${CMAKE_BUILD_TYPE}")
    string(APPEND CMAKE_CXX_FLAGS " -g -O0 ")
endif ()

string(REGEX MATCH "^Release" IS_RELEASE_BUILD "${CMAKE_BUILD_TYPE}")
if (IS_RELEASE_BUILD)
    message(STATUS " Configuring release build: ${CMAKE_BUILD_TYPE}")
    string(APPEND CMAKE_CXX_FLAGS " -O2 -ffast-math -DNDEBUG -march=native -fpic -ftree-vectorize ")
endif ()

option(BUILD_TESTS "Build tests" OFF)

if (BUILD_TESTS)
    enable_testing()
    include(GoogleTest)
    include(CTest)
endif (BUILD_TESTS)
option(BUILD_DOC "Build documentation" OFF)


if (BUILD_DOC)
    add_subdirectory(doc)
endif (BUILD_DOC)


find_package(Drogon)
find_package(Qt6)
find_package(Matplot++)
find_package(amqpcpp)
find_package(libpqxx)
find_package(Boost)
find_package(Eigen3 3.3 NO_MODULE)
find_package(fmt)
find_package(jsoncpp)
find_package(OpenMP)
find_package(CURL)
find_package(scn CONFIG)
find_package(cpptrace)

set(UTIL_LIB_DEPS Eigen3 jsoncpp fmt scn cpptrace)
set(COMPUTATIONAL_LIB_DEPS OpenMP amqpcpp CURL libpqxx)
set(COMPUTATIONAL_NODE_DEPS Drogon)
set(CLIENT_DEPS Qt6 Matplot++)
set(MAIN_NODE_DEPS Drogon)


set(UTIL_LIB_DIRS)
set(COMPUTATIONAL_LIB_DIRS utilLib)
set(COMPUTATIONAL_NODE_DIRS utilLib computationalLib)
set(CLIENT_DIRS utilLib)
set(MAIN_NODE_DIRS utilLib computationalLib)


add_subdirectory_with_deps(utilLib UTIL_LIB_DEPS UTIL_LIB_DIRS)
add_subdirectory_with_deps(computationalLib COMPUTATIONAL_LIB_DEPS COMPUTATIONAL_LIB_DIRS)
add_subdirectory_with_deps(computationalNode COMPUTATIONAL_NODE_DEPS COMPUTATIONAL_NODE_DIRS)
add_subdirectory_with_deps(client CLIENT_DEPS CLIENT_DIRS)
add_subdirectory_with_deps(mainNode MAIN_NODE_DEPS MAIN_NODE_DIRS)



