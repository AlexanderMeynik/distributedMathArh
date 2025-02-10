find_package(Qt6 REQUIRED COMPONENTS Widgets Xml Core Charts Sql)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()


if(BUILD_TESTS)
    add_subdirectory(test)
endif ()



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

