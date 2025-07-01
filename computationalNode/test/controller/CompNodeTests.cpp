#include "testingUtils/GoogleCommon.h"
#include <cstdlib>
#include <filesystem>
#include <boost/process.hpp>
using namespace test_common;
TEST(_1,_1)
{
  ASSERT_TRUE(true);
}
AuthParams g_serviceParams;
//todo run comp node(compile it)(workflow)
//integrate it into env
//create tests for comp node

namespace bp = boost::process;

class DatabaseTestEnvironment : public ::testing::Environment {
 public:
  void SetUp() override {
    auto abs_fp = std::filesystem::absolute(fp);
    childP = std::make_unique<bp::child>(comp_node,fmt::format_int(8081).c_str(), bp::start_dir(abs_fp.string()));
  }

  void TearDown() override {
    childP->terminate();
  }

 private:
  static std::unique_ptr<bp::child> inline childP= nullptr;
  static inline const std::filesystem::path fp=std::filesystem::absolute("../bin");
  const static std::string inline comp_node="./compNode";
};
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  if (argc < 4) {
    std::cerr << "Usage: " << argv[0] << " <host> <login> <password>\n";
    return 1;
  }
  g_serviceParams.host = argv[1];
  g_serviceParams.username = argv[2];
  g_serviceParams.password = argv[3];

  ::testing::AddGlobalTestEnvironment(new DatabaseTestEnvironment);
  return RUN_ALL_TESTS();
}