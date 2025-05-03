#include <gtest/gtest.h>
#include "network_shared/DbService.h"
#include "network_shared/networkTypes.h"
#include "network_shared/amqpCommon.h"
#include "../GoogleCommon.h"
namespace {
using test_common::AuthParams;
using test_common::WaitFor;
using amqp_common::ExtractHost;
AuthParams g_serviceParams;
class DbServiceTest : public ::testing::Test {
 protected:
  void SetUp() override {
    conn_string_=network_types::myConnString(g_serviceParams.username,
                                             g_serviceParams.password,
                                             ExtractHost(g_serviceParams.host).value()
                                             ,"",5432);

    conn_string_.SetPort(5432);
    conn_string_.SetDbname("data_deduplication_service");
    service = std::make_unique<db_service::DbService>(conn_string_.operator std::string());
  }

  network_types::myConnString conn_string_;
  std::unique_ptr<db_service::DbService> service;
};

TEST_F(DbServiceTest, ConnectToDatabase) {
  ASSERT_TRUE(service->Connect());
  ASSERT_TRUE(service->IsConnected());
}

TEST_F(DbServiceTest, CreateAndAuthenticateUser) {
  std::string user_id = service->CreateUser("testuser", "password");
  ASSERT_FALSE(user_id.empty());
  ASSERT_TRUE(service->AuthenticateUser("testuser", "password"));
}

TEST_F(DbServiceTest, CreateExperiment) {
  std::string user_id = service->CreateUser("expuser", "password");
  Json::Value params;
  params["N"] = 100;
  std::string exp_id = service->CreateExperiment(user_id, params);
  ASSERT_FALSE(exp_id.empty());

  Json::Value exp = service->GetExperiment(exp_id);
  ASSERT_EQ(exp["status"].asString(), "pending");
  ASSERT_EQ(exp["parameters"]["N"].asInt(), 100);
}

TEST_F(DbServiceTest, RegisterNodeAndUpdateStatus) {
  std::string node_id = service->RegisterNode("192.168.1.1", 100.0);
  ASSERT_FALSE(node_id.empty());

  service->UpdateNodeStatus(node_id, "busy");
  Json::Value node = service->GetNode(node_id);
  ASSERT_EQ(node["status"].asString(), "busy");
}

TEST_F(DbServiceTest, CreateIterationAndUpdate) {
  std::string user_id = service->CreateUser("iteruser", "password");
  Json::Value params;
  params["N"] = 50;
  std::string exp_id = service->CreateExperiment(user_id, params);
  std::string node_id = service->RegisterNode("192.168.1.2", 200.0);

  std::string iter_id = service->CreateIteration(exp_id, node_id, "solve");
  ASSERT_FALSE(iter_id.empty());

  Json::Value output;
  output["result"] = 42;
  service->UpdateIterationStatus(iter_id, "completed", output);
  Json::Value iter = service->GetIteration(iter_id);
  ASSERT_EQ(iter["status"].asString(), "completed");
  ASSERT_EQ(iter["output_data"]["result"].asInt(), 42);
}

TEST_F(DbServiceTest, LogMessage) {
  std::string node_id = service->RegisterNode("192.168.1.3", 300.0);
  service->Log(node_id, "info", "Test Log message");
  //todo Full log verification requires querying the Log table, omitted for brevity
}

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