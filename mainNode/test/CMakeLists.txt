



file(COPY mappings DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
#[[add_executable(serviceTest ../../computationalLib/test/interegated/rabbitMQRestServiceTest.cpp)
target_link_libraries(serviceTest PUBLIC network_shared_lib gtest gmock)
enable_coverage_for_target(serviceTest)]]



