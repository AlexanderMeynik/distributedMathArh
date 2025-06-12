#include "network_shared/amqpRestService.h"
#include "../GoogleCommon.h"

using namespace amqp_common;

using test_common::AuthParams;

AuthParams g_serviceParams;

class RabbitMqRestServiceTest : public ::testing::Test {
 protected:
  std::shared_ptr<RabbitMQRestService> m_service_ptr_;

  std::shared_ptr<BasicAuthHandler> hander_;

  static inline std::string qq_ = "test_queue";
  static inline std::string exch_ = "amq.direct";
  static inline std::string r_key_ = "test_routing";
  static inline std::string vhost_ = "%2F";

  static inline std::string uname_ = "new_user";
  static inline std::string pass_ = "password";

  void SetUp() override {

    hander_ = std::make_shared<BasicAuthHandler>(g_serviceParams.username, g_serviceParams.password);
    m_service_ptr_ = std::make_shared<RabbitMQRestService>(g_serviceParams.host, hander_.get());
  }

  void TearDown() override {

  }
};

TEST_F(RabbitMqRestServiceTest, whoAmI) {
  auto json = m_service_ptr_->Whoami();
  EXPECT_EQ(json["name"].asString(), g_serviceParams.username);
}

TEST_F(RabbitMqRestServiceTest, whoAmI_Unauthorized) {

  auto handler_s = std::make_shared<BasicAuthHandler>(*hander_);
  handler_s->SetActive(false);
  EXPECT_EXCEPTION_WITH_ARGS(
      PerformCurlRequest(
          "/api/whoami",
          "GET",
          g_serviceParams.host,
          handler_s.get()
      ),
      shared::HttpError,
      std::make_tuple(401, ""));
}

TEST_F(RabbitMqRestServiceTest, CreateQueue) {
  Json::Value args;
  args["worker_id"] = "worker_123";

  auto q = network_types::queue{qq_, "user"};
  EXPECT_TRUE(m_service_ptr_->CreateQueue(vhost_, q, args));

  auto queues = m_service_ptr_->ListQueues(vhost_);

  EXPECT_TRUE(std::find(queues.begin(), queues.end(), qq_) != queues.end());

}

TEST_F(RabbitMqRestServiceTest, GetQueueStats) {
  Json::Value stats;

  EXPECT_NO_THROW(stats = m_service_ptr_->GetQueueStats(vhost_, "test_queue"));

  auto ss = stats["messages"];
  EXPECT_EQ(stats["messages"].asInt(), 0);
}

TEST_F(RabbitMqRestServiceTest, ListQueues) {
  std::vector<std::string> queues = m_service_ptr_->ListQueues(vhost_);
  EXPECT_GT(queues.size(), 0);

  std::vector<std::string> queues2 = m_service_ptr_->ListQueues(vhost_);

  test_common::CompareArrays(queues, queues2,
                             [](const std::string &a, const std::string &a2, size_t i, double tol) {
                               return a == a2;
                             });
  ///@todo more verbose operator
}

TEST_F(RabbitMqRestServiceTest, BindQueueToExchange) {

  EXPECT_TRUE(m_service_ptr_->BindQueueToExchange(vhost_, qq_, exch_, r_key_));

  auto bindings = m_service_ptr_->GetQueueBindings(vhost_, qq_);

  auto qb = queueBinding(exch_, r_key_);

  EXPECT_TRUE(std::find(bindings.begin(), bindings.end(), qb) != bindings.end());
}

TEST_F(RabbitMqRestServiceTest, BindQueueToExchange_NoExchangeFound) {

  EXPECT_EXCEPTION_WITH_ARGS(m_service_ptr_->BindQueueToExchange(vhost_,
                                                                 qq_,
                                                                 "nonExistentExhc",
                                                                 r_key_),
                             shared::HttpError,
                             std::make_tuple(404,
                                             "{\"error\":\"not_found\",\"reason\":\"no exchange 'nonExistentExhc' in vhost '/'\"}"));

}

TEST_F(RabbitMqRestServiceTest, BindQueueToExchange_NoQueueFound) {

  EXPECT_EXCEPTION_WITH_ARGS(m_service_ptr_->BindQueueToExchange(vhost_,
                                                                 "qdwwcwedc",
                                                                 exch_,
                                                                 r_key_),
                             shared::HttpError,
                             std::make_tuple(404,
                                             "{\"error\":\"not_found\",\"reason\":\"no queue 'qdwwcwedc' in vhost '/'\"}"));

}

TEST_F(RabbitMqRestServiceTest, UnbindQueueFromExchange) {

  EXPECT_TRUE(m_service_ptr_->UnbindQueueFromExchange(vhost_, qq_, exch_, r_key_));

  auto bindings = m_service_ptr_->GetQueueBindings(vhost_, qq_);
  auto qb = queueBinding(exch_, r_key_);

  EXPECT_TRUE(std::find(bindings.begin(), bindings.end(), qb) == bindings.end());
}

