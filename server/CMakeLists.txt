
find_package(cpprestsdk REQUIRED)
add_executable(main main.cpp)


if(BUILD_TESTS)
    add_subdirectory(test)
endif ()

target_link_libraries(main PRIVATE cpprestsdk::cpprest)
target_link_libraries(main PRIVATE -lssl -lcrypto)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/server/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/server/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/server/bin)


include_directories(../mdspan/include)

file(GLOB services CONFIGURE_DEPENDS
        "*service/*.h"
        "service/*.cpp"
)

file(GLOB controllers CONFIGURE_DEPENDS
        "controller/*.h"
        "controller/*.cpp"
)


add_library(serviceLib ${services})

add_library(controllerLib ${controllers})
target_link_libraries(controllerLib PUBLIC serviceLib math_core_lib)



add_executable(server server.cpp)
target_link_libraries(server cpprestsdk::cpprest)
target_link_libraries(server -lssl -lcrypto)
target_link_libraries(server controllerLib allUtils)

add_executable(server2 server2.cpp)
target_link_libraries(server2 cpprestsdk::cpprest)
target_link_libraries(server2 -lssl -lcrypto)
target_link_libraries(server2 controllerLib allUtils)

add_executable(client client.cpp)
target_link_libraries(client PRIVATE cpprestsdk::cpprest)
target_link_libraries(client PRIVATE -lssl -lcrypto)

