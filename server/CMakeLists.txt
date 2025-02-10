
find_package(cpprestsdk REQUIRED)
add_executable(main main.cpp)


if(BUILD_TESTS)
    add_subdirectory(test)
endif ()

target_link_libraries(main PRIVATE cpprestsdk::cpprest)
target_link_libraries(main PRIVATE -lssl -lcrypto)

include_directories(../computationalNode/include)
include_directories(../utilLib/include)
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
target_link_libraries(controllerLib PRIVATE serviceLib math_core_lib)



add_executable(server server.cpp)
target_link_libraries(server cpprestsdk::cpprest)
target_link_libraries(server -lssl -lcrypto)
#target_link_libraries(server  math_core_lib parallelLib)
target_link_libraries(server controllerLib)


add_executable(client client.cpp)
target_link_libraries(client PRIVATE cpprestsdk::cpprest)
target_link_libraries(client PRIVATE -lssl -lcrypto)

