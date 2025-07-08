#include "testingUtils/GoogleCommon.h"
#include <cstdlib>
#include <filesystem>
#include "network_shared/amqpRestService.h"
#include "network_shared/amqpCommon.h"
#include <unordered_set>
using namespace test_common;
AuthParams g_serviceParams;

namespace bp = boost::process;
using namespace rest_utils;
using namespace amqp_common;

static std::string qq_ = "test_queue";
static std::string vhost_ = "%2F";
static size_t d_port = 8081;

class CompNodeFixture: public ::testing::Test {
  using ChildSeT=std::unordered_set<ChildProcess>;
 public:
  ChildSeT child_set_;

  void EmplaceChild(size_t port=d_port,bool runs_real_benchmark= false)
  {
    child_set_.emplace(comp_node, fmt::format_int(port).c_str(), fmt::format_int(!runs_real_benchmark).c_str(),
                       bp::start_dir(compNodeBin.string()), bp::std_out > stdout, bp::std_err > stderr);

  }

 protected:


  void SetUp() override {
    EmplaceChild();
    requestor_->SetParams(fmt::format("http://localhost:{}",d_port));

    SLEEP(std::chrono::milliseconds(100));
  }

  void TearDown() override {

  }

  static void SetUpTestSuite()
  {
    comp_node="./compNode";
    requestor_=std::make_unique<HttpRequestService>();
    compNodeBin = std::filesystem::absolute(".");
    hander_ = std::make_shared<BasicAuthHandler>(g_serviceParams.username, g_serviceParams.password);
    amqp_service_ = std::make_unique<RabbitMQRestService>(g_serviceParams.host, hander_.get());
    amqp_service_->CreateQueue(vhost_,network_types::queue{qq_,g_serviceParams.username});

    body=Json::Value();
    body["ip"]=ExtractHost(g_serviceParams.host).value();
    body["name"]=qq_;
    body["user"]=g_serviceParams.username;
    body["password"]=g_serviceParams.password;

  }
  static void TearDownTestSuite()
  {
    
  }
  HttpResult r;
  static inline std::string comp_node;
  static inline std::unique_ptr<HttpRequestService> requestor_;
  static inline std::unique_ptr<RabbitMQRestService> amqp_service_;
  static inline std::shared_ptr<BasicAuthHandler> hander_;///< is used for rabbimq service to connect
  static inline std::filesystem::path compNodeBin;
  static inline Json::Value body; ///< contains a body that is valid for connection request
};

TEST_F(CompNodeFixture, TestService_Status_Default)
{
  SLEEP(std::chrono::milliseconds(100));

  EXPECT_NO_THROW(r=requestor_->PerformRequest("/v1/status",HttpMethod::GET));

  EXPECT_EQ(r.first,200)<<fmt::format("Invalid response code: expected {}, got {} !",200,r.first);
  auto json=ParseJson(r.second);

  EXPECT_STREQ(json["request"].asCString(),"status");
  EXPECT_STREQ(json["worker_status"].asCString(),"not running");
}

TEST_F(CompNodeFixture, TestService_Connect_Sucess)
{
  EXPECT_NO_THROW(r=requestor_->PerformRequest("/v1/Connect",HttpMethod::POST,body.toStyledString()));

  EXPECT_EQ(r.first,200)<<fmt::format("Invalid response code: expected {}, got {} !",200,r.first);

  EXPECT_NO_THROW(r=requestor_->PerformRequest("/v1/status",HttpMethod::GET));

  EXPECT_EQ(r.first,200)<<fmt::format("Invalid response code: expected {}, got {} !",200,r.first);
  auto json=ParseJson(r.second);

  EXPECT_STREQ(json["worker_status"].asCString(),"running");
  EXPECT_TRUE(json.isMember("connected_to"));
}

TEST_F(CompNodeFixture, TestService_Connect_TooEarly)
{
  EmplaceChild(8082, true);
  requestor_->SetParams(fmt::format("http://localhost:{}",8082));
  SLEEP(std::chrono::milliseconds(100));


  shared::HttpError sl(78,"s");
  EXPECT_EXCEPTION_WITH_CHECKS(
      shared::HttpError,
      r = requestor_->PerformRequest("/v1/Connect", HttpMethod::POST, body.toStyledString()),
      {
        EXPECT_EQ(e.get<0>(), 409);
        auto json = ParseJson(e.get<1>());
        EXPECT_STREQ(json["message"].asCString(), "Unable to connect node. Wait until benchmark result is computed!");
      }
  );

}

