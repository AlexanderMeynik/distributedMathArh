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
static std::string exch_ = "amq.direct";
static std::string r_key_ = "test_routing";
static std::string vhost_ = "%2F";
static size_t d_port = 8080;
static std::string uname_ = "new_user";
static std::string pass_ = "password";

class MainNodeTs: public ::testing::Test {
  using ChildSeT=std::unordered_set<ChildProcess>;
 public:
  ChildSeT child_set_;

  auto RunCompNode(size_t port= d_port, bool runs_real_benchmark= false)
  {
    return child_set_.emplace(comp_node, fmt::format_int(port).c_str(), fmt::format_int(!runs_real_benchmark).c_str(),
                       bp::start_dir(compNodeBin.string()), bp::std_out > stdout, bp::std_err > stderr);

  }

 protected:
  void RunMainNode()
  {
    child_set_.emplace(main_node,
                       bp::start_dir(mainNodeBin.string()), bp::std_out > stdout, bp::std_err > stderr);

  }

  void SetUp() override {
    RunMainNode();
    requestor_->SetParams(fmt::format("http://{}:{}",comp_node_host,d_port));

    SLEEP(std::chrono::milliseconds(100));
  }

  void TearDown() override {

  }

  static void SetUpTestSuite()
  {
    comp_node="./compNode";
    main_node="./mainNode";
    requestor_=std::make_unique<HttpRequestService>();
    mainNodeBin = std::filesystem::absolute("../bin");
    compNodeBin = std::filesystem::absolute("../../computationalNode/bin");
    hander_ = std::make_shared<BasicAuthHandler>(g_serviceParams.username, g_serviceParams.password);
    amqp_service_ = std::make_unique<RabbitMQRestService>(g_serviceParams.host, hander_.get());
    amqp_service_->CreateQueue(vhost_,network_types::queue{qq_,g_serviceParams.username});

    host =ExtractHost(g_serviceParams.host).value();
    comp_node_host="localhost";
    body=Json::Value();
    body["ip"]=comp_node_host;
    body["name"]=qq_;
    body["user"]=g_serviceParams.username;
    body["password"]=g_serviceParams.password;

    connect_publisher_body=Json::Value();
    connect_publisher_body["queue_host"]=host;
    connect_publisher_body["user"]=g_serviceParams.username;
    connect_publisher_body["password"]=g_serviceParams.password;

  }
  static void TearDownTestSuite()
  {

  }
  HttpResult r;
  static inline std::string comp_node;
  static inline std::string main_node;
  static inline std::unique_ptr<HttpRequestService> requestor_;
  static inline std::unique_ptr<RabbitMQRestService> amqp_service_;
  static inline std::shared_ptr<BasicAuthHandler> hander_;///< is used for rabbimq service to connect
  static inline std::filesystem::path compNodeBin;
  static inline std::filesystem::path mainNodeBin;
  static inline std::string host;
  static inline std::string comp_node_host;
  static inline Json::Value body; ///< contains a body that is valid for connection request
  static inline Json::Value connect_publisher_body; ///< contains a body that is valid for connection request

};


TEST_F(MainNodeTs, MainNodeService_Status_Default)
{
  r=requestor_->PerformRequest("/v1/status",HttpMethod::GET);
  auto json=RabbitMQRestService::ParseJson(r.second);
  EXPECT_EQ(r.first,200);
  EXPECT_STREQ(json["rabbitmq_service"]["status"].asCString(),"Not Connected");
}


TEST_F(MainNodeTs, MainNodeService_ConnectPublisher_Sucess)
{
  EXPECT_NO_THROW(r=requestor_->PerformRequest("v1/connect_publisher",HttpMethod::POST,
                                               connect_publisher_body.toStyledString()));

  EXPECT_EQ(r.first,200);
  r=requestor_->PerformRequest("/v1/status",HttpMethod::GET);

  auto json=RabbitMQRestService::ParseJson(r.second);

  EXPECT_STREQ(json["rabbitmq_service"]["status"].asCString(),"Connected");
  EXPECT_TRUE(json["rabbitmq_service"].isMember("c_string"));
}

