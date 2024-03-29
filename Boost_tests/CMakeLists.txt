set (Boost_USE_STATIC_LIBS OFF)
find_package (Boost REQUIRED COMPONENTS unit_test_framework)
find_package(Matplot++ REQUIRED)
include_directories (${Boost_INCLUDE_DIRS})

add_executable (Boost_Tests_run test1.cpp)
target_link_libraries (Boost_Tests_run ${Boost_LIBRARIES})
target_link_libraries(Boost_Tests_run Matplot++::matplot)
target_link_libraries (Boost_Tests_run myLibrary)