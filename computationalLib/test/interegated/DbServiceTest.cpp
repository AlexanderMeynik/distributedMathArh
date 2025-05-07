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

  static inline std::string tlogin="testuser";
  static inline std::string tpassword="password";
};

TEST_F(DbServiceTest, SetGetCstring) {
  auto c_string = network_types::myConnString("a", "p", "localhost", "db", 5432);
  service_->SetConnStr(c_string);

  ASSERT_EQ(c_string, service_->GetConnStr());
}

TEST_F(DbServiceTest, ConnectToDatabase) {
  ASSERT_TRUE(service_->IsConnected());
}

TEST_F(DbServiceTest, TimestampSerializationDeserailizationTest) {
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

TEST_F(DbServiceTest, TimestampInsertRetrieveTest) {
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


TEST_F(DbServiceTest, CreateAndAuthenticateUser) {
  User user;
  user.login = tlogin;
  user.hashed_password = tpassword;
  user.role = UserRole::USER;

  user.user_id = service_->CreateUser(user);

  ASSERT_TRUE(service_->AuthenticateUser(user));

  auto users = service_->GetUsers(1);
  ASSERT_TRUE(std::find(users.begin(), users.end(), user) != users.end());
}
TEST_F(DbServiceTest,DeleteUser)
{
  auto users1 = service_->GetUsers(1);

  User user = *std::find_if(users1.begin(), users1.end(), [&](const User &item) {
    return item.login==tlogin;
  });

  service_->DeleteUser(user.user_id);

  auto users = service_->GetUsers(1);
  ASSERT_FALSE(std::find(users.begin(), users.end(), user) != users.end());

}

TEST_F(DbServiceTest, CreateExperiment) {
  User user;
  user.role = UserRole::USER;
  user.login = "expuser";
  user.hashed_password = "password";
  user.user_id = service_->CreateUser(user);

  Experiment experiment;
  experiment.user_id=user.user_id;
  experiment.parameters["N"] = 100;
  experiment.experiment_id = service_->CreateExperiment(experiment);

  auto experiments=service_->ListExperiments(user.user_id,1);

  ASSERT_TRUE(std::find(experiments.begin(),
                        experiments.end(), experiment) != experiments.end());
}

TEST_F(DbServiceTest, CreateIterationAndUpdate) {

  User user;
  user.role = UserRole::USER;
  user.login = "iteruser";
  user.hashed_password = "password";
  user.user_id = service_->CreateUser(user);

  Experiment exp;
  exp.user_id=user.user_id;
  exp.parameters["N"]=50;
  exp.experiment_id = service_->CreateExperiment(exp);

  Node node;
  node.ip_address="192.168.1.2";
  node.benchmark_score=shared::BenchResVec{200, 400};
  node.node_id = service_->RegisterNode(node);

  Iteration iterat;
  iterat.experiment_id=exp.experiment_id;
  iterat.node_id=node.node_id;
  iterat.iter_t=IterationType::SOLVE;

  iterat.iteration_id = service_->CreateIteration(iterat);

  Json::Value output;
  output["result"] = 42;
  service_->UpdateIterationStatus(iterat.iteration_id, "completed", output);

  auto iters=service_->ListIterations(exp.experiment_id,1);
  ASSERT_TRUE(std::find(iters.begin(),
                        iters.end(), iterat) != iters.end());
}




TEST_F(DbServiceTest, RegisterNodeAndUpdateStatus) {

  Node node;
  node.ip_address="193.168.1.2";
  node.benchmark_score=shared::BenchResVec{100, 200};
  node.node_id = service_->RegisterNode(node);
  auto node_id = service_->RegisterNode(node);

  service_->UpdateNodeStatus(node_id, "busy");

  auto nodes=service_->ListNodes(1);
  ASSERT_TRUE(std::find(nodes.begin(),
                        nodes.end(), node) != nodes.end());
}


TEST_F(DbServiceTest, DeleteNode) {
  auto nodes=service_->ListNodes(1);

  auto node=nodes[0];

  service_->UnregisterNode(node.node_id);
  auto nodes_del=service_->ListNodes(1);
  ASSERT_TRUE(std::find(nodes_del.begin(),
                        nodes_del.end(), node) != nodes_del.end());
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