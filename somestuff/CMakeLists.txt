
SET( CMAKE_CXX_FLAGS_RELEASE    "-O3 -g -march=native -fpic -ffast-math -ftree-vectorize" )

add_executable (test1 test1.cpp)

add_executable (test2 test2.cpp)

