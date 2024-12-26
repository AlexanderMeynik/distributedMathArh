set(CMAKE_CXX_STANDARD 23)

find_package(OpenMP)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})




file(GLOB core CONFIGURE_DEPENDS
        "*math_core/*.h"
        "*math_core/*.cpp"
)

add_executable(core_Tests ${core})
target_link_libraries(core_Tests gtest gmock)
target_link_libraries(core_Tests io_lib)#todo what components of iolib we actually use here
target_compile_options(core_Tests PUBLIC -coverage)
target_link_options(core_Tests PUBLIC -coverage -fprofile-arcs -ftest-coverage)

add_executable(benchmark benchmark.cpp fileHandler.cpp benchmarkHandler.cpp)
target_link_libraries(benchmark  io_lib)#todo smae as prev

if(OpenMP_CXX_FOUND)
    target_link_libraries(benchmark OpenMP::OpenMP_CXX)
endif()


add_executable(samplePerExample simplePerExample.cpp)
target_link_libraries(samplePerExample math_core_lib io_lib)
#todo https://discourse.cmake.org/t/guideline-for-code-coverage/167
#todo https://danielsieger.com/blog/2024/08/03/code-coverage-with-cmake.html
