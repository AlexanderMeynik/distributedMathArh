set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(utilLib)
find_package(Matplot++ REQUIRED)#todo remove

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




file(GLOB_RECURSE plotUtils CONFIGURE_DEPENDS
        "lib/plotUtils/*.cpp"
)



add_library(plotUtilsLib ${plotUtils})
target_link_libraries(plotUtilsLib PUBLIC commonLib Matplot++::matplot)
target_include_directories(plotUtilsLib PUBLIC ../mdspan/include)

add_library(allUtils STATIC ${plotUtils})
target_link_libraries(allUtils PUBLIC commonLib parallelLib Matplot++::matplot)
target_include_directories(allUtils PUBLIC ../mdspan/include include)


add_executable(checker ../utilLib/src/eigenTest.cpp)
target_link_libraries(checker PUBLIC   plotUtilsLib)