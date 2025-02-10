

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()


if(BUILD_TESTS)
    add_subdirectory(test)
endif ()