TEST_F(MainNodeTs, MainNodeService_ConnectPublisher_Repeated)
{
  EXPECT_NO_THROW(r=requestor_->PerformRequest("v1/connect_publisher",HttpMethod::POST,
                                               connect_publisher_body.toStyledString()));
  EXPECT_EXCEPTION_WITH_CHECKS(
      shared::HttpError,
      r=requestor_->PerformRequest("v1/connect_publisher",HttpMethod::POST,
                                   connect_publisher_body.toStyledString()),
      {
        EXPECT_EQ(e.get<0>(), 409);
        //todo message make more verbose
        auto json = HttpRequestService::ParseJson(e.get<1>());
        EXPECT_STR_CONTAINS(json["message"].asCString(), "Queue service is currently working");
      }
  );
}

TEST_F(MainNodeTs, MainNodeService_ConnectPublisher_UnableToConnect)
{
  auto body=connect_publisher_body;
  body["queue_host"]="invalid_ip";
  EXPECT_EXCEPTION_WITH_CHECKS(
      shared::HttpError,
      r=requestor_->PerformRequest("v1/connect_publisher",HttpMethod::POST,
                                   body.toStyledString()),
      {
        EXPECT_EQ(e.get<0>(), 409);
        auto json = HttpRequestService::ParseJson(e.get<1>());
        EXPECT_STREQ(json["message"].asCString(), "Error during http curl request");
      }
  );
}


TEST_F(MainNodeTs, MainNodeServiceConnectDisconnectPublisher_Sucess)
{
  EXPECT_NO_THROW(r=requestor_->PerformRequest("v1/connect_publisher",HttpMethod::POST,
                                               connect_publisher_body.toStyledString()));

  EXPECT_NO_THROW(r=requestor_->PerformRequest("v1/disconnect_publisher",HttpMethod::POST));
  EXPECT_EQ(r.first,200);
  r=requestor_->PerformRequest("/v1/status",HttpMethod::GET);

  auto json=RabbitMQRestService::ParseJson(r.second);

  EXPECT_STREQ(json["rabbitmq_service"]["status"].asCString(),"Not Connected");
  EXPECT_FALSE(json["rabbitmq_service"].isMember("c_string"));
}

TEST_F(MainNodeTs, MainNodeService_DisconnectPublisher_Repeated)
{
  EXPECT_EXCEPTION_WITH_CHECKS(
      shared::HttpError,
      r=requestor_->PerformRequest("v1/disconnect_publisher",HttpMethod::POST),
      {
        EXPECT_EQ(e.get<0>(), 409);
        auto json = HttpRequestService::ParseJson(e.get<1>());
        EXPECT_STREQ(json["message"].asCString(), "Queue service is currently unavailable try using connect_publisher/ request");
      }
  );
}

TEST_F(MainNodeTs, MainNodeService_ConnectNode_PublisherNotConnected)
{
  EXPECT_EXCEPTION_WITH_CHECKS(
      shared::HttpError,
      r=requestor_->PerformRequest(fmt::format("v1/connect_node?ip={}:{}",comp_node_host,8081),HttpMethod::POST),
      {
        EXPECT_EQ(e.get<0>(), 409);
        auto json = HttpRequestService::ParseJson(e.get<1>());
        EXPECT_STREQ(json["message"].asCString(), "Queue service is currently unavailable try using connect_publisher/ request");
      }
  );
}


TEST_F(MainNodeTs, MainNodeService_ConnectNode_NodeNotRunning)
{

  EXPECT_NO_THROW(r=requestor_->PerformRequest("v1/connect_publisher",HttpMethod::POST,
                                               connect_publisher_body.toStyledString()));

  EXPECT_EXCEPTION_WITH_CHECKS(
      shared::HttpError,
      r=requestor_->PerformRequest(fmt::format("v1/connect_node?ip={}:{}",comp_node_host,8081),HttpMethod::POST),
      {
        EXPECT_EQ(e.get<0>(), 504);
        auto json = HttpRequestService::ParseJson(e.get<1>());
        EXPECT_TRUE(json.isMember("balancer_output"));
        EXPECT_STR_CONTAINS(json["balancer_output"]["message"].asCString(), "Unable to access worker on");
      }
  );
}

