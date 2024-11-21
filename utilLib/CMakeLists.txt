cmake_minimum_required(VERSION 3.26)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

SET( CMAKE_CXX_FLAGS_DEBUG  "-g")
SET( CMAKE_CXX_FLAGS_RELEASE  "-O3 -ffast-math -DNDEBUG -march=native -fpic -ftree-vectorize")
SET( CMAKE_CXX_FLAGS  " -std=gnu++2b")

project(utilLib)

find_package(libpqxx REQUIRED)
find_package(glog 0.7.1 REQUIRED)
find_package(OpenSSL REQUIRED)

include_directories(include)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/utilLib/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/utilLib/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/utilLib/bin)

#todo copy include

add_library(commonLibrary lib/myConcepts.cpp lib/clockArray.cpp lib/HashUtils.cpp)
target_link_libraries(commonLibrary glog::glog OpenSSL::SSL -lcrypto)
target_include_directories(commonLibrary PUBLIC include)

option(BUILD_DOC "Build documentation" OFF)
if(BUILD_DOC)
    add_subdirectory(doc)
endif (BUILD_DOC)

install(TARGETS commonLibrary
LIBRARY DESTINATION lib
ARCHIVE DESTINATION lib
INCLUDES DESTINATION INCLUDE)