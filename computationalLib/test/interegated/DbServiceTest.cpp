#include <gtest/gtest.h>
#include <fstream>
#include "network_shared/DbService.h"
#include "network_shared/networkTypes.h"
#include "network_shared/amqpCommon.h"
#include "../GoogleCommon.h"
namespace {
using test_common::AuthParams;
using test_common::WaitFor;
using amqp_common::ExtractHost;
using namespace db_service;
AuthParams g_serviceParams;
const std::string dbName="test_db_1";
class DbServiceTest : public ::testing::Test {
 protected:
  void SetUp() override {
    conn_string_=network_types::myConnString(g_serviceParams.username,
                                             g_serviceParams.password,
                                             ExtractHost(g_serviceParams.host).value()
                                             ,dbName,5432);

    service = std::make_unique<db_service::DbService>(conn_string_);
    service->Connect();
  }

  network_types::myConnString conn_string_;
  std::unique_ptr<db_service::DbService> service;
};

TEST_F(DbServiceTest,SetGetCstring)
{
  auto cString=network_types::myConnString("a","p","localhost","db",5432);
  service->SetConnStr(cString);

  ASSERT_EQ(cString,service->GetConnStr());
}

TEST_F(DbServiceTest,timestampFullCycle)
{
  using namespace std;
  using std::chrono::steady_clock;

  auto r=service->ExecuteTransaction([&](TransactionT&txt){
    ResType r;
    r=txt.exec("SELECT now()::timestamp");
    return r;
  });
  auto postgres_time=r[0][0].as<std::string>();
  auto s= fromTimestamp(postgres_time);

  auto new_time= myTimeStampToTimestamp(s.value());

  ASSERT_EQ(postgres_time, new_time);
}

TEST_F(DbServiceTest, ConnectToDatabase) {
  ASSERT_TRUE(service->IsConnected());
}

TEST_F(DbServiceTest, CreateAndAuthenticateUser) {
  User user;
  user.login="testuser";
  user.hashed_password="password";
  user.role=UserRole::USER;

  user.user_id = service->CreateUser(user);

  ASSERT_TRUE(service->AuthenticateUser(user));

  auto users=service->GetUsers(1);
  ASSERT_TRUE(std::find(users.begin(), users.end(),user)!=users.end());
}

TEST_F(DbServiceTest, CreateExperiment) {
  auto user_id = service->CreateUser("expuser", "password");
  Json::Value params;
  params["N"] = 100;
  auto exp_id = service->CreateExperiment(user_id, params);

  Json::Value exp = service->GetExperiment(exp_id);
  ASSERT_EQ(exp["status"].asString(), "pending");
  ASSERT_EQ(exp["parameters"]["N"].asInt(), 100);
}

TEST_F(DbServiceTest, RegisterNodeAndUpdateStatus) {
  auto node_id = service->RegisterNode("192.168.1.1", shared::BenchResVec{100,200});


  service->UpdateNodeStatus(node_id, "busy");
  Json::Value node = service->GetNode(node_id);
  ASSERT_EQ(node["status"].asString(), "busy");
}

TEST_F(DbServiceTest, CreateIterationAndUpdate) {
  auto user_id = service->CreateUser("iteruser", "password");
  Json::Value params;
  params["N"] = 50;
  auto exp_id = service->CreateExperiment(user_id, params);
  auto node_id = service->RegisterNode("192.168.1.2", shared::BenchResVec{200,400});

  auto iter_id = service->CreateIteration(exp_id, node_id, "solve");


  Json::Value output;
  output["result"] = 42;
  service->UpdateIterationStatus(iter_id, "completed", output);
  Json::Value iter = service->GetIteration(iter_id);
  ASSERT_EQ(iter["status"].asString(), "completed");
  ASSERT_EQ(iter["output_data"]["result"].asInt(), 42);
}

TEST_F(DbServiceTest, LogMessage) {
  auto node_id = service->RegisterNode("192.168.1.3", shared::BenchResVec{300,600});
  service->Log(node_id, "info", "Test Log message");
  //todo Full log verification requires querying the Log table, omitted for brevity
}

}

class DatabaseTestEnvironment : public ::testing::Environment {
 public:
  void SetUp() override {

   conn_string_=network_types::myConnString(g_serviceParams.username,
                                             g_serviceParams.password,
                                             ExtractHost(g_serviceParams.host).value()
        ,dbName,5432);

    CreateDatabase(conn_string_, dbName);



    std::ifstream t("fixture/dbSchem.sql");
    if(!t)
    {
      throw std::runtime_error("No dbSchem to init");
    }
    std::stringstream buffer;
    buffer << t.rdbuf();
    FillDatabase(conn_string_, buffer.str());
  }

  void TearDown() override {
    DropDatabase(conn_string_, dbName);
  }

 private:
  ConnPtr conn_ptr_;
  myConnString conn_string_;
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