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
#SET( CMAKE_CXX_FLAGS_RELEASE    "-O3 -g -march=native -fpic -ftree-vectorize" )
#find_package(OpenXLSX)
find_package(Matplot++ REQUIRED)
#find_package(Matplot++ REQUIRED CONFIG)
find_library(PQXX_LIB pqxx)
find_library(PQ_LIB pq)
find_package(GSL REQUIRED)
find_package(OpenMP REQUIRED)
#add_executable(Magister1 main.cpp) #Dipoles.cpp)



# Add Executable
add_executable(Magister1  main.cpp) #lib.cpp printUtils.h MeshProcessor.cpp)
add_executable(Magister2 Dipoles2.h main.cpp)
add_executable(ConFReader configReader.cpp)
add_executable(testIntegr gausTets.cpp)
# Link Libraries

add_library(myLibrary Dipoles.cpp lib.cpp MeshProcessor.h MeshProcessor.cpp printUtils.h)
# Include Directories
target_link_libraries(Magister1 PRIVATE ${GSL_LIBRARIES})
target_link_libraries(Magister1 PRIVATE ${PQXX_LIB} ${PQ_LIB})
target_include_directories(Magister1 PRIVATE ${GSL_INCLUDE_DIRS})
target_link_libraries(Magister1 PUBLIC Matplot++::matplot)
target_link_libraries(Magister1 PUBLIC myLibrary)


target_link_libraries(ConFReader PRIVATE ${GSL_LIBRARIES})
target_link_libraries(ConFReader PRIVATE ${PQXX_LIB} ${PQ_LIB})
target_include_directories(ConFReader PRIVATE ${GSL_INCLUDE_DIRS})


target_link_libraries(testIntegr PUBLIC Matplot++::matplot)
target_include_directories(testIntegr PRIVATE ${GSL_INCLUDE_DIRS})
target_link_libraries(testIntegr PRIVATE ${GSL_LIBRARIES})
target_link_libraries(testIntegr PRIVATE ${PQXX_LIB} ${PQ_LIB})


# Include Directories



add_executable(moveDipoles moveDipoles.cpp)
target_link_libraries(moveDipoles PUBLIC Matplot++::matplot)
target_link_libraries(moveDipoles PUBLIC myLibrary)
target_include_directories(moveDipoles PRIVATE ${GSL_INCLUDE_DIRS})
target_link_libraries(moveDipoles PRIVATE ${GSL_LIBRARIES})
target_link_libraries(moveDipoles PRIVATE ${PQXX_LIB} ${PQ_LIB})

add_executable(generator coordGenerate.cpp)
target_link_libraries(generator PUBLIC myLibrary)
target_include_directories(generator PRIVATE ${GSL_INCLUDE_DIRS})
target_link_libraries(generator PRIVATE ${GSL_LIBRARIES})
target_link_libraries(generator PRIVATE Matplot++::matplot)

target_link_libraries(generator PRIVATE OpenMP::OpenMP_CXX)


add_executable(checker eigenTest.cpp)
target_link_libraries(checker PUBLIC myLibrary)
target_link_libraries(checker PRIVATE Matplot++::matplot)
target_include_directories(checker PRIVATE ${GSL_INCLUDE_DIRS})
target_link_libraries(checker PRIVATE ${GSL_LIBRARIES})

target_link_libraries(checker PRIVATE OpenMP::OpenMP_CXX)


add_executable(grids darwingGrids.cpp)
target_link_libraries(grids PUBLIC myLibrary)
target_link_libraries(grids PRIVATE Matplot++::matplot)
target_include_directories(grids PRIVATE ${GSL_INCLUDE_DIRS})
target_link_libraries(grids PRIVATE ${GSL_LIBRARIES})

target_link_libraries(grids PRIVATE OpenMP::OpenMP_CXX)