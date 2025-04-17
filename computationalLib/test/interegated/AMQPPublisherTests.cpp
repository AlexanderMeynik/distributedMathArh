#include <gtest/gtest.h>
#include "network_shared/AMQPPublisherService.h"
#include "../GoogleCommon.h"

using test_common::AuthParams;

AuthParams g_serviceParams;

TEST(AMQPPublisherTest,BaseStub)
{
  ASSERT_TRUE(true);
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
  std::cout<<g_serviceParams.host<<'\t'
           <<g_serviceParams.username<<'\t'
           <<g_serviceParams.password<<'\n';

  return RUN_ALL_TESTS();
}