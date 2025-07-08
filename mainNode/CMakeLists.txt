



include_directories(include)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/mainNode/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/mainNode/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/mainNode/bin)


file(COPY config
        DESTINATION ${CMAKE_BINARY_DIR}/mainNode/bin)


file(GLOB_RECURSE httpSources CONFIGURE_DEPENDS
        "src/controller/*.cpp"
        "src/service/*.cpp"
)


add_library(mainNodeLib ${httpSources})
target_link_libraries(mainNodeLib PUBLIC math_core_lib network_shared_lib Drogon::Drogon)
target_include_directories(mainNodeLib PUBLIC include)
enable_coverage_for_target(mainNodeLib)

add_executable(mainNode main.cpp)
target_link_libraries(mainNode)
target_link_libraries(mainNode -lssl -lcrypto)
target_link_libraries(mainNode mainNodeLib allUtils)
enable_coverage_for_target(mainNode)


add_executable(consumer aqmpCons.cpp)
target_link_libraries(consumer PUBLIC network_shared_lib)


add_executable(producer aqmpProd.cpp)
target_link_libraries(producer PUBLIC network_shared_lib)


if (BUILD_TESTS)
    add_subdirectory(test)
    add_custom_command(
            TARGET mainNode
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:mainNode>
            ${CMAKE_BINARY_DIR}/mainNode/test/$<TARGET_FILE_NAME:mainNode>
            COMMENT "Copying mainNode to test directory"
    )

    file(COPY config
            DESTINATION ${CMAKE_BINARY_DIR}/mainNode/test)

endif ()