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
target_include_directories(math_core_lib PUBLIC include/computationalLib)
enable_coverage_for_target(math_core_lib)



file(GLOB_RECURSE network_shared_lib_f CONFIGURE_DEPENDS
        "lib/computationalLib/network_shared/*.cpp"
)

add_library(network_shared_lib ${network_shared_lib_f})
target_include_directories(network_shared_lib PUBLIC include/computationalLib)
target_include_directories(network_shared_lib PUBLIC ../utilLib/include)
target_link_libraries(network_shared_lib PUBLIC fmt::fmt amqpcpp jsoncpp_lib CURL::libcurl)
enable_coverage_for_target(network_shared_lib)