TEST_F(MainNodeTs, MainNodeService_ConnectNode_Sucess)
{

  this->RunCompNode(8081);
  EXPECT_NO_THROW(r=requestor_->PerformRequest("v1/connect_publisher",HttpMethod::POST,
                                               connect_publisher_body.toStyledString()));


  SLEEP(std::chrono::milliseconds(100));

  EXPECT_NO_THROW(r=requestor_->PerformRequest(fmt::format("v1/connect_node?ip={}:{}",comp_node_host,8081),HttpMethod::POST));
  EXPECT_EQ(r.first,200);

  r=requestor_->PerformRequest("/v1/status",HttpMethod::GET);

  auto json=RabbitMQRestService::ParseJson(r.second);
  ASSERT_EQ(json["clients"][0]["data"].size(),1);
  EXPECT_STREQ(json["clients"][0]["data"][0]["host"].asCString(),fmt::format("{}:{}",host,8081).c_str());
}



TEST_F(MainNodeTs, MainNodeService_DisconnectNode_NodeSucess)
{

  this->RunCompNode(8081);
  EXPECT_NO_THROW(r=requestor_->PerformRequest("v1/connect_publisher",HttpMethod::POST,
                                               connect_publisher_body.toStyledString()));


  SLEEP(std::chrono::milliseconds(100));

  EXPECT_NO_THROW(r=requestor_->PerformRequest(fmt::format("v1/connect_node?ip={}:{}",comp_node_host,8081),HttpMethod::POST));
  EXPECT_EQ(r.first,200);

  EXPECT_NO_THROW(r=requestor_->PerformRequest(fmt::format("v1/disconnect_node?ip={}:{}",comp_node_host,8081),HttpMethod::POST));

   r=requestor_->PerformRequest("/v1/status",HttpMethod::GET);

   auto json=RabbitMQRestService::ParseJson(r.second);
   ASSERT_EQ(json["clients"][0]["data"].size(),0);
}

TEST_F(MainNodeTs, MainNodeService_DisconnectNode_PublisherNotConnected)
{

  EXPECT_EXCEPTION_WITH_CHECKS(
      shared::HttpError,
      r=requestor_->PerformRequest(fmt::format("v1/disconnect_node?ip={}:{}",comp_node_host,8081),HttpMethod::POST),
      {
        EXPECT_EQ(e.get<0>(), 409);
        auto json = HttpRequestService::ParseJson(e.get<1>());
        EXPECT_STREQ(json["message"].asCString(), "Queue service is currently unavailable try using connect_publisher/ request");
      }
  );
}


TEST_F(MainNodeTs, MainNodeService_DisconnectNode_UnableToAcessWorkerNode)
{

  auto it=this->RunCompNode(8081).first;
  EXPECT_NO_THROW(r=requestor_->PerformRequest("v1/connect_publisher",HttpMethod::POST,
                                               connect_publisher_body.toStyledString()));


  SLEEP(std::chrono::milliseconds(200));

  EXPECT_NO_THROW(r=requestor_->PerformRequest(fmt::format("v1/connect_node?ip={}:{}",comp_node_host,8081),HttpMethod::POST));
  EXPECT_EQ(r.first,200);

  child_set_.erase(it);

  EXPECT_EXCEPTION_WITH_CHECKS(
      shared::HttpError,
      r=requestor_->PerformRequest(fmt::format("v1/disconnect_node?ip={}:{}",comp_node_host,8081),HttpMethod::POST),
      {
        EXPECT_EQ(e.get<0>(), 504);
        auto json = HttpRequestService::ParseJson(e.get<1>());
        EXPECT_TRUE(json.isMember("balancer_output"));
        EXPECT_STR_CONTAINS(json["balancer_output"]["message"].asCString(), "Unable to access worker on");
      }
  );
}

