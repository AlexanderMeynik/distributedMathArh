project(Google_tests)
#todo remake
#todo fix debug coverage
set(CMAKE_CXX_STANDARD 20)
if (CMAKE_BUILD_TYPE STREQUAL "Debug-Coverage")
    set(-DCMAKE_CXX_FLAGS="-g -fprofile-instr-generate -fcoverage-mapping --coverage")
    set(-DCMAKE_C_FLAGS="-g -fprofile-instr-generate -fcoverage-mapping --coverage")
else ()
    SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -g -march=native -fpic -ffast-math -ftree-vectorize")
endif ()
function(target_link_libraries_with_coverage targetName library)
    target_link_libraries(${targetName} ${library})
    set_target_properties(${library} PROPERTIES COMPILE_FLAGS "-fprofile-arcs -ftest-coverage")
endfunction()


include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})


add_executable(Google_Tests_run test1.cpp)
target_link_libraries(Google_Tests_run math_core_lib)

target_link_libraries(Google_Tests_run gtest gtest_main)


file(GLOB common CONFIGURE_DEPENDS
        "*common/*.h"
        "*common/*.cpp"
)
add_executable(common_Tests ${common})
target_link_libraries(common_Tests gtest gtest_main)

file(GLOB parallelUtils CONFIGURE_DEPENDS
        "*parallelUtils/*.h"
        "*parallelUtils/*.cpp"
)

add_executable(par_Tests ${parallelUtils})
target_link_libraries(par_Tests gtest gmock)
target_link_libraries(par_Tests math_core_lib parallelLib)


file(GLOB interfaces CONFIGURE_DEPENDS
        "*interfaces/*.h"
        "*interfaces/*.cpp"
)

add_executable(inter_Tests ${interfaces})
target_link_libraries(inter_Tests gtest gmock)
target_link_libraries(inter_Tests interfaces_lib)

file(GLOB core CONFIGURE_DEPENDS
        "*math_core/*.h"
        "*math_core/*.cpp"
)

add_executable(core_Tests ${core})
target_link_libraries(core_Tests gtest gmock)
target_link_libraries(core_Tests interfaces_lib)

file(GLOB resourse CONFIGURE_DEPENDS
        "res/*.*"
)
file(COPY res DESTINATION ${CMAKE_BINARY_DIR})