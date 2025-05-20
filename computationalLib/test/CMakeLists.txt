set(CMAKE_CXX_STANDARD 23)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

foreach(var IN ITEMS host pghost username password)
    if (NOT DEFINED ENV{${var}})
        message(FATAL_ERROR "For the testing purposes environment variable '${var}' must be set")
    endif()
endforeach()

set(HOST_PARAMS    "$ENV{host} $ENV{username} $ENV{password}")
set(HOST_PARAMS_DB "$ENV{pghost} $ENV{username} $ENV{password}")

separate_arguments(HOST_PARAMS_LIST    NATIVE_COMMAND "${HOST_PARAMS}")
separate_arguments(HOST_PARAMS_DB_LIST NATIVE_COMMAND "${HOST_PARAMS_DB}")


message("Variable vals: ${HOST_PARAMS_LIST} ${HOST_PARAMS_DB_LIST}")


add_executable(core_Tests math_core/smokeTests.cpp math_core/dipoleCommonTests.cpp)
target_link_libraries(core_Tests gtest gtest_main)
target_link_libraries(core_Tests math_core_lib commonLib)
enable_coverage_for_target(core_Tests)
gtest_discover_tests(core_Tests NO_PRETTY_TYPES NO_PRETTY_VALUES)



add_executable(network_common_tests network_shared/networkTypesTests.cpp)
target_link_libraries(network_common_tests gtest gtest_main)
target_link_libraries(network_common_tests network_shared_lib)
enable_coverage_for_target(network_common_tests)
gtest_discover_tests(network_common_tests NO_PRETTY_TYPES NO_PRETTY_VALUES)



add_executable(serviceTest interegated/rabbitMQRestServiceTest.cpp)
target_link_libraries(serviceTest PUBLIC network_shared_lib gtest gmock)
enable_coverage_for_target(serviceTest)
gtest_add_tests(
        TARGET serviceTest
        EXTRA_ARGS ${HOST_PARAMS_LIST})


add_executable(consumerServiceTest interegated/AMQPConsumerServiceTests.cpp)
target_link_libraries(consumerServiceTest PUBLIC network_shared_lib gtest gmock)
enable_coverage_for_target(consumerServiceTest)

gtest_add_tests(
        TARGET consumerServiceTest
        EXTRA_ARGS ${HOST_PARAMS_LIST})


add_executable(publisherServiceTest interegated/AMQPPublisherTests.cpp)
target_link_libraries(publisherServiceTest PUBLIC network_shared_lib gtest gmock)
enable_coverage_for_target(publisherServiceTest)


add_executable(databaseServiceTest interegated/DbServiceTest.cpp)
target_link_libraries(databaseServiceTest PUBLIC Drogon::Drogon network_shared_lib gtest gmock)
enable_coverage_for_target(databaseServiceTest)
gtest_add_tests(
        TARGET databaseServiceTest
        EXTRA_ARGS ${HOST_PARAMS_DB_LIST})

add_executable(benchmark benchmark.cpp fileHandler.cpp benchmarkHandler.cpp)
target_link_libraries(benchmark math_core_lib commonLib)

file(COPY fixture
        DESTINATION ${CMAKE_BINARY_DIR}/computationalLib/test)
