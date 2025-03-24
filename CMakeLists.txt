cmake_minimum_required(VERSION 3.22)
project(data_deduplication_service)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(Coverage)

set(CMAKE_CXX_STANDARD 23)
SET(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -ffast-math -DNDEBUG -march=native -fpic -ftree-vectorize")
SET(CMAKE_CXX_FLAGS " -std=gnu++2b")

find_package(Drogon)
find_package(Qt6)
find_package(Matplot++)
find_package(jsoncpp)
find_package(Eigen3 3.3 REQUIRED NO_MODULE)
find_package(fmt REQUIRED)

option(BUILD_TESTS "Build tests" ON)

option(BUILD_DOC "Build documentation" OFF)

if (BUILD_DOC)
    add_subdirectory(doc)
endif (BUILD_DOC)

add_subdirectory(utilLib)
add_subdirectory(computationalLib)

if(Drogon_FOUND)
    add_subdirectory(computationalNode)
else ()
    message("missing dependencies to build server")
endif()


if(Qt6_FOUND AND Matplot++_FOUND)
    add_subdirectory(client)
else ()
    message("missing dependencies to build client")
endif()


if(Drogon_FOUND)
    add_subdirectory(mainNode)
else ()
    message("No cpprestFrameworkFound. mainNode subdirectory wont be built")
endif()