TEST_F(RabbitMqRestServiceTest, DeleteQueue) {

  EXPECT_TRUE(m_service_ptr_->DeleteQueue(vhost_, qq_));

  auto queues = m_service_ptr_->ListQueues(vhost_);

  EXPECT_TRUE(std::find(queues.begin(), queues.end(), qq_) == queues.end());
}

TEST_F(RabbitMqRestServiceTest, DeleteQueue_DoesNotExists) {

  std::string nn = "exch2edw";
  EXPECT_EXCEPTION_WITH_ARGS(m_service_ptr_->DeleteQueue(vhost_, nn),
                             shared::HttpError,
                             std::make_tuple(404, "{\"error\":\"Object Not Found\",\"reason\":\"Not Found\"}"));
}

TEST_F(RabbitMqRestServiceTest, CreateUser) {

  EXPECT_TRUE(m_service_ptr_->CreateUser(uname_, pass_));

  auto users = m_service_ptr_->ListUsers(vhost_);

  EXPECT_TRUE(std::find_if(users.begin(), users.end(),
                           [](const rabbitMQUser &a1) {
                             return a1.name == uname_;
                           })
                  != users.end());

}

TEST_F(RabbitMqRestServiceTest, DeleteUser) {
  EXPECT_TRUE(m_service_ptr_->DeleteUser(uname_));

  auto users = m_service_ptr_->ListUsers(vhost_);
  EXPECT_TRUE(std::find_if(users.begin(), users.end(),
                           [](const rabbitMQUser &a1) {
                             return a1.name == uname_;
                           })
                  == users.end());
}

TEST_F(RabbitMqRestServiceTest, CreateExchange) {
  exchange exchange{"exch", "user", AMQP::direct};

  EXPECT_TRUE(m_service_ptr_->CreateExchange(vhost_, exchange, Json::Value{}));

  auto exhc = m_service_ptr_->GetExchanges(vhost_);

  EXPECT_TRUE(std::find_if(exhc.begin(), exhc.end(),
                           [&exchange](const amqp_common::exchange &a1) {
                             return a1.name == exchange.name;
                           })
                  != exhc.end());

}

TEST_F(RabbitMqRestServiceTest, DeleteExchange) {

  std::string nn = "exch";
  EXPECT_TRUE(m_service_ptr_->DeleteExchange(vhost_, nn));

  auto exhc = m_service_ptr_->GetExchanges(vhost_);

  EXPECT_TRUE(std::find_if(exhc.begin(), exhc.end(),
                           [&nn](const amqp_common::exchange &a1) {
                             return a1.name == nn;
                           })
                  == exhc.end());

}

TEST_F(RabbitMqRestServiceTest, DeleteExchange_DoesNotExists) {

  std::string nn = "exch2edw";
  EXPECT_EXCEPTION_WITH_ARGS(m_service_ptr_->DeleteExchange(vhost_, nn),
                             shared::HttpError,
                             std::make_tuple(404,
                                             "{\"error\":\"Object Not Found\",\"reason\":\"Not Found\"}"));
}

TEST_F(RabbitMqRestServiceTest, ListConnectionsNoErroThrown) {

  ASSERT_NO_THROW(m_service_ptr_->ListConnections());
}

TEST_F(RabbitMqRestServiceTest, ListChannelsNoErroThrown) {

  ASSERT_NO_THROW(m_service_ptr_->ListConnections());
}

TEST_F(RabbitMqRestServiceTest, TestCreateGlobalParam) {

  global_param param("value", Json::objectValue);

  m_service_ptr_->GlobalParam(param, HttpMethod::PUT);

  auto list = m_service_ptr_->ListGlobalParams();

  EXPECT_TRUE(std::find_if(list.begin(), list.end(), [&](const auto &item) {
    return item.name == param.name;
  }) != list.end());
}

TEST_F(RabbitMqRestServiceTest, TestDeleteGlobalParam) {

  global_param param("value", Json::objectValue);

  m_service_ptr_->GlobalParam(param, HttpMethod::DELETE);

  auto list = m_service_ptr_->ListGlobalParams();

  EXPECT_TRUE(std::find_if(list.begin(), list.end(), [&](const auto &item) {
    return item.name == param.name;
  }) == list.end());
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  if (argc < 4) {
    std::cerr << "Usage: " << argv[0] << " <host> <login> <password>\n";
    return 1;
  }

  g_serviceParams.host = argv[1];
  g_serviceParams.username = argv[2];
  g_serviceParams.password = argv[3];
  std::cout << g_serviceParams.host << '\t'
            << g_serviceParams.username << '\t'
            << g_serviceParams.password << '\n';

  return RUN_ALL_TESTS();
}
