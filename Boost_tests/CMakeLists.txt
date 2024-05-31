set (Boost_USE_STATIC_LIBS OFF)
SET( CMAKE_CXX_FLAGS_RELEASE    "-O3 -g -march=native -fpic -ffast-math -ftree-vectorize" )
find_package (Boost REQUIRED COMPONENTS unit_test_framework)
include_directories (${Boost_INCLUDE_DIRS})

add_executable (Boost_Tests_run test1.cpp PrintUtilsTest.cpp SpeedTests.cpp ClockTests.cpp DipolesTests.cpp TestRunnerTests.cpp)
target_link_libraries (Boost_Tests_run ${Boost_LIBRARIES})
target_link_libraries (Boost_Tests_run math_core_lib parallelLib)


