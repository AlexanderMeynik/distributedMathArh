#include <gtest/gtest.h>

#include "network_shared/AMQPConsumerService.h"
#include "network_shared/AMQPPublisherService.h"
#include "network_shared/amqpRestService.h"
#include "../GoogleCommon.h"

using test_common::AuthParams;

AuthParams g_serviceParams;

using namespace amqp_common;
class AMQPConsumerSeviceIntegratedTs : public testing::Test {
 public:

  std::shared_ptr<RabbitMQRestService> m_service_ptr_;

  std::shared_ptr<BasicAuthHandler> hander_;
  std::unique_ptr<AMQPConsumerService> consumer_service_;

  void SetUp() override {

    hander_ = std::make_shared<BasicAuthHandler>(g_serviceParams.username, g_serviceParams.password);
    m_service_ptr_ = std::make_shared<RabbitMQRestService>(g_serviceParams.host, hander_.get());

    network_types::queue qq{qq_,g_serviceParams.username};

    m_service_ptr_->CreateQueue(vhost_,qq,Json::Value());
    m_service_ptr_->BindQueueToExchange(vhost_,qq_,exch_,qq_);


    consumer_service_=std::make_unique<AMQPConsumerService>(ConstructCString(ExtractHost(g_serviceParams.host).value(),
                                                                            g_serviceParams.username,
                                                                             g_serviceParams.password),qq_);

    consumer_service_->SetMessageCallback(
        [this](const AMQP::Message &m,uint64_t i,bool b)
        {
          stack_.emplace_back(m,i,b);
        });


  }

  void TearDown() override {
    m_service_ptr_->DeleteQueue(vhost_,qq_);
  }

  static inline std::string qq_ = "test_queue";
  static inline std::string exch_ = "amq.direct";
  static inline std::string r_key_ = "test_routing";
  static inline std::string vhost_ = "%2F";

  static inline std::string uname_ = "new_user";
  static inline std::string pass_ = "password";

  std::vector<std::tuple<const AMQP::Message &,uint64_t,bool>> stack_;

  MessageCallback handler_;
};
TEST_F(AMQPConsumerSeviceIntegratedTs,TestConnect)
{
  consumer_service_->Connect();

  sleep(2);

  auto cons=m_service_ptr_->ListConnections();
  auto channels=m_service_ptr_->ListChannels();

  EXPECT_TRUE(std::any_of(channels.begin(), channels.end(),[](const channel&channel)
                          {
                            return channel.user==g_serviceParams.username;
                          }
  ));
  EXPECT_TRUE(std::any_of(cons.begin(), cons.end(),[](const connection&conn)
  {
    return conn.user==g_serviceParams.username;
  }
  ));
}

TEST_F(AMQPConsumerSeviceIntegratedTs,TestProcessMessages)
{
  message message_m{qq_,"MyPayload","string"};

  int t=10;
  for (int i = 0; i < 10; ++i) {
    m_service_ptr_->PublishMessage(vhost_,exch_,message_m);//todo C++ exception with description "HTTP error: code 500 , reason "" !" thrown in the test body.
  }
  EXPECT_EQ(t,stack_.size());

  EXPECT_EQ(m_service_ptr_->GetMessageCount(vhost_,qq_),0);


}

TEST_F(AMQPConsumerSeviceIntegratedTs,TestDisConnect)
{
  consumer_service_->Disconnect();

  auto cons=m_service_ptr_->ListConnections();
  auto channels=m_service_ptr_->ListChannels();

  EXPECT_TRUE(std::none_of(channels.begin(), channels.end(),[](const channel&channel)
                          {
                            return channel.user==uname_;
                          }
  ));
  EXPECT_TRUE(std::none_of(cons.begin(), cons.end(),[](const connection&conn)
                          {
                            return conn.user==uname_;
                          }
  ));
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