set(CMAKE_CXX_STANDARD 23)

find_package(OpenMP)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})


add_executable(core_Tests math_core/smokeTests.cpp math_core/dipoleCommonTests.cpp ../../utilLib/test/plotUtils/meshProcessorTests.cpp)
target_link_libraries(core_Tests gtest gtest_main)
target_link_libraries(core_Tests math_core_lib commonLib)
enable_coverage_for_target(core_Tests)


add_executable(benchmark benchmark.cpp fileHandler.cpp benchmarkHandler.cpp)#this one can be stored separately from all packages
target_link_libraries(benchmark math_core_lib commonLib)

if (OpenMP_CXX_FOUND)
    target_link_libraries(benchmark OpenMP::OpenMP_CXX)
endif ()

