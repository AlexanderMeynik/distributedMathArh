project(main)

find_package(cpprestsdk REQUIRED)
find_package(Matplot++ REQUIRED)
find_package(OpenMP REQUIRED)
add_executable(main main.cpp)

target_link_libraries(main PRIVATE cpprestsdk::cpprest)
target_link_libraries(main PRIVATE -lssl -lcrypto)


# Add the server executable
add_executable(server server.cpp)
target_link_libraries(server cpprestsdk::cpprest)
target_link_libraries(server -lssl -lcrypto)
target_link_libraries(server  myLibrary)

# Add the client executable
add_executable(client client.cpp)
target_link_libraries(client PRIVATE cpprestsdk::cpprest)
target_link_libraries(client PRIVATE -lssl -lcrypto)

target_link_libraries(server Matplot++::matplot)#todo почему мне жто надо делать
target_link_libraries(server OpenMP::OpenMP_CXX)
target_link_libraries(server Clock)