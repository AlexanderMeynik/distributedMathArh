cmake_minimum_required(VERSION 3.26)
project(Magister1)



set(CMAKE_CXX_STANDARD 20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_BUILD_TYPE STREQUAL "Debug-Coverage")
    set(-DCMAKE_CXX_FLAGS="-g -fprofile-instr-generate -fcoverage-mapping --coverage")
    set(-DCMAKE_C_FLAGS="-g -fprofile-instr-generate -fcoverage-mapping --coverage")
else()
    set(CMAKE_BUILD_TYPE RelWithDebInfo)
    set(CMAKE_CXX_FLAGS "-O0 -g")
    set(CMAKE_CXX_FLAGS_DEBUG   "-O0 -g")
    SET( CMAKE_CXX_FLAGS_RELEASE    "-O3 -g -march=native -fpic -ffast-math -ftree-vectorize" )
endif()

find_package(Matplot++ REQUIRED)
find_library(PQXX_LIB pqxx)
find_library(PQ_LIB pq)
find_package(GSL REQUIRED)
find_package(OpenMP REQUIRED)




# Add Executable
add_executable(Magister1  main.cpp)
add_executable(ConFReader configReader.cpp)
add_executable(testIntegr gausTets.cpp)



file(GLOB common CONFIGURE_DEPENDS
        "*common/*.h"
        "*common/*.cpp"
)
add_library(commonLib ${common})

file(GLOB parallelUtils CONFIGURE_DEPENDS
        "*parallelUtils/*.h"
        "*parallelUtils/*.cpp"
)

add_library(parallelLib ${parallelUtils})
target_link_libraries(parallelLib PUBLIC OpenMP::OpenMP_CXX)


file(GLOB math_core_files CONFIGURE_DEPENDS
        "*math_core/*.h"
        "*math_core/*.cpp"
)

add_library(math_core_lib ${math_core_files})
target_link_libraries(math_core_lib PUBLIC ${GSL_LIBRARIES})
target_link_libraries(math_core_lib PUBLIC ${PQXX_LIB} ${PQ_LIB})#todo убрать
target_include_directories(math_core_lib PUBLIC ${GSL_INCLUDE_DIRS})
target_link_libraries(math_core_lib PUBLIC Matplot++::matplot)
target_link_libraries(math_core_lib PUBLIC commonLib parallelLib)


add_executable(clock clockTest.cpp)
target_link_libraries(clock parallelLib)







target_link_libraries(Magister1 PUBLIC math_core_lib)


target_link_libraries(ConFReader PRIVATE ${GSL_LIBRARIES})
target_link_libraries(ConFReader PRIVATE ${PQXX_LIB} ${PQ_LIB})
target_include_directories(ConFReader PRIVATE ${GSL_INCLUDE_DIRS})


target_link_libraries(testIntegr PUBLIC math_core_lib)


add_executable(moveDipoles moveDipoles.cpp)
target_link_libraries(moveDipoles PUBLIC math_core_lib)

add_executable(generator coordGenerate.cpp)
target_link_libraries(generator PUBLIC math_core_lib)


add_executable(checker eigenTest.cpp)
target_link_libraries(checker PUBLIC math_core_lib)


add_executable(grids darwingGrids.cpp math_core/TestRunner.cpp)
target_link_libraries(grids PUBLIC math_core_lib parallelLib)


