#include <gtest/gtest.h>

#include "network_shared/AMQPPublisherService.h"
#include "network_shared/amqpRestService.h"
#include "network_shared/amqpCommon.h"
#include "../GoogleCommon.h"

using test_common::AuthParams;
using test_common::WaitFor;

AuthParams g_serviceParams;

using namespace amqp_common;

class AMQPPublisherServiceTS : public testing::Test {
 public:
  std::shared_ptr<RabbitMQRestService> rest_service_;
  std::shared_ptr<BasicAuthHandler> authHandler;
  std::unique_ptr<AMQPPublisherService> publisher_service_;

  static inline std::string queue1 = "pub_test_q1";
  static inline std::string queue2 = "pub_test_q2";
  static inline std::string exchange = "amq.direct";
  static inline std::string vhost = "%2F";

  void SetUp() override {
    authHandler = std::make_shared<BasicAuthHandler>(g_serviceParams.username,
                                                     g_serviceParams.password);
    rest_service_ = std::make_shared<RabbitMQRestService>(g_serviceParams.host,
                                                          authHandler.get());

    network_types::queue q1{queue1, g_serviceParams.username};
    network_types::queue q2{queue2, g_serviceParams.username};
    rest_service_->CreateQueue(vhost, q1, Json::Value());
    rest_service_->CreateQueue(vhost, q2, Json::Value());

    rest_service_->BindQueueToExchange(vhost, q1.name, exchange, q1.name);
    rest_service_->BindQueueToExchange(vhost, q2.name, exchange, q2.name);
    publisher_service_ = std::make_unique<AMQPPublisherService>();
  }

  void TearDown() override {
    rest_service_->DeleteQueue(vhost, queue1);
    rest_service_->DeleteQueue(vhost, queue2);
  }
};

TEST_F(AMQPPublisherServiceTS, DefaultConstructorAndSetParameters) {

  EXPECT_FALSE(publisher_service_->IsConnected());
  publisher_service_->SetParameters(
      ConstructCString(ExtractHost(g_serviceParams.host).value(),
                       g_serviceParams.username, g_serviceParams.password),
      std::vector<std::string>{queue1, queue2}, exchange);
  EXPECT_EQ(publisher_service_->GetDefaultExchange(), exchange);
}

TEST_F(AMQPPublisherServiceTS, ConstructorWithParams) {
  publisher_service_ = std::make_unique<AMQPPublisherService>(
      ConstructCString(ExtractHost(g_serviceParams.host).value(),
                       g_serviceParams.username, g_serviceParams.password),
      std::vector<std::string>{queue1}, exchange);
  EXPECT_EQ(publisher_service_->GetDefaultExchange(), exchange);
}

TEST_F(AMQPPublisherServiceTS, AddRemoveQueue) {
  publisher_service_->SetParameters(
      ConstructCString(ExtractHost(g_serviceParams.host).value(),
                       g_serviceParams.username, g_serviceParams.password),
      std::vector<std::string>{queue1}, exchange);

  publisher_service_->Connect();
  EXPECT_TRUE(publisher_service_->IsConnected());

  bool connOk = WaitFor([&]() {
    auto conns = rest_service_->ListConnections();
    return std::any_of(conns.begin(), conns.end(), [&](const connection &c) {
      return c.user == g_serviceParams.username;
    });
  });
  bool chanOk = WaitFor([&]() {
    auto chs = rest_service_->ListChannels();
    return std::any_of(chs.begin(), chs.end(), [&](const channel &c) {
      return c.user == g_serviceParams.username;
    });
  });
  EXPECT_TRUE(connOk);
  EXPECT_TRUE(chanOk);

  EXPECT_THROW(publisher_service_->RemoveQueue(5), shared::outOfRange);

}

TEST_F(AMQPPublisherServiceTS, PublishByIndexAndName) {
  publisher_service_ = std::make_unique<AMQPPublisherService>(
      ConstructCString(ExtractHost(g_serviceParams.host).value(),
                       g_serviceParams.username, g_serviceParams.password),
      std::vector<std::string>{queue1, queue2}, exchange);
  publisher_service_->Connect();
  EXPECT_TRUE(publisher_service_->IsConnected());

  auto makeMsg = [](const std::string &body) {

    auto envelope = std::make_shared<AMQP::Envelope>(body);

    envelope->setPersistent(true);
    envelope->setPriority(1);
    AMQP::Table headers;
    headers["messageNum"] = rand() % 1000;
    headers["tt"] = "header1";

    envelope->setHeaders(headers);

    envelope->setContentType("string");
    return envelope;
  };

  publisher_service_->Publish(makeMsg("msg1"), 0);

  publisher_service_->Publish(makeMsg("msg2"), queue2);

  EXPECT_TRUE(WaitFor([&] { return rest_service_->GetMessageCount(vhost, queue1) == 1; }));
  EXPECT_TRUE(WaitFor([&] { return rest_service_->GetMessageCount(vhost, queue2) == 1; }));
}

TEST_F(AMQPPublisherServiceTS, PublishOutOfRangeThrows) {
  publisher_service_ = std::make_unique<AMQPPublisherService>(
      ConstructCString(ExtractHost(g_serviceParams.host).value(),
                       g_serviceParams.username, g_serviceParams.password),
      std::vector<std::string>{queue1}, exchange);
  publisher_service_->Connect();
  auto env = std::make_shared<Envelope>("x");
  env->setContentType("string");
  EXPECT_THROW(publisher_service_->Publish(env, 5), shared::outOfRange);
}

TEST_F(AMQPPublisherServiceTS, DisconnectCleanup) {
  {
    AMQPPublisherService tempPub(
        ConstructCString(ExtractHost(g_serviceParams.host).value(),
                         g_serviceParams.username, g_serviceParams.password),
        std::vector<std::string>{queue1}, exchange);
    tempPub.Connect();
    EXPECT_TRUE(tempPub.IsConnected());
  }
  bool connGone = WaitFor([&] {
    auto conns = rest_service_->ListConnections();
    return !std::any_of(conns.begin(), conns.end(), [&](auto &c) { return c.user == g_serviceParams.username; });
  });
  EXPECT_TRUE(connGone);
}

TEST_F(AMQPPublisherServiceTS, ConnectFailureThrows) {
  publisher_service_ = std::make_unique<AMQPPublisherService>();
  publisher_service_->SetParameters("bad_host:9999", {queue1}, exchange);
  EXPECT_THROW(publisher_service_->Connect(), std::runtime_error);
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
  return RUN_ALL_TESTS();
}