

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()


if(BUILD_TESTS)
    add_subdirectory(test)
endif ()


find_package(libpqxx REQUIRED)
find_package(glog 0.7.1 REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(OpenMP REQUIRED)
find_package(Eigen3 3.3 REQUIRED NO_MODULE)


set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/computationalNode/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/computationalNode/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/computationalNode/bin)



file(GLOB_RECURSE math_core_files CONFIGURE_DEPENDS
        "lib/computationalLib/math_core/*.cpp"
)



add_library(math_core_lib ${math_core_files})
target_link_libraries(math_core_lib PUBLIC allUtils)

target_include_directories(math_core_lib PUBLIC include)

file(GLOB_RECURSE interfaces_files CONFIGURE_DEPENDS
        "lib/computationalLib/interfaces/*.cpp"
)
add_library(interfaces_lib ${interfaces_files})
target_link_libraries(interfaces_lib PUBLIC math_core_lib)
target_include_directories(interfaces_lib PUBLIC include)

file(GLOB_RECURSE iolib_files CONFIGURE_DEPENDS
        "lib/iolib/*cpp"
)
add_library(io_lib ${iolib_files})
target_link_libraries(io_lib PUBLIC math_core_lib)


add_executable(generator src/coordGenerate.cpp)
target_link_libraries(generator PUBLIC io_lib OpenMP::OpenMP_CXX allUtils)



add_executable(grids src/darwingGrids.cpp)
target_link_libraries(grids PUBLIC io_lib allUtils)


add_executable(printerssst src/printersTests.cpp)
target_link_libraries(grids PUBLIC  Eigen3::Eigen)
