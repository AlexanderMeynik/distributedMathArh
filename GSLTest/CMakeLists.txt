cmake_minimum_required(VERSION 3.26)
project(Magister1)

#add_subdirectory(Boost_tests)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE RelWithDebInfo)
set(CMAKE_CXX_FLAGS "-O0 -g")

set(CMAKE_CXX_FLAGS_DEBUG   "-O0 -g")

SET( CMAKE_CXX_FLAGS_RELEASE    "-O3 -g -march=native -fpic -ffast-math -ftree-vectorize" )

find_package(Matplot++ REQUIRED)

find_library(PQXX_LIB pqxx)
find_library(PQ_LIB pq)
find_package(GSL REQUIRED)
find_package(OpenMP REQUIRED)




add_executable(gsl1 main.cpp)
target_link_libraries(gsl1 GSL::gsl GSL::gslcblas)