TEST_F(CompNodeFixture, TestService_Connect_Repeated)
{

  EXPECT_NO_THROW(r=requestor_->PerformRequest("/v1/Connect",HttpMethod::POST,body.toStyledString()));

  EXPECT_EQ(r.first,200)<<fmt::format("Invalid response code: expected {}, got {} !",200,r.first);


  EXPECT_EXCEPTION_WITH_CHECKS(
      shared::HttpError,
      r = requestor_->PerformRequest("/v1/Connect", HttpMethod::POST, body.toStyledString()),
      {
        EXPECT_EQ(e.get<0>(), 409);
        auto json = ParseJson(e.get<1>());
        EXPECT_STR_CONTAINS(json["message"].asCString(),"Node is already connected to RabbitMQ");
      }
  );

}

TEST_F(CompNodeFixture, TestService_Connect_InvalidQService)
{

  auto invalid_body=body;

  invalid_body["ip"]="some_invalid_ip";
  EXPECT_EXCEPTION_WITH_CHECKS(
      shared::HttpError,
      r = requestor_->PerformRequest("/v1/Connect", HttpMethod::POST, invalid_body.toStyledString()),
      {
        EXPECT_EQ(e.get<0>(), 409);
        auto json = ParseJson(e.get<1>());
        EXPECT_STR_CONTAINS(json["message"].asCString(),"Queue connection error ms");
      }
  );
}

TEST_F(CompNodeFixture, TestService_Disconnect_Sucess)
{
  EXPECT_NO_THROW(r=requestor_->PerformRequest("/v1/Connect",HttpMethod::POST,body.toStyledString()));


  EXPECT_NO_THROW(r=requestor_->PerformRequest("/v1/Disconnect",HttpMethod::POST));

  EXPECT_NO_THROW(r=requestor_->PerformRequest("/v1/status",HttpMethod::GET));

  EXPECT_EQ(r.first,200)<<fmt::format("Invalid response code: expected {}, got {} !",200,r.first);
  auto json=ParseJson(r.second);

  EXPECT_FALSE(json.isMember("connected_to"));

  EXPECT_STREQ(json["worker_status"].asCString(),"not running");
}

TEST_F(CompNodeFixture, TestService_Disconnect_Repeated)
{

  EXPECT_EXCEPTION_WITH_CHECKS(
      shared::HttpError,
      r = requestor_->PerformRequest("/v1/Disconnect", HttpMethod::POST),
      {
        EXPECT_EQ(e.get<0>(), 409);
        auto json = ParseJson(e.get<1>());
        EXPECT_STREQ(json["message"].asCString(),"This worker node is currently not connected to any cluster!");
      }
  );
}


TEST_F(CompNodeFixture, TestService_Rebalance_Sucess)
{
  SLEEP(std::chrono::milliseconds(200));
  r = requestor_->PerformRequest("/v1/rebalance_node", HttpMethod::POST);
  auto json =ParseJson(r.second);
  EXPECT_STREQ(json["request"].asCString(),"rebalance_node");
  ASSERT_TRUE(json.isMember("old_bench"));
}

TEST_F(CompNodeFixture, TestService_Rebalance_TooSoon)
{

  EmplaceChild(8082, true);
  requestor_->SetParams(fmt::format("http://localhost:{}",8082));
  SLEEP(std::chrono::milliseconds(100));
  EXPECT_EXCEPTION_WITH_CHECKS(
      shared::HttpError,
      r = requestor_->PerformRequest("/v1/rebalance_node", HttpMethod::POST),
      {
        EXPECT_EQ(e.get<0>(), 409);
        auto json = ParseJson(e.get<1>());
        EXPECT_STREQ(json["message"].asCString(),"Rabalancing is already running try again later!");
      }
  );
}

TEST_F(CompNodeFixture, MainNodeService_soft_terminate_Sucess)
{


  EXPECT_NO_THROW(r=requestor_->PerformRequest("v1/soft_terminate",HttpMethod::POST));

  EXPECT_EQ(r.first,200);

  EXPECT_THROW(r=requestor_->PerformRequest("v1/status",HttpMethod::POST),shared::CurlError);
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