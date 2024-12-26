cmake_minimum_required(VERSION 3.26)
project(Diplom)


add_subdirectory(server)

add_subdirectory(utilLib)
add_subdirectory(computationalNode)


option(BUILD_DOC "Build documentation" OFF)
if (BUILD_DOC)
    add_subdirectory(doc)
endif (BUILD_DOC)