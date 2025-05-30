if (BUILD_TESTS)
    add_subdirectory(test)
endif ()


set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/computationalNode/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/computationalNode/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/computationalNode/bin)

file(COPY config
        DESTINATION ${CMAKE_BINARY_DIR}/computationalNode/bin)


file(GLOB_RECURSE httpSources CONFIGURE_DEPENDS
        "src/controller/*.cpp"
        "src/service/*.cpp"
)


add_library(computationalNodeLib ${httpSources})
target_link_libraries(computationalNodeLib PUBLIC math_core_lib Drogon::Drogon network_shared_lib)
target_include_directories(computationalNodeLib PUBLIC include)
enable_coverage_for_target(computationalNodeLib)

add_executable(compNode main.cpp)
target_link_libraries(compNode)
target_link_libraries(compNode -lssl -lcrypto)
target_link_libraries(compNode computationalNodeLib allUtils)
enable_coverage_for_target(compNode)



