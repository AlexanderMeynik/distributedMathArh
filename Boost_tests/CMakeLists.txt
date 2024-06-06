set(CMAKE_CXX_STANDARD 20)
if(CMAKE_BUILD_TYPE STREQUAL "Debug-Coverage")
    set(-DCMAKE_CXX_FLAGS="-g -fprofile-instr-generate -fcoverage-mapping --coverage")
    set(-DCMAKE_C_FLAGS="-g -fprofile-instr-generate -fcoverage-mapping --coverage")
else()
    SET( CMAKE_CXX_FLAGS_RELEASE    "-O3 -g -march=native -fpic -ffast-math -ftree-vectorize" )
endif()
function(target_link_libraries_with_coverage targetName library)
    target_link_libraries(${targetName} ${library})
    set_target_properties(${library} PROPERTIES COMPILE_FLAGS "-fprofile-arcs -ftest-coverage")
endfunction()

set (Boost_USE_STATIC_LIBS OFF)

find_package (Boost REQUIRED COMPONENTS unit_test_framework)
include_directories (${Boost_INCLUDE_DIRS})



add_executable (Boost_Tests_run test1.cpp PrintUtilsTest.cpp SpeedTests.cpp ClockTests.cpp DipolesTests.cpp TestRunnerTests.cpp ClockArrayTests.cpp)
target_link_libraries (Boost_Tests_run ${Boost_LIBRARIES})
target_link_libraries (Boost_Tests_run math_core_lib parallelLib)


