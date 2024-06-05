project(Google_tests)

add_executable (Google_Tests_run test1.cpp)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

# adding the Google_Tests_run target
#add_executable(Google_Tests_run ConverterTests.cpp AbsoluteDateTest.cpp)

# linking Google_Tests_run with DateConverter_lib which will be tested
target_link_libraries(Google_Tests_run math_core_lib)

target_link_libraries(Google_Tests_run gtest gtest_main)