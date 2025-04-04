#include <gtest/gtest.h>
#include <drogon/HttpAppFramework.h>
#include "amqpRestService.h"
using namespace amqpCommon;


class DrogonTestEnvironment : public ::testing::Environment {
public:
    void SetUp() override {
        drogon::app()
                .setThreadNum(1)
                .setLogLevel(trantor::Logger::kFatal);

        appThread = std::thread([]() {
            drogon::app().run();
        });


        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::cout << "Drogon app started for testing\n";
    }

    void TearDown() override {

        drogon::app().quit();
        if (appThread.joinable()) {
            appThread.join();
        }
        std::cout << "Drogon app stopped after testing\n";
    }

private:
    std::thread appThread;
};

class RabbitMQRestServiceTest : public ::testing::Test {
protected:
    RabbitMQRestService* service;

    void SetUp() override {
        service = new RabbitMQRestService("http://localhost:15672", "sysadmin","syspassword");
    }

    void TearDown() override {
        delete service;
    }
};

TEST_F(RabbitMQRestServiceTest, CreateQueue) {
    Json::Value args;
    args["worker_id"] = "worker_123";

    EXPECT_TRUE(service->createQueue("%2F", "test_queue", args));
}


TEST_F(RabbitMQRestServiceTest, GetQueueStats) {
    Json::Value stats = service->getQueueStats("%2F", "test_queue");
    EXPECT_TRUE(stats.isMember("messages"));
    EXPECT_EQ(stats["messages"].asInt(), 0);
}

TEST_F(RabbitMQRestServiceTest, ListQueues) {
    std::vector<std::string> queues = service->listQueues("%2F");
    EXPECT_GT(queues.size(), 0);
    EXPECT_EQ(queues[0], "test_queue");
}

TEST_F(RabbitMQRestServiceTest, BindQueueToExchange) {
    EXPECT_TRUE(service->bindQueueToExchange("%2F", "test_queue", "amq.direct", "test_routing"));
}

TEST_F(RabbitMQRestServiceTest, UnbindQueueFromExchange) {
    EXPECT_TRUE(service->unbindQueueFromExchange("%2F", "test_queue", "amq.direct", "test_routing"));
}

TEST_F(RabbitMQRestServiceTest, DeleteQueue) {
    EXPECT_TRUE(service->deleteQueue("%2F", "test_queue"));
}

TEST_F(RabbitMQRestServiceTest, CreateUser) {
    EXPECT_TRUE(service->createUser("new_user", "password"));
}

TEST_F(RabbitMQRestServiceTest, DeleteUser) {
    EXPECT_TRUE(service->deleteUser("new_user"));
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);


    ::testing::AddGlobalTestEnvironment(new DrogonTestEnvironment);

    return RUN_ALL_TESTS();
}
