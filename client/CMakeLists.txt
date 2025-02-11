find_package(Qt6 REQUIRED COMPONENTS Widgets Xml Core Charts Sql Http)
find_package(Matplot++ REQUIRED)

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
target_include_directories(plotLib include)
target_link_libraries(plotLib plotLib PUBLIC allUtils Matplot++::matplot)

add_executable(checker src/eigenTest.cpp )
target_link_libraries(checker PUBLIC plotLib )


add_executable(generator ../client/src/coordGenerate.cpp)
target_link_libraries(generator PUBLIC io_lib OpenMP::OpenMP_CXX allUtils)



add_executable(grids ../client/src/darwingGrids.cpp)
target_link_libraries(grids PUBLIC io_lib allUtils)
#todo move to client part
#option(BUILD_GUI "Build GUI" OFF)
#if(BUILD_GUI)
#    find_package(Qt6 REQUIRED COMPONENTS Widgets Xml Core Charts Sql)
#    set(CMAKE_AUTOMOC ON)
#    set(CMAKE_AUTORCC ON)
#    set(CMAKE_AUTOUIC ON)
#    file(GLOB GUI_SOURCES src/*.cpp)
#    add_executable(qt_main ${GUI_SOURCES})
#
#    target_link_libraries(qt_main  Qt6::Widgets Qt6::Core Qt6::Xml Qt6::Gui Qt6::Sql Qt6::Charts FileUtilsLib)
#endif (BUILD_GUI)

