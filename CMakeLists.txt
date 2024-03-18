cmake_minimum_required(VERSION 3.26)
project(Magister1)

#add_subdirectory(Boost_tests)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
#set(CMAKE_BUILD_TYPE RelWithDebInfo)
#set(CMAKE_CXX_FLAGS "-O1 -g -march=native -fpic -ftree-vectorize")
#SET( CMAKE_CXX_FLAGS_DEBUG   "-O1 -g -march=native -fpic -ftree-vectorize")

set(CMAKE_CXX_FLAGS "-O0 -g")
SET( CMAKE_CXX_FLAGS_DEBUG   "-O0 -g" )
#find_package(OpenXLSX)
#find_package(Matplot++ REQUIRED CONFIG)
find_library(PQXX_LIB pqxx)
find_library(PQ_LIB pq)
find_package(GSL REQUIRED)
#add_executable(Magister1 main.cpp) #Dipoles.cpp)



# Add Executable
add_executable(Magister1 Dipoles.h main.cpp)
add_executable(Magister2 Dipoles2.h main.cpp)
add_executable(ConFReader configReader.cpp)
# Link Libraries
target_link_libraries(Magister1 PRIVATE ${GSL_LIBRARIES})
target_link_libraries(Magister1 PRIVATE ${PQXX_LIB} ${PQ_LIB})

# Include Directories
target_include_directories(Magister1 PRIVATE ${GSL_INCLUDE_DIRS})



target_link_libraries(ConFReader PRIVATE ${GSL_LIBRARIES})
target_link_libraries(ConFReader PRIVATE ${PQXX_LIB} ${PQ_LIB})

# Include Directories
target_include_directories(ConFReader PRIVATE ${GSL_INCLUDE_DIRS})
#target_link_libraries(Magister1 OpenXLSX::OpenXLSX)

#target_link_libraries(Magister1 ${PQXX_LIB} ${PQ_LIB} -L/usr/local/lib -litpp)
#target_link_libraries(Magister1 OpenXLSX::OpenXLSX ${PQXX_LIB} ${PQ_LIB} -L/usr/local/lib -litpp)
#target_link_libraries(Magister1 Matplot++::matplot)