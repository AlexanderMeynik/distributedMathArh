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
const std::string kDbName = "test_db_1";
class DbServiceTest : public ::testing::Test {
 protected:
  void SetUp() override {
    conn_string_ = network_types::myConnString(g_serviceParams.username,
                                               g_serviceParams.password,
                                               ExtractHost(g_serviceParams.host).value(), kDbName, 5432);

    service_ = std::make_unique<db_service::DbService>(conn_string_);
    service_->Connect();
  }

  network_types::myConnString conn_string_;
  std::unique_ptr<db_service::DbService> service_;
};

TEST_F(DbServiceTest, SetGetCstring) {
  auto c_string = network_types::myConnString("a", "p", "localhost", "db", 5432);
  service_->SetConnStr(c_string);

  ASSERT_EQ(c_string, service_->GetConnStr());
}

TEST_F(DbServiceTest, timestampFullCycle) {
  using namespace std;
  using std::chrono::steady_clock;

  auto r = service_->ExecuteTransaction([&](TransactionT &txt) {
    ResType r;
    r = txt.exec("SELECT fmt_ts_us(now()::timestamp)");
    return r;
  });
  auto postgres_time = r[0][0].as<std::string>();
  auto s = StrToTimepoint(postgres_time);

  auto new_time = TpToString(s.value());

  ASSERT_EQ(postgres_time, new_time);
}

TEST_F(DbServiceTest, timestampSaveRetrieveCycle) {
  uint64_t now = timing::Now<std::chrono::microseconds>();

  auto initilaTime = TpToString(now);
  using namespace std;

  auto r = service_->ExecuteTransaction([&](TransactionT &txt) {
    ResType r;
    r = txt.exec(fmt::format("SELECT fmt_ts_us({}::timestamp)", txt.quote(initilaTime)));
    return r;
  });
  auto postgres_time = r[0][0].as<std::string>();

  ASSERT_EQ(initilaTime, postgres_time);
}
TEST_F(DbServiceTest, ConnectToDatabase) {
  ASSERT_TRUE(service_->IsConnected());
}

TEST_F(DbServiceTest, CreateAndAuthenticateUser) {
  User user;
  user.login = "testuser";
  user.hashed_password = "password";
  user.role = UserRole::USER;

  user.user_id = service_->CreateUser(user);

  ASSERT_TRUE(service_->AuthenticateUser(user));

  auto users = service_->GetUsers(1);
  ASSERT_TRUE(std::find(users.begin(), users.end(), user) != users.end());
}

TEST_F(DbServiceTest, CreateExperiment) {
  auto user_id = service_->CreateUser("expuser", "password");
  Json::Value params;
  params["N"] = 100;
  auto exp_id = service_->CreateExperiment(user_id, params);

  Json::Value exp = service_->GetExperiment(exp_id);
  ASSERT_EQ(exp["status"].asString(), "pending");
  ASSERT_EQ(exp["parameters"]["N"].asInt(), 100);
}

TEST_F(DbServiceTest, RegisterNodeAndUpdateStatus) {
  auto node_id = service_->RegisterNode("192.168.1.1", shared::BenchResVec{100, 200});

  service_->UpdateNodeStatus(node_id, "busy");
  Json::Value node = service_->GetNode(node_id);
  ASSERT_EQ(node["status"].asString(), "busy");
}

TEST_F(DbServiceTest, CreateIterationAndUpdate) {
  auto user_id = service_->CreateUser("iteruser", "password");
  Json::Value params;
  params["N"] = 50;
  auto exp_id = service_->CreateExperiment(user_id, params);
  auto node_id = service_->RegisterNode("192.168.1.2", shared::BenchResVec{200, 400});

  auto iter_id = service_->CreateIteration(exp_id, node_id, "solve");

  Json::Value output;
  output["result"] = 42;
  service_->UpdateIterationStatus(iter_id, "completed", output);
  Json::Value iter = service_->GetIteration(iter_id);
  ASSERT_EQ(iter["status"].asString(), "completed");
  ASSERT_EQ(iter["output_data"]["result"].asInt(), 42);
}

TEST_F(DbServiceTest, LogMessage) {
  auto node_id = service_->RegisterNode("192.168.1.3", shared::BenchResVec{300, 600});
  service_->Log(node_id, "info", "Test Log message");
  //todo Full log verification requires querying the Log table, omitted for brevity
}

}

class DatabaseTestEnvironment : public ::testing::Environment {
 public:
  void SetUp() override {

    conn_string_ = network_types::myConnString(g_serviceParams.username,
                                               g_serviceParams.password,
                                               ExtractHost(g_serviceParams.host).value(), kDbName, 5432);

    CreateDatabase(conn_string_, kDbName);

    std::ifstream t("fixture/dbSchem.sql");
    if (!t) {
      throw std::runtime_error("No dbSchem to init");
    }
    std::stringstream buffer;
    buffer << t.rdbuf();
    FillDatabase(conn_string_, buffer.str());
  }

  void TearDown() override {
    DropDatabase(conn_string_, kDbName);
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