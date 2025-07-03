#include "testingUtils/GoogleCommon.h"
#include <cstdlib>
#include <filesystem>
#include "network_shared/restUtils.h"
#include <unordered_set>
using namespace test_common;
AuthParams g_serviceParams;

namespace bp = boost::process;
using namespace rest_utils;
class ProcessRunningFixture: public ::testing::Test {
  using ChildSeT=std::unordered_set<ChildProcess>;


 public:
  ChildSeT child_set_;


 protected:


  void SetUp() override {
  }

  void TearDown() override {

  }

  static void SetUpTestSuite()
  {

    fp=std::filesystem::absolute("../bin");
    comp_node="./compNode";
    requestor_=std::make_unique<HttpRequestService>();
    abs_fp = std::filesystem::absolute(fp);
  }
  static void TearDownTestSuite()
  {
    
  }
  static inline std::filesystem::path fp;
  static inline std::string comp_node;
  static inline std::unique_ptr<HttpRequestService> requestor_;
  static inline std::filesystem::path abs_fp;
};

TEST_F(ProcessRunningFixture,TestServiceRun)
{
  requestor_->SetParams(fmt::format("http://localhost:{}",8081));

  child_set_.emplace(comp_node,fmt::format_int(8081).c_str(),fmt::format_int(1).c_str(),
                bp::start_dir(abs_fp.string()), bp::std_out > stdout, bp::std_err > stderr);

  SLEEP(std::chrono::milliseconds(200));
  HttpResult r;
  EXPECT_NO_THROW(r=requestor_->PerformRequest("/v1/status",HttpMethod::GET));
  ASSERT_EQ(r.first,200)<<fmt::format("Invalid response code: expected {}, got {} !",200,r.first);
}

