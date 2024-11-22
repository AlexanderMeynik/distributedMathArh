project(main)
set(CMAKE_CXX_STANDARD 20)
find_package(cpprestsdk REQUIRED)
add_executable(main main.cpp)

target_link_libraries(main PRIVATE cpprestsdk::cpprest)
target_link_libraries(main PRIVATE -lssl -lcrypto)

include_directories(../computationalNode/include)
include_directories(../utilLib/include)
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
target_link_libraries(controllerLib PRIVATE serviceLib math_core_lib)


# Add the server executable
add_executable(server server.cpp)
target_link_libraries(server cpprestsdk::cpprest)
target_link_libraries(server -lssl -lcrypto)
#target_link_libraries(server  math_core_lib parallelLib)
target_link_libraries(server controllerLib)

# Add the client executable
add_executable(client client.cpp)
target_link_libraries(client PRIVATE cpprestsdk::cpprest)
target_link_libraries(client PRIVATE -lssl -lcrypto)

