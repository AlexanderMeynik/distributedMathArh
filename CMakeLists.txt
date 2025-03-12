cmake_minimum_required(VERSION 3.22)
project(data_deduplication_service)

set(CMAKE_CXX_STANDARD 23)
SET(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -ffast-math -DNDEBUG -march=native -fpic -ftree-vectorize")
SET(CMAKE_CXX_FLAGS " -std=gnu++2b")





option(BUILD_TESTS "Build tests" ON)

option(BUILD_DOC "Build documentation" OFF)
if (BUILD_DOC)
    add_subdirectory(doc)
endif (BUILD_DOC)

add_subdirectory(utilLib)
add_subdirectory(computationalNode)


find_package(Drogon)
if(Drogon_FOUND)
    add_subdirectory(server)
else ()
    message("missing dependencies to build server")
endif()

find_package(Qt6)
find_package(Matplot++)
if(Qt6_FOUND AND Matplot++_FOUND)
    add_subdirectory(client)
else ()
    message("missing dependencies to build client")
endif()

#todo check that http server lib exists
#check that redis lib exists
if(Drogon_FOUND)
    add_subdirectory(mainNode)
else ()
    message("No cpprestFrameworkFound. mainNode subdirectory wont be built")
endif()