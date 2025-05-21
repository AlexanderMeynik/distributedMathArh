find_package(Qt6 REQUIRED COMPONENTS Widgets Xml Core Charts Sql DataVisualization Network)
find_package(Matplot++ REQUIRED)


set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/client/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/client/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/client/bin)

if (BUILD_TESTS)
    add_subdirectory(test)
endif ()


set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_library(plotLib lib/plotingUtils.cpp lib/MeshPlot.cpp include/MeshPlot.h)
target_link_libraries(plotLib PUBLIC allUtils Matplot++::matplot Qt6::Charts Qt6::DataVisualization Qt6::Widgets)
target_include_directories(plotLib PUBLIC include)


add_executable(checker src/eigenTest.cpp)
target_link_libraries(checker PUBLIC plotLib)


add_executable(generator src/coordGenerate.cpp)
target_link_libraries(generator PUBLIC math_core_lib plotLib OpenMP::OpenMP_CXX allUtils)





add_executable(qtPlotExample src/createPlotQt.cpp)
target_link_libraries(qtPlotExample PUBLIC allUtils plotLib)
target_include_directories(qtPlotExample PUBLIC include)


add_executable(httpClientTest src/requestInformation.cpp)
target_link_libraries(httpClientTest PUBLIC allUtils plotLib)
target_include_directories(httpClientTest PUBLIC include)


add_executable(averageGetter src/avgsTest.cpp ../computationalLib/test/fileHandler.cpp)
target_link_libraries(averageGetter PUBLIC allUtils network_shared_lib math_core_lib plotLib)
target_include_directories(averageGetter PUBLIC include)