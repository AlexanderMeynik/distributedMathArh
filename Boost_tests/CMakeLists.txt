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

file(GLOB common CONFIGURE_DEPENDS
        "*common/*.h"
        "*common/*.cpp"
)
add_executable(commonTests ${common})
target_link_libraries (commonTests ${Boost_LIBRARIES})

file(GLOB parallelUtils CONFIGURE_DEPENDS
        "*parallelUtils/*.h"
        "*parallelUtils/*.cpp"
)

add_executable(parTests ${parallelUtils})
target_link_libraries (parTests  ${Boost_LIBRARIES})
target_link_libraries (parTests math_core_lib parallelLib)

add_executable (Boost_Tests_run test1.cpp common/PrintUtilsTest.cpp math_core/SpeedTests.cpp parallelUtils/ClockTests.cpp DipolesTests.cpp math_core/TestRunnerTests.cpp parallelUtils/ClockArrayTests.cpp)
target_link_libraries (Boost_Tests_run ${Boost_LIBRARIES})
target_link_libraries (Boost_Tests_run math_core_lib parallelLib)


