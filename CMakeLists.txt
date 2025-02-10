cmake_minimum_required(VERSION 3.22)
project(data_deduplication_service)

set(CMAKE_CXX_STANDARD 23)
SET(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -ffast-math -DNDEBUG -march=native -fpic -ftree-vectorize")
SET(CMAKE_CXX_FLAGS " -std=gnu++2b")

find_package(cpprestsdk)
if(cpprestsdk_FOUND)
    add_subdirectory(server)
endif()



option(BUILD_TESTS "Build tests" ON)

option(BUILD_DOC "Build documentation" OFF)
if (BUILD_DOC)
    add_subdirectory(doc)
endif (BUILD_DOC)

add_subdirectory(utilLib)
add_subdirectory(computationalNode)