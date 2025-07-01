
set(HOST_PARAMS    "$ENV{host} $ENV{username} $ENV{password}")
set(HOST_PARAMS_DB "$ENV{pghost} $ENV{username} $ENV{password}")
set(QUEUE_TEST_ARGS host username password)
set(DB_TEST_ARGS pghost username password)

add_env_test(
        compNodeTest
        controller/CompNodeTests.cpp
        QUEUE_TEST_ARGS
)

add_executable(benchServiceBench BenhcmarkRunnerBenchs.cpp)
target_link_libraries(benchServiceBench computationalNodeLib)
