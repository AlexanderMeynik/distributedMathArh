set(HOST_PARAMS    "$ENV{host} $ENV{username} $ENV{password}")
set(HOST_PARAMS_DB "$ENV{pghost} $ENV{username} $ENV{password}")
set(QUEUE_TEST_ARGS host username password)
set(DB_TEST_ARGS pghost username password)

add_env_test(
        mainNodeTest
        controller/MainNodeTests.cpp
        QUEUE_TEST_ARGS
)