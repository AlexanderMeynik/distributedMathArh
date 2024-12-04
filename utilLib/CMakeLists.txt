cmake_minimum_required(VERSION 3.26)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

SET(CMAKE_CXX_FLAGS_DEBUG "-g")
SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -ffast-math -DNDEBUG -march=native -fpic -ftree-vectorize")
SET(CMAKE_CXX_FLAGS " -std=gnu++2b")

project(utilLib)
add_subdirectory(test)

find_package(Eigen3 3.3 REQUIRED NO_MODULE)

include_directories(include)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/utilLib/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/utilLib/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/utilLib/bin)


file(GLOB common CONFIGURE_DEPENDS
        "lib/common/*.cpp"
)
add_library(commonLib ${common})
target_link_libraries(commonLib PUBLIC Eigen3::Eigen)

file(GLOB_RECURSE parallelUtils CONFIGURE_DEPENDS
        "lib/parallelUtils/*.cpp"
)

add_library(parallelLib ${parallelUtils})


add_library(commonLibrary lib/parallelUtils/commonDeclarations.cpp lib/parallelUtils/clockArray.cpp)
target_include_directories(commonLibrary PUBLIC include)

option(BUILD_DOC "Build documentation" OFF)
if (BUILD_DOC)
    add_subdirectory(doc)
endif (BUILD_DOC)

install(TARGETS commonLibrary
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        INCLUDES DESTINATION INCLUDE)