#include <memory>

#include <gtest/gtest.h>

#include "common/errorHandling.h"
#include "amqpRestService.h"
#include "../../computationalLib/test/GoogleCommon.h"

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

    std::shared_ptr<basicAuthHandler> hander;

    static inline std::string qq="test_queue";
    static inline std::string exch="amq.direct";
    static inline std::string rKey="test_routing";
    static inline std::string vhost="%2F";

    void SetUp() override {

        hander=std::make_shared<basicAuthHandler>(g_serviceParams.username, g_serviceParams.password);
        m_service_ptr = std::make_shared<RabbitMQRestService>(g_serviceParams.host, hander.get());
    }


    void TearDown() override {

    }
};


TEST_F(RabbitMQRestServiceTest, whoAmI) {
    auto json =m_service_ptr->whoami();
    EXPECT_EQ(json["name"].asString(),g_serviceParams.username);
}

TEST_F(RabbitMQRestServiceTest, whoAmI_Unauthorized) {

    auto handlerS=std::make_shared<basicAuthHandler>(*hander);
    handlerS->setActive(false);
    EXPECT_EXCEPTION_WITH_ARGS(
            performCurlRequest(
                    "/api/whoami",
                    "GET",
                    g_serviceParams.host,
                    handlerS.get()
                    ),
            shared::httpError,
            std::make_tuple(401));
}

TEST_F(RabbitMQRestServiceTest, CreateQueue) {
    Json::Value args;
    args["worker_id"] = "worker_123";

    EXPECT_TRUE(m_service_ptr->createQueue(vhost,qq , args));

    auto queues=m_service_ptr->listQueues(vhost);

    EXPECT_TRUE(std::find(queues.begin(), queues.end(),qq)!=queues.end());

}


TEST_F(RabbitMQRestServiceTest, GetQueueStats) {
    Json::Value stats;

    EXPECT_NO_THROW(stats = m_service_ptr->getQueueStats(vhost, "test_queue"));

    auto ss=stats["messages"];
    EXPECT_EQ(stats["messages"].asInt(), 0);
}

TEST_F(RabbitMQRestServiceTest, ListQueues) {
    std::vector<std::string> queues = m_service_ptr->listQueues(vhost);
    EXPECT_GT(queues.size(), 0);

    std::vector<std::string> queues2 = m_service_ptr->listQueues(vhost);


    testCommon::compareArrays(queues,queues2,[](const std::string&a,const std::string&a2,size_t i,double tol){return a==a2;});
    //todo more verbose operator
}

TEST_F(RabbitMQRestServiceTest, BindQueueToExchange) {

    EXPECT_TRUE(m_service_ptr->bindQueueToExchange(vhost, qq, exch,rKey ));

    auto bindings=m_service_ptr->getQueueBindings(vhost,qq);

    auto qb=queueBinding(exch,rKey);

    EXPECT_TRUE(std::find(bindings.begin(), bindings.end(),qb)!=bindings.end());
}

TEST_F(RabbitMQRestServiceTest, UnbindQueueFromExchange) {



    EXPECT_TRUE(m_service_ptr->unbindQueueFromExchange(vhost, qq, exch, rKey));

    auto bindings=m_service_ptr->getQueueBindings(vhost,qq);
    auto qb=queueBinding(exch,rKey);

    EXPECT_TRUE(std::find(bindings.begin(), bindings.end(),qb)==bindings.end());
}

TEST_F(RabbitMQRestServiceTest, DeleteQueue) {

    EXPECT_TRUE(m_service_ptr->deleteQueue(vhost, qq));

    auto queues=m_service_ptr->listQueues(vhost);

    EXPECT_TRUE(std::find(queues.begin(), queues.end(),qq)==queues.end());


}


TEST_F(RabbitMQRestServiceTest, CreateUser) {
    EXPECT_TRUE(m_service_ptr->createUser("new_user", "password"));

}

TEST_F(RabbitMQRestServiceTest, DeleteUser) {
    EXPECT_TRUE(m_service_ptr->deleteUser("new_user"));
}

TEST_F(RabbitMQRestServiceTest, CreateExchange) {
    exchange exchange{"exch",amqpCommon::exchange::exchangeData{"user",AMQP::direct}};

    EXPECT_TRUE(m_service_ptr->createExchange(vhost,exchange,Json::Value{}));


    auto exhc=m_service_ptr->getExchanges(vhost);

    EXPECT_TRUE(std::find_if(exhc.begin(), exhc.end(),
                             [&exchange](const amqpCommon::exchange&a1)
                             {
                                 return a1.name==exchange.name;
                             })
                             !=exhc.end());


}

TEST_F(RabbitMQRestServiceTest, DeleteExchange) {

    std::string nn="exch";
    EXPECT_TRUE(m_service_ptr->deleteExchange(vhost, nn));

    auto exhc=m_service_ptr->getExchanges(vhost);

    EXPECT_TRUE(std::find_if(exhc.begin(), exhc.end(),
                             [&nn](const amqpCommon::exchange&a1)
                             {
                                 return a1.name==nn;
                             })
                ==exhc.end());


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
