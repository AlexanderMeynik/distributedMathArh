find_package(Qt6 REQUIRED COMPONENTS Widgets Xml Core Charts Sql DataVisualization Network)
find_package(Matplot++ REQUIRED)
find_package(OpenMP REQUIRED)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/client/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/client/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/client/bin)
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()


if(BUILD_TESTS)
    add_subdirectory(test)
endif ()

add_library(plotLib lib/plotingUtils.cpp)
target_link_libraries(plotLib PUBLIC plotUtilsLib allUtils Matplot++::matplot)

add_executable(checker src/eigenTest.cpp )
target_link_libraries(checker PUBLIC plotLib )


add_executable(generator src/coordGenerate.cpp)
target_link_libraries(generator PUBLIC io_lib math_core_lib plotLib OpenMP::OpenMP_CXX allUtils)



add_executable(grids src/darwingGrids.cpp )
target_link_libraries(grids PUBLIC io_lib math_core_lib allUtils)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
add_executable(qtPlotExample src/createPlotQt.cpp lib/surfacegraph.cpp include/surfacegraph.h)
target_link_libraries(qtPlotExample PUBLIC plotUtilsLib  Qt6::Charts Qt6::DataVisualization Qt6::Network plotLib)
target_include_directories(qtPlotExample PUBLIC include)
