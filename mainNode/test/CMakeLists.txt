



file(COPY mappings DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
add_executable(serviceTest rabbitMQRestServiceTest.cpp)
target_link_libraries(serviceTest PUBLIC amqpUtils gtest gmock)
enable_coverage_for_target(serviceTest)



