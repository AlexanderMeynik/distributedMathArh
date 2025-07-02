set(CMAKE_CXX_STANDARD 23)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})


set(HOST_PARAMS    "$ENV{host} $ENV{username} $ENV{password}")
set(HOST_PARAMS_DB "$ENV{pghost} $ENV{username} $ENV{password}")
set(QUEUE_TEST_ARGS host username password)
set(DB_TEST_ARGS pghost username password)

add_executable(core_Tests math_core/smokeTests.cpp math_core/dipoleCommonTests.cpp)
target_link_libraries(core_Tests gtest gtest_main)
target_link_libraries(core_Tests math_core_lib commonLib)
enable_coverage_for_target(core_Tests)
gtest_discover_tests(core_Tests NO_PRETTY_TYPES NO_PRETTY_VALUES)

add_executable(network_common_tests network_shared/networkTypesTests.cpp network_shared/connectionStringsTests.cpp)
target_link_libraries(network_common_tests gtest gtest_main)
target_link_libraries(network_common_tests network_shared_lib)
enable_coverage_for_target(network_common_tests)
gtest_discover_tests(network_common_tests NO_PRETTY_TYPES NO_PRETTY_VALUES)

add_env_test(
        serviceTest
        interegated/rabbitMQRestServiceTest.cpp
        QUEUE_TEST_ARGS
)

add_env_test(
        consumerServiceTest
        interegated/AMQPConsumerServiceTests.cpp
        QUEUE_TEST_ARGS
)

add_env_test(
        publisherServiceTest
        interegated/AMQPPublisherTests.cpp
        QUEUE_TEST_ARGS
)

add_env_test(
        databaseServiceTest
        interegated/DbServicesTests.cpp
        DB_TEST_ARGS
)


add_executable(benchmark benchmark.cpp benchmarkHandler.cpp)
target_link_libraries(benchmark math_core_lib commonLib testingUtilsLib)

file(COPY fixture
        DESTINATION ${CMAKE_BINARY_DIR}/computationalLib/test)
