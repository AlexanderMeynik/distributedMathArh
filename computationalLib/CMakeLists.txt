if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()


if (BUILD_TESTS)
    add_subdirectory(test)
endif ()


set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/computationalLib/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/computationalLib/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/computationalLib/bin)

file(GLOB_RECURSE math_core_files CONFIGURE_DEPENDS
        "lib/computationalLib/math_core/*.cpp"
)

add_library(math_core_lib ${math_core_files})
target_link_libraries(math_core_lib PUBLIC allUtils OpenMP::OpenMP_CXX)
target_include_directories(math_core_lib PUBLIC include)
enable_coverage_for_target(math_core_lib)

file(GLOB_RECURSE interfaces_files CONFIGURE_DEPENDS
        "lib/computationalLib/interfaces/*.cpp"
)
add_library(interfaces_lib ${interfaces_files})
target_link_libraries(interfaces_lib PUBLIC math_core_lib)
enable_coverage_for_target(interfaces_lib)

add_executable(printerssst src/printersTests.cpp)
target_link_libraries(printerssst PUBLIC allUtils)
