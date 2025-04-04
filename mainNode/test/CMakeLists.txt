



file(COPY mappings DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
add_executable(serviceTest RabbitMQRestServiceTest.cpp)
target_link_libraries(serviceTest PUBLIC amqpUtils gtest gmock gtest_main)
enable_coverage_for_target(serviceTest)




