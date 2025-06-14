#include "network_shared/AMQPConsumerService.h"
#include "network_shared/amqpRestService.h"
#include "testingUtils/GoogleCommon.h"

using test_common::AuthParams;
using test_common::WaitFor;
AuthParams g_serviceParams;

using namespace amqp_common;
class AMQPConsumerSeviceTS : public testing::Test {
 public:

  using GuardType = std::scoped_lock<std::mutex>;
  std::shared_ptr<RabbitMQRestService> rest_service_;

  std::shared_ptr<BasicAuthHandler> hander_;
  std::unique_ptr<AMQPConsumerService> consumer_service_;

  static inline std::mutex mutex_{};

  void SetUp() override {

    hander_ = std::make_shared<BasicAuthHandler>(g_serviceParams.username, g_serviceParams.password);
    rest_service_ = std::make_shared<RabbitMQRestService>(g_serviceParams.host, hander_.get());

    network_types::queue qq{qq_, g_serviceParams.username};

    rest_service_->CreateQueue(vhost_, qq, Json::Value());
    rest_service_->BindQueueToExchange(vhost_, qq_, exch_, qq_);

    consumer_service_ =
        std::make_unique<AMQPConsumerService>(AMQPSQLCStr(ExtractHost(g_serviceParams.host).value(),
                                                               g_serviceParams.username,
                                                               g_serviceParams.password), qq_);

    consumer_service_->SetMessageCallback(
        [this](const AMQP::Message &m, uint64_t i, bool b) {
          const GuardType kGuardType{mutex_};
          stack_.emplace_back(m, i, b);
        });

  }

  void TearDown() override {
    rest_service_->DeleteQueue(vhost_, qq_);
  }

  static inline std::string qq_ = "test_queue";
  static inline std::string exch_ = "amq.direct";
  static inline std::string r_key_ = "test_routing";
  static inline std::string vhost_ = "%2F";

  static inline std::string uname_ = "new_user";
  static inline std::string pass_ = "password";

  std::vector<std::tuple<const AMQP::Message &, uint64_t, bool>> stack_;

  MessageCallback handler_;
};

TEST(AMQP_COMMON_TS, ExtractHost_ValidURL) {
  auto host = ExtractHost("amqp://localhost:5672/");
  ASSERT_TRUE(host.has_value());
  EXPECT_EQ(host.value(), "localhost");
}

TEST(AMQP_COMMON_TS, ExtractHost_InvalidURL) {
  auto host = ExtractHost("no-protocol-host:5672");
  EXPECT_FALSE(host.has_value());
}

TEST(AMQP_COMMON_TS, AMQPSQLCStr_SecureAndNonSecure) {
  std::string h = "rabbit:5672";
  auto nonSecure = AMQPSQLCStr(h, "u", "p", false);
  EXPECT_EQ(nonSecure.to_string(), "amqp://u:p@rabbit:5672/");

  auto secure = AMQPSQLCStr(h, "u", "p", true);
  EXPECT_EQ(secure.to_string(), "amqps://u:p@rabbit:5672/");
}

TEST(AMQP_COMMON_TS, testConstructCaderss) {
  auto a1 = AMQPSQLCStr(ExtractHost(g_serviceParams.host).value(),
                             g_serviceParams.username,
                             g_serviceParams.password);
  auto a2 = ConstructCAddress(ExtractHost(g_serviceParams.host).value(),
                              g_serviceParams.username,
                              g_serviceParams.password);
  EXPECT_EQ(a1.operator std::string_view(), a2.operator std::string());
}

TEST_F(AMQPConsumerSeviceTS, TestConnect) {
  consumer_service_->Connect();

  EXPECT_TRUE(consumer_service_->IsConnected());

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
}

TEST_F(AMQPConsumerSeviceTS, TestProcessMessages) {

  message message_m{qq_, "MyPayload", "string"};

  int t = 10;
  for (int i = 0; i < 10; ++i) {
    rest_service_->PublishMessage(vhost_, exch_, message_m);
  }

  EXPECT_EQ(0, stack_.size());
  consumer_service_->Connect();

  sleep(1);

  EXPECT_EQ(t, stack_.size());

  EXPECT_EQ(rest_service_->GetMessageCount(vhost_, qq_), 0);

}

TEST_F(AMQPConsumerSeviceTS, TestNoMessagesBeforeConnect) {
  consumer_service_->Connect();
  std::this_thread::sleep_for(std::chrono::seconds(1));
  EXPECT_TRUE(stack_.empty());

}

TEST_F(AMQPConsumerSeviceTS, TestDisconnect) {
  consumer_service_->Disconnect();

  EXPECT_FALSE(consumer_service_->IsConnected());

  const bool kFoundConnection = WaitFor([this]() {
    auto cons = rest_service_->ListConnections();
    return std::any_of(cons.begin(), cons.end(), [](const connection &conn) {
      return conn.user == g_serviceParams.username;
    });
  }, std::chrono::seconds(2));

  auto channels = rest_service_->ListChannels();
  const bool kFoundChannel = std::any_of(channels.begin(), channels.end(), [](const channel &ch) {
    return ch.user == g_serviceParams.username;
  });

  EXPECT_FALSE(kFoundConnection);
  EXPECT_FALSE(kFoundChannel);
}

TEST_F(AMQPConsumerSeviceTS, TestReconnectAfterDisconnect) {

  network_types::message msg1{qq_, "MyPayload", "string"};
  rest_service_->PublishMessage(vhost_, exch_, msg1);
  consumer_service_->Connect();
  ASSERT_TRUE(WaitFor([&]() { return !stack_.empty(); }));

  consumer_service_->Disconnect();
  EXPECT_FALSE(consumer_service_->IsConnected());

  stack_.clear();
  network_types::message msg2{qq_, "second", "string"};
  rest_service_->PublishMessage(vhost_, exch_, msg2);

  consumer_service_->Connect();
  ASSERT_TRUE(WaitFor([&]() { return !stack_.empty(); }));

  std::string body(reinterpret_cast<const char *>(std::get<0>(stack_[0]).body()),
                   std::get<0>(stack_[0]).bodySize());
  EXPECT_EQ(body, "second");
}

TEST_F(AMQPConsumerSeviceTS, TestConnectFailureEarlyExit) {
  consumer_service_->SetParameters(AMQPSQLCStr("localhost30","",""), qq_);
  EXPECT_THROW(consumer_service_->Connect(), std::runtime_error);

  const bool kFoundConnection = WaitFor([this]() {
    auto cons = rest_service_->ListConnections();
    return std::any_of(cons.begin(), cons.end(), [](const connection &conn) {
      return conn.user == g_serviceParams.username;
    });
  }, std::chrono::seconds(2));

  auto channels = rest_service_->ListChannels();
  const bool kFoundChannel = std::any_of(channels.begin(), channels.end(), [](const channel &ch) {
    return ch.user == g_serviceParams.username;
  });

  EXPECT_FALSE(kFoundConnection);
  EXPECT_FALSE(kFoundChannel);
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