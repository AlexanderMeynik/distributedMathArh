cmake_minimum_required(VERSION 3.26)
project(Magister1)

#add_subdirectory(Boost_tests)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE RelWithDebInfo)
set(CMAKE_CXX_FLAGS "-O0 -g")
#SET( CMAKE_CXX_FLAGS_DEBUG   "-O1 -g -march=native -fpic -ftree-vectorize")
set(CMAKE_CXX_FLAGS_DEBUG   "-O0 -g")
#set(CMAKE_CXX_FLAGS_DEBUG   "-O3 -march=native -fpic -ffast-math -ftree-vectorize")
SET( CMAKE_CXX_FLAGS_RELEASE    "-O3 -g -march=native -fpic -ffast-math -ftree-vectorize" )
#find_package(OpenXLSX)
find_package(Matplot++ REQUIRED)
#find_package(Matplot++ REQUIRED CONFIG)
find_library(PQXX_LIB pqxx)
find_library(PQ_LIB pq)
find_package(GSL REQUIRED)
find_package(OpenMP REQUIRED)
#add_executable(Magister1 main.cpp) #Dipoles.cpp)



# Add Executable
add_executable(Magister1  main.cpp)
add_executable(ConFReader configReader.cpp)
add_executable(testIntegr gausTets.cpp)


add_executable(clock parUtils/clockTest.cpp parUtils/OpenmpParralelClock.cpp)
target_link_libraries(clock PRIVATE OpenMP::OpenMP_CXX)

add_library(Clock parUtils/OpenmpParralelClock.cpp)
target_link_libraries(Clock PUBLIC OpenMP::OpenMP_CXX)

add_library(myLibrary math_core/Dipoles.cpp commonFunctions/lib.cpp math_core/MeshProcessor.h math_core/MeshProcessor.cpp printUtils.h math_core/TestRunner.cpp)
target_link_libraries(myLibrary PUBLIC ${GSL_LIBRARIES})
target_link_libraries(myLibrary PUBLIC ${PQXX_LIB} ${PQ_LIB})
target_include_directories(myLibrary PUBLIC ${GSL_INCLUDE_DIRS})
target_link_libraries(myLibrary PUBLIC Matplot++::matplot)
target_link_libraries(myLibrary PUBLIC OpenMP::OpenMP_CXX)




target_link_libraries(Magister1 PUBLIC myLibrary)


target_link_libraries(ConFReader PRIVATE ${GSL_LIBRARIES})
target_link_libraries(ConFReader PRIVATE ${PQXX_LIB} ${PQ_LIB})
target_include_directories(ConFReader PRIVATE ${GSL_INCLUDE_DIRS})


target_link_libraries(testIntegr PUBLIC myLibrary)


add_executable(moveDipoles moveDipoles.cpp)
target_link_libraries(moveDipoles PUBLIC myLibrary)

add_executable(generator coordGenerate.cpp)
target_link_libraries(generator PUBLIC myLibrary)


add_executable(checker eigenTest.cpp)
target_link_libraries(checker PUBLIC myLibrary)


add_executable(grids darwingGrids.cpp math_core/TestRunner.cpp)
target_link_libraries(grids PUBLIC myLibrary Clock)


