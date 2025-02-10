set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


project(utilLib)
if(BUILD_TESTS)
    add_subdirectory(test)
endif ()

find_package(Eigen3 3.3 REQUIRED NO_MODULE)

include_directories(include)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/utilLib/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/utilLib/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/utilLib/bin)


file(GLOB common CONFIGURE_DEPENDS
        "lib/common/*.cpp"
)
add_library(commonLib ${common})
target_link_libraries(commonLib PUBLIC Eigen3::Eigen)

file(GLOB_RECURSE parallelUtils CONFIGURE_DEPENDS
        "lib/parallelUtils/*.cpp"
)

add_library(parallelLib ${parallelUtils})

