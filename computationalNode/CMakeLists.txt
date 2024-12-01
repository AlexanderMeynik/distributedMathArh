cmake_minimum_required(VERSION 3.26)
project(data_deduplication_service)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

SET(CMAKE_CXX_FLAGS_DEBUG "-g")
SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -ffast-math -DNDEBUG -march=native -fpic -ftree-vectorize")
SET(CMAKE_CXX_FLAGS " -std=gnu++2b")

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

add_subdirectory(test)


find_package(libpqxx REQUIRED)
find_package(glog 0.7.1 REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(OpenMP REQUIRED)
find_package(Eigen3 3.3 REQUIRED NO_MODULE)
find_package(Matplot++ REQUIRED)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/computationalNode/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/computationalNode/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/computationalNode/bin)



file(GLOB_RECURSE math_core_files CONFIGURE_DEPENDS
        "lib/computationalLib/math_core/*.cpp"
)



add_library(math_core_lib ${math_core_files})
target_link_libraries(math_core_lib PUBLIC ${GSL_LIBRARIES})
target_link_libraries(math_core_lib PUBLIC ${PQXX_LIB} ${PQ_LIB})#todo убрать
target_link_libraries(math_core_lib PUBLIC Eigen3::Eigen)
target_include_directories(math_core_lib PUBLIC ${GSL_INCLUDE_DIRS})
target_link_libraries(math_core_lib PUBLIC Matplot++::matplot)
target_link_libraries(math_core_lib PUBLIC commonLib parallelLib)

target_include_directories(math_core_lib PUBLIC ../mdspan/include)
target_include_directories(math_core_lib PUBLIC include)
target_include_directories(math_core_lib PUBLIC ../utilLib/include)

file(GLOB_RECURSE interfaces_files CONFIGURE_DEPENDS
        "lib/computationalLib/interfaces/*.cpp"
)
add_library(interfaces_lib ${interfaces_files})
target_link_libraries(interfaces_lib PUBLIC parallelLib)
target_link_libraries(interfaces_lib PUBLIC math_core_lib)



target_include_directories(interfaces_lib PUBLIC ../mdspan/include)
target_include_directories(interfaces_lib PUBLIC include)
target_include_directories(interfaces_lib PUBLIC ../utilLib/include)

file(GLOB_RECURSE iolib_files CONFIGURE_DEPENDS
        "lib/iolib/*cpp"
)
add_library(io_lib ${iolib_files})


target_link_libraries(io_lib PUBLIC math_core_lib)
target_include_directories(io_lib PUBLIC ../mdspan/include)
option(BUILD_DOC "Build documentation" OFF)
if (BUILD_DOC)
    add_subdirectory(doc)
endif (BUILD_DOC)


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


add_executable(moveDipoles src/moveDipoles.cpp)
target_link_libraries(moveDipoles PUBLIC math_core_lib)

add_executable(generator src/coordGenerate.cpp)
target_link_libraries(generator PUBLIC math_core_lib)


add_executable(checker src/eigenTest.cpp)
target_link_libraries(checker PUBLIC math_core_lib)


add_executable(grids src/darwingGrids.cpp)
target_link_libraries(grids PUBLIC math_core_lib parallelLib)
