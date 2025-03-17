set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(utilLib)


if (BUILD_TESTS)
    add_subdirectory(test)
endif ()


include_directories(include)


set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/utilLib/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/utilLib/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/utilLib/bin)


file(GLOB common CONFIGURE_DEPENDS
        "lib/common/*.cpp"
)
add_library(commonLib ${common})
target_link_libraries(commonLib PUBLIC Eigen3::Eigen jsoncpp_lib)
target_include_directories(commonLib PUBLIC ../mdspan/include)
target_include_directories(commonLib PUBLIC include)

file(GLOB_RECURSE parallelUtils CONFIGURE_DEPENDS
        "lib/parallelUtils/*.cpp"
)

add_library(parallelLib ${parallelUtils})
target_include_directories(parallelLib PUBLIC include)


file(GLOB_RECURSE plotUtils CONFIGURE_DEPENDS
        "lib/plotUtils/*.cpp"
)


add_library(plotUtilsLib STATIC ${plotUtils})
target_link_libraries(plotUtilsLib PUBLIC commonLib)

add_library(allUtils STATIC ${parallelUtils})
target_link_libraries(allUtils PUBLIC plotUtilsLib)


