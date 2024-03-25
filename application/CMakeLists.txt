cmake_minimum_required(VERSION 3.26)
project(Magister1)

#add_subdirectory(Boost_tests)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
#set(CMAKE_BUILD_TYPE RelWithDebInfo)
#set(CMAKE_CXX_FLAGS "-O1 -g")
#SET( CMAKE_CXX_FLAGS_DEBUG   "-O1 -g -march=native -fpic -ftree-vectorize")

set(CMAKE_CXX_FLAGS_DEBUG   "-O3 -march=native -fpic -ffast-math -ftree-vectorize")
SET( CMAKE_CXX_FLAGS_RELEASE    "-O3 -march=native -fpic -ftree-vectorize" )
#find_package(OpenXLSX)
find_package(Matplot++ REQUIRED)
#find_package(Matplot++ REQUIRED CONFIG)
find_library(PQXX_LIB pqxx)
find_library(PQ_LIB pq)
find_package(GSL REQUIRED)
#add_executable(Magister1 main.cpp) #Dipoles.cpp)



# Add Executable
add_executable(Magister1 Dipoles.h main.cpp lib.cpp)
add_executable(Magister2 Dipoles2.h main.cpp)
add_executable(ConFReader configReader.cpp)
add_executable(testIntegr gausTets.cpp)
# Link Libraries


# Include Directories
target_link_libraries(Magister1 PRIVATE ${GSL_LIBRARIES})
target_link_libraries(Magister1 PRIVATE ${PQXX_LIB} ${PQ_LIB})
target_include_directories(Magister1 PRIVATE ${GSL_INCLUDE_DIRS})
target_link_libraries(Magister1 PUBLIC Matplot++::matplot)


target_link_libraries(ConFReader PRIVATE ${GSL_LIBRARIES})
target_link_libraries(ConFReader PRIVATE ${PQXX_LIB} ${PQ_LIB})
target_include_directories(ConFReader PRIVATE ${GSL_INCLUDE_DIRS})


target_link_libraries(testIntegr PUBLIC Matplot++::matplot)
target_include_directories(testIntegr PRIVATE ${GSL_INCLUDE_DIRS})
target_link_libraries(testIntegr PRIVATE ${GSL_LIBRARIES})
target_link_libraries(testIntegr PRIVATE ${PQXX_LIB} ${PQ_LIB})


# Include Directories
add_library(myLibrary Dipoles.cpp lib.cpp)