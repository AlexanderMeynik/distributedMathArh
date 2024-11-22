
include_directories(../include)
file(GLOB common CONFIGURE_DEPENDS
        "*common/*.h"
        "*common/*.cpp"
)
#todo tests for all other components
message(${common})
add_executable(common_Tests ${common})
target_link_libraries(common_Tests gtest gtest_main)


file(GLOB parallelUtils CONFIGURE_DEPENDS
        "parallelUtils/*.cpp"
)

add_executable(par_Tests ${parallelUtils})
target_link_libraries(par_Tests gtest gmock)
target_link_libraries(par_Tests math_core_lib parallelLib)
