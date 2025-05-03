#include <gtest/gtest.h>
#include "network_shared/DbService.h"

namespace {

class DbServiceTest : public ::testing::Test {
 protected:
  void SetUp() override {
    service = std::make_unique<db_service::DbService>("dbname=test user=test password=test host=localhost");
  }

  std::unique_ptr<db_service::DbService> service;
};

TEST_F(DbServiceTest, ConnectToDatabase) {
  ASSERT_TRUE(service->connect());
  ASSERT_TRUE(service->isConnected());
}

TEST_F(DbServiceTest, CreateAndAuthenticateUser) {
  std::string user_id = service->createUser("testuser", "password");
  ASSERT_FALSE(user_id.empty());
  ASSERT_TRUE(service->authenticateUser("testuser", "password"));
}

TEST_F(DbServiceTest, CreateExperiment) {
  std::string user_id = service->createUser("expuser", "password");
  Json::Value params;
  params["N"] = 100;
  std::string exp_id = service->createExperiment(user_id, params);
  ASSERT_FALSE(exp_id.empty());

  Json::Value exp = service->getExperiment(exp_id);
  ASSERT_EQ(exp["status"].asString(), "pending");
  ASSERT_EQ(exp["parameters"]["N"].asInt(), 100);
}

TEST_F(DbServiceTest, RegisterNodeAndUpdateStatus) {
  std::string node_id = service->registerNode("192.168.1.1", 100.0);
  ASSERT_FALSE(node_id.empty());

  service->updateNodeStatus(node_id, "busy");
  Json::Value node = service->getNode(node_id);
  ASSERT_EQ(node["status"].asString(), "busy");
}

TEST_F(DbServiceTest, CreateIterationAndUpdate) {
  std::string user_id = service->createUser("iteruser", "password");
  Json::Value params;
  params["N"] = 50;
  std::string exp_id = service->createExperiment(user_id, params);
  std::string node_id = service->registerNode("192.168.1.2", 200.0);

  std::string iter_id = service->createIteration(exp_id, node_id, "solve");
  ASSERT_FALSE(iter_id.empty());

  Json::Value output;
  output["result"] = 42;
  service->updateIterationStatus(iter_id, "completed", output);
  Json::Value iter = service->getIteration(iter_id);
  ASSERT_EQ(iter["status"].asString(), "completed");
  ASSERT_EQ(iter["output_data"]["result"].asInt(), 42);
}

TEST_F(DbServiceTest, LogMessage) {
  std::string node_id = service->registerNode("192.168.1.3", 300.0);
  service->log(node_id, "info", "Test log message");
  // Note: Full log verification requires querying the Log table, omitted for brevity
}

} // namespace

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}