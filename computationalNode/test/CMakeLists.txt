set(CMAKE_CXX_STANDARD 23)

find_package(OpenMP)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})





add_executable(core_Tests math_core/smokeTests.cpp math_core/dipoleCommonTests.cpp ../../utilLib/test/plotUtils/meshProcessorTests.cpp)
target_link_libraries(core_Tests gtest gtest_main)
target_link_libraries(core_Tests io_lib plotUtilsLib)
target_compile_options(core_Tests PUBLIC -coverage)
target_link_options(core_Tests PUBLIC -coverage -fprofile-arcs -ftest-coverage)



add_executable(benchmark benchmark.cpp fileHandler.cpp benchmarkHandler.cpp)
target_link_libraries(benchmark  io_lib plotUtilsLib)#todo smae as prev

if(OpenMP_CXX_FOUND)
    target_link_libraries(benchmark OpenMP::OpenMP_CXX )
endif()


add_executable(samplePerExample simplePerExample.cpp)
target_link_libraries(samplePerExample math_core_lib io_lib plotUtilsLib)

#todo https://discourse.cmake.org/t/guideline-for-code-coverage/167
#todo https://danielsieger.com/blog/2024/08/03/code-coverage-with-cmake.html
