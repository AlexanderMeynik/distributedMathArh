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

class ProcessRunningFixture2: public ::testing::Test {
  using ChildSeT=std::unordered_set<ChildProcess>;
 public:
  ChildSeT child_set_;

  void RunCompNode(size_t port= d_port, bool runs_real_benchmark= false)
  {
    child_set_.emplace(comp_node, fmt::format_int(port).c_str(), fmt::format_int(!runs_real_benchmark).c_str(),
                       bp::start_dir(compNodeBin.string()), bp::std_out > stdout, bp::std_err > stderr);

  }

  void RunMainNode()
  {
    child_set_.emplace(main_node,
                       bp::start_dir(mainNodeBin.string()), bp::std_out > stdout, bp::std_err > stderr);

  }

 protected:


  void SetUp() override {
    RunMainNode();
    requestor_->SetParams(fmt::format("http://localhost:{}",d_port));

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
  static inline std::string main_node;
  static inline std::unique_ptr<HttpRequestService> requestor_;
  static inline std::unique_ptr<RabbitMQRestService> amqp_service_;
  static inline std::shared_ptr<BasicAuthHandler> hander_;///< is used for rabbimq service to connect
  static inline std::filesystem::path compNodeBin;
  static inline std::filesystem::path mainNodeBin;
  static inline Json::Value body; ///< contains a body that is valid for connection request
};


TEST_F(ProcessRunningFixture2,_1)
{
  ASSERT_TRUE(true);
  r=requestor_->PerformRequest("/v1/status",HttpMethod::GET);
  auto json=RabbitMQRestService::ParseJson(r.second);
  EXPECT_EQ(r.first,200);
  EXPECT_STREQ(json["rabbitmq_service"]["status"].asCString(),"Not Connected");
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