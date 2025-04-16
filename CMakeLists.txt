cmake_minimum_required(VERSION 3.22)
project(distributed_math_arh)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(Coverage)

set(CMAKE_CXX_STANDARD 23)

#SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -ffast-math -DNDEBUG -march=native -fpic -ftree-vectorize")
SET(CMAKE_CXX_FLAGS "-std=gnu++2b")

string(REGEX MATCH "^Debug" IS_DEBUG_BUILD "${CMAKE_BUILD_TYPE}")
if(IS_DEBUG_BUILD)
    message("Configuring debug build: ${CMAKE_BUILD_TYPE}")
    string(APPEND CMAKE_CXX_FLAGS " -g -O0 ")
endif ()

string(REGEX MATCH "^Release" IS_RELEASE_BUILD "${CMAKE_BUILD_TYPE}")
if(IS_RELEASE_BUILD)

    message(STATUS " Configuring release build: ${CMAKE_BUILD_TYPE}")
    string(APPEND CMAKE_CXX_FLAGS " -O2 -ffast-math -DNDEBUG -march=native -fpic -ftree-vectorize ")
endif ()

option(BUILD_TESTS "Build tests" ON)

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

find_package(Eigen3 3.3 REQUIRED NO_MODULE)
find_package(fmt REQUIRED)
find_package(jsoncpp REQUIRED)
find_package(OpenMP REQUIRED)
find_package(CURL REQUIRED)




add_subdirectory(utilLib)
add_subdirectory(computationalLib)

if(Drogon_FOUND AND amqpcpp_FOUND AND libpqxx_FOUND)
    add_subdirectory(computationalNode)
else ()
    message("Missing dependencies to build computationalNode.")
endif()


if(Qt6_FOUND AND Matplot++_FOUND)
    add_subdirectory(client)
else ()
    message("Missing dependencies to build client.")
endif()


if(Drogon_FOUND AND amqpcpp_FOUND AND libpqxx_FOUND)
    add_subdirectory(mainNode)
else ()
    message("Missing dependencies to build mainNode.")
endif()
