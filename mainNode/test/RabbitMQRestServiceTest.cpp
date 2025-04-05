#include <memory>

#include <gtest/gtest.h>

#include "common/errorHandling.h"
#include "amqpRestService.h"

using namespace amqpCommon;

struct AuthParams {
    std::string host;
    std::string username;
    std::string password;
};

AuthParams g_serviceParams;

class RabbitMQRestServiceTest : public ::testing::Test {
protected:
    std::shared_ptr<RabbitMQRestService> m_service_ptr;

    void SetUp() override {
        m_service_ptr = std::make_shared<RabbitMQRestService>(g_serviceParams.host, g_serviceParams.username, g_serviceParams.password);
    }


    void TearDown() override {

    }
};


TEST_F(RabbitMQRestServiceTest, whoAmI) {
    auto json =m_service_ptr->whoami();
    EXPECT_EQ(json["name"].asString(),g_serviceParams.username);
}

TEST_F(RabbitMQRestServiceTest, whoAmI_Unauthorized) {
    m_service_ptr->setAuth(false);
    try {
        m_service_ptr->whoami();
        FAIL()<<"Expected shared::httpError but nothing was thrown";
    } catch (const shared::httpError& e) {
        EXPECT_EQ(401, e.get<0>());
    } catch (...) {
        FAIL() << "Expected MyException but caught a different exception";
    }//todo exception matcher macros

    m_service_ptr->setAuth(true);
}

TEST_F(RabbitMQRestServiceTest, CreateQueue) {
    Json::Value args;
    args["worker_id"] = "worker_123";

    EXPECT_TRUE(m_service_ptr->createQueue("%2F", "test_queue", args));
}


TEST_F(RabbitMQRestServiceTest, GetQueueStats) {
    Json::Value stats = m_service_ptr->getQueueStats("%2F", "test_queue");
    auto ss=stats["messages"];
    EXPECT_EQ(stats["messages"].asInt(), 0);
}

TEST_F(RabbitMQRestServiceTest, ListQueues) {
    std::vector<std::string> queues = m_service_ptr->listQueues("%2F");
    EXPECT_GT(queues.size(), 0);
    EXPECT_EQ(queues[0], "TestQ");
}

TEST_F(RabbitMQRestServiceTest, BindQueueToExchange) {
    EXPECT_TRUE(m_service_ptr->bindQueueToExchange("%2F", "test_queue", "amq.direct", "test_routing"));
}

TEST_F(RabbitMQRestServiceTest, UnbindQueueFromExchange) {
    EXPECT_TRUE(m_service_ptr->unbindQueueFromExchange("%2F", "test_queue", "amq.direct", "test_routing"));
}

TEST_F(RabbitMQRestServiceTest, DeleteQueue) {
    EXPECT_TRUE(m_service_ptr->deleteQueue("%2F", "test_queue"));
}

TEST_F(RabbitMQRestServiceTest, CreateUser) {
    EXPECT_TRUE(m_service_ptr->createUser("new_user", "password"));
}

TEST_F(RabbitMQRestServiceTest, DeleteUser) {
    EXPECT_TRUE(m_service_ptr->deleteUser("new_user"));
}


int main(int argc, char **argv) {


    if (argc < 4) {
            std::cerr << "Usage: " << argv[0] << " <host> <login> <password>\n";
            return 1;
    }

    g_serviceParams.host=argv[1];
    g_serviceParams.username=argv[2];
    g_serviceParams.password=argv[3];

    ::testing::InitGoogleTest(&argc, argv);


    return RUN_ALL_TESTS();
}
