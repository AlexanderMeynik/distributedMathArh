
SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -g -march=native -fpic -ffast-math -ftree-vectorize")
find_package(Eigen3 3.3 REQUIRED NO_MODULE)
#[[
add_executable (test1 test1.cpp)

add_executable (test2 test2.cpp)

add_executable(test3.cpp test3.cpp nested_template.h)

target_link_libraries(test3.cpp PUBLIC Eigen3::Eigen)]]
