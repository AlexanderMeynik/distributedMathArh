

if (BUILD_TESTS)
    add_subdirectory(test)
endif ()

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


add_library(servLib2 ${httpSources})
target_link_libraries(servLib2 PUBLIC math_core_lib network_shared_lib Drogon::Drogon)
target_include_directories(servLib2 PUBLIC include)
enable_coverage_for_target(servLib2)

add_executable(mainNode main.cpp)
target_link_libraries(mainNode)
target_link_libraries(mainNode -lssl -lcrypto)
target_link_libraries(mainNode servLib2 allUtils)
enable_coverage_for_target(mainNode)


add_executable(consumer aqmpCons.cpp)
target_link_libraries(consumer PUBLIC network_shared_lib)


add_executable(producer aqmpProd.cpp)
target_link_libraries(producer PUBLIC network_shared_lib)
