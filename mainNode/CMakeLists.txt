if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

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
target_link_libraries(servLib2 PUBLIC math_core_lib Drogon::Drogon)
target_include_directories(servLib2 PUBLIC include)
enable_coverage_for_target(servLib2)

add_executable(mainNode main.cpp)
target_link_libraries(mainNode)
target_link_libraries(mainNode -lssl -lcrypto)
target_link_libraries(mainNode servLib2 allUtils)
enable_coverage_for_target(mainNode)

add_library(amqpUtils src/amqpCommon.cpp src/amqpRestService.cpp)
target_link_libraries( amqpUtils PUBLIC fmt::fmt amqpcpp jsoncpp_lib CURL::libcurl)
target_include_directories(amqpUtils PUBLIC ../utilLib/include)
enable_coverage_for_target(amqpUtils)
target_include_directories(amqpUtils PUBLIC include)

add_executable(consumer aqmpCons.cpp)
target_link_libraries(consumer PUBLIC amqpUtils )


add_executable(producer aqmpProd.cpp )
target_link_libraries(producer PUBLIC amqpUtils)