TEST_F(MainNodeTs, MainNodeService_DisconnectNode_NodeWasNotConnected)
{

  this->RunCompNode(8081);
  EXPECT_NO_THROW(r=requestor_->PerformRequest("v1/connect_publisher",HttpMethod::POST,
                                               connect_publisher_body.toStyledString()));


  SLEEP(std::chrono::milliseconds(100));

  EXPECT_EXCEPTION_WITH_CHECKS(
      shared::HttpError,
      r=requestor_->PerformRequest(fmt::format("v1/disconnect_node?ip={}:{}",comp_node_host,8081),HttpMethod::POST),
      {
        EXPECT_EQ(e.get<0>(), 409);
        auto json = HttpRequestService::ParseJson(e.get<1>());
        EXPECT_TRUE(json.isMember("balancer_output"));
        EXPECT_STR_CONTAINS(json["balancer_output"]["message"].asCString(), "was not connected to cluster!");
      }
  );
}


TEST_F(MainNodeTs, MainNodeService_Rebalance_PublisherNotConnected)
{
  EXPECT_EXCEPTION_WITH_CHECKS(
      shared::HttpError,
      r=requestor_->PerformRequest("v1/rebalance",HttpMethod::POST),
      {
        EXPECT_EQ(e.get<0>(), 409);
        auto json = HttpRequestService::ParseJson(e.get<1>());
        EXPECT_STREQ(json["message"].asCString(), "Queue service is currently unavailable try using connect_publisher/ request");
      }
  );
}

TEST_F(MainNodeTs, MainNodeService_Rebalance_NoNodes)
{

  EXPECT_NO_THROW(r=requestor_->PerformRequest("v1/connect_publisher",HttpMethod::POST,
                                               connect_publisher_body.toStyledString()));



  EXPECT_EXCEPTION_WITH_CHECKS(
      shared::HttpError,
      r=requestor_->PerformRequest("v1/rebalance",HttpMethod::POST),
      {
        EXPECT_EQ(e.get<0>(), 409);
        auto json = HttpRequestService::ParseJson(e.get<1>());
        EXPECT_STREQ(json["message"].asCString(), "No nodes to be rebalanced");
      }
  );
}


TEST_F(MainNodeTs, MainNodeService_Rebalance_UnableToAcessWotkerNode)
{


  auto it=this->RunCompNode(8081).first;
  auto node_name=fmt::format("{}:{}",host,8081);
  EXPECT_NO_THROW(r=requestor_->PerformRequest("v1/connect_publisher",HttpMethod::POST,
                                               connect_publisher_body.toStyledString()));


  SLEEP(std::chrono::milliseconds(100));

  EXPECT_NO_THROW(r=requestor_->PerformRequest(fmt::format("v1/connect_node?ip={}:{}",comp_node_host,8081),HttpMethod::POST));
  EXPECT_EQ(r.first,200);

  child_set_.erase(it);


  EXPECT_EXCEPTION_WITH_CHECKS(
      shared::HttpError,
      r=requestor_->PerformRequest("v1/rebalance",HttpMethod::POST),
      {
        EXPECT_EQ(e.get<0>(), 504);
        auto json = HttpRequestService::ParseJson(e.get<1>());
        EXPECT_TRUE(json.isMember("nodes"));
        EXPECT_TRUE(json["nodes"].isMember(node_name));
        EXPECT_STR_CONTAINS(json["nodes"][node_name]["message"].asCString(), "Unable to access worker on");
      }
  );
}


TEST_F(MainNodeTs, MainNodeService_Rebalance_Sucess)
{

  this->RunCompNode(8081);
  auto node_name=fmt::format("{}:{}",host,8081);
  EXPECT_NO_THROW(r=requestor_->PerformRequest("v1/connect_publisher",HttpMethod::POST,
                                               connect_publisher_body.toStyledString()));


  SLEEP(std::chrono::milliseconds(200));

  EXPECT_NO_THROW(r=requestor_->PerformRequest(fmt::format("v1/connect_node?ip={}:{}",comp_node_host,8081),HttpMethod::POST));

  EXPECT_NO_THROW(r=requestor_->PerformRequest("v1/rebalance",HttpMethod::POST));
  EXPECT_EQ(r.first,200);
}


TEST_F(MainNodeTs, MainNodeService_soft_terminate_Sucess)
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