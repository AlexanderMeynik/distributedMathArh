#todo remake
#todo fix debug coverage
set(CMAKE_CXX_STANDARD 20)


include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})



file(GLOB interfaces CONFIGURE_DEPENDS
        "interfaces/*.h"
        "interfaces/*.cpp"
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
target_link_libraries(core_Tests math_core_lib io_lib)


add_executable(benchmark benchmark.cpp)
target_link_libraries(benchmark math_core_lib io_lib)#todo fix interfaces

