#include "network_shared/DbService.h"

namespace db_service {

DbService::DbService() : conn_str_(), conn_(nullptr) {

}

DbService::DbService(const PostgreSQLCStr &conn_str) : conn_str_(conn_str), conn_(nullptr) {

}

DbService::~DbService() {
  Disconnect();
}

bool DbService::Connect() {
  Reconnect();
  return true;
}

void DbService::Disconnect() {
  ::db_service::Disconnect(conn_);
}

bool DbService::IsConnected() const {
  return ::db_service::CheckConnection(conn_);
}

ResType DbService::ExecuteTransaction(const std::function<ResType(TransactionT &)> &func) {
  return ::db_common::ExecuteTransaction(conn_, func, service_name, conn_str_);
}
ResType DbService::ExecuteSubTransaction(TransactionT &txn,
                                         const std::function<ResType(Subtransaction &)> &func,
                                         std::string_view sub_name) {
  return ::db_common::ExecuteSubTransaction(txn, func, sub_name);

}

IndexType DbService::CreateUser(std::string_view login, std::string_view password, std::string_view role) {
  IndexType user_id;
  ExecuteTransaction([&](TransactionT &txn) {

    std::string qq = "INSERT INTO \"User\" (login, hashed_password, role)"
                     " VALUES ($1, $2, $3)"
                     " RETURNING user_id";
    user_id = txn.exec(qq,pqxx::params{login, password, role}
    ).one_row()[0].as<IndexType>();

    return ResType();
  });
  return user_id;
}

bool DbService::AuthenticateUser(std::string_view login, std::string_view password) {
  bool authenticated = false;
  ExecuteTransaction([&](TransactionT &txn) {

    std::string qq = "SELECT hashed_password FROM \"User\" WHERE login = $1";
    auto result = txn.exec(qq, pqxx::params{login}).one_row();
    if (result[0].as<std::string>() == password) {
      authenticated = true;
      std::string qq2 = "UPDATE \"User\" SET last_login = NOW() WHERE login = $1";
      txn.exec(qq2, pqxx::params{login});
    }
    return ResType();
  });
  return authenticated;
}

void DbService::DeleteUser(IndexType user_id) {
  ExecuteTransaction([&](TransactionT &txn) {
    std::string qq = "DELETE FROM \"User\" WHERE user_id = $1";
    txn.exec(qq, pqxx::params{user_id});
    return ResType();
  });
}
std::vector<User> DbService::ListUsers(IndexType page_num, IndexType page_size) {
  auto result=ExecuteTransaction([&](TransactionT &txn) {
    std::string qq = "SELECT * FROM \"User\"";
    auto req=PaginateRequest(qq,
                             page_num, page_size);
    return txn.exec(req);
  });
  return ParseArray<std::vector,User>(result);
}

IndexType DbService::CreateExperiment(IndexType user_id, const Json::Value &parameters) {
  IndexType experiment_id;
  ExecuteTransaction([&](TransactionT &txn) {
    Json::StreamWriterBuilder builder;
    std::string params_str = Json::writeString(builder, parameters);

    std::string qq = "INSERT INTO \"Experiment\" (user_id, parameters)"
                     " VALUES ($1, $2::jsonb)"
                     " RETURNING experiment_id";

    experiment_id = txn.exec(qq,
                             pqxx::params{ user_id, params_str}
                             ).one_row()[0].as<IndexType>();

    InnerLog(txn, std::nullopt,std::nullopt, "info", fmt::format("User {} created experiment ", user_id, experiment_id));
    return ResType();
  });
  return experiment_id;
}

IndexType DbService::CreateExperiment(Experiment exp) {
  return CreateExperiment(exp.user_id,exp.parameters);
}

std::vector<Experiment> DbService::ListExperiments(IndexType user_id,IndexType page_num, IndexType page_size) {

  auto result=ExecuteTransaction([&](TransactionT &txn) {
    std::string qq = "SELECT * FROM \"Experiment\" WHERE user_id =$1";
    auto req=PaginateRequest(qq,
                             page_num, page_size);
    return txn.exec(req, pqxx::params{user_id});
  });
  return ParseArray<std::vector,Experiment>(result);
}

void DbService::UpdateExperimentStatus(IndexType experiment_id, std::string_view status) {
  ExecuteTransaction([&](TransactionT &txn) {

    std::string qq = "UPDATE \"Experiment\" SET status = $1, start_time = CASE WHEN"
                     " $1 = 'running'::status THEN NOW() ELSE start_time END,"
                     " end_time = CASE WHEN $1 IN ('succeeded'::status, 'error'::status)"
                     " THEN NOW() ELSE end_time END WHERE experiment_id = $2";
    txn.exec(qq,pqxx::params{status, experiment_id});

    InnerLog(txn, std::nullopt,std::nullopt, "info", fmt::format("Experiment {}  status updated to {}", experiment_id, status));
    return ResType();
  });
}

void DbService::UpdateExperimentStatus(IndexType experiment_id, ExperimentStatus status) {
  UpdateExperimentStatus(experiment_id,shared::EnumToStr(status,kExpStatusToStr));
}
Experiment DbService::GetExperiment(IndexType experiment_id) {
  Experiment experiment;
  ExecuteTransaction([&](TransactionT &txn) {
    std::string qq = "SELECT * FROM \"Experiment\" WHERE experiment_id = $1";
    auto row = txn.exec(qq, pqxx::params{experiment_id}).one_row();
    experiment=std::move(Experiment(row));
    return ResType();
  });
  return experiment;
}

IndexType DbService::CreateIteration(IndexType experiment_id,
                                     IndexType node_id,
                                     std::string_view iter_type) {
  IndexType iteration_id;
  ///@todo use stream api
  ExecuteTransaction([&](TransactionT &txn) {
    std::string qq = "INSERT INTO \"Iteration\" (experiment_id, node_id, iter_t)"
                     " VALUES ($1, $2, $3)"
                     " RETURNING iteration_id";
    iteration_id = txn.exec(qq,
                            pqxx::params{experiment_id, node_id, iter_type}).one_row()[0].as<IndexType>();

    InnerLog(txn,experiment_id, node_id, "info", fmt::format("Iteration {} created for experiment {}", iteration_id, experiment_id));
    return ResType();
  });
  return iteration_id;
}

void DbService::UpdateIterationStatus(IndexType iteration_id,
                                      std::string_view status,
                                      const Json::Value &output_data) {
  ExecuteTransaction([&](TransactionT &txn) {

    if (output_data.isNull()) {

      std::string qq = "UPDATE \"Iteration\" SET status = $1::iteration_status, end_time = CASE WHEN $1 IN ('succeeded', 'error')"
                       " THEN NOW() ELSE end_time END "
                       " WHERE iteration_id = $2";

      txn.exec(qq, pqxx::params{status, iteration_id});
    } else {
      Json::StreamWriterBuilder builder;
      std::string output_str = Json::writeString(builder, output_data);

      std::string qq = "UPDATE \"Iteration\" SET status = $1::iteration_status, output_data = $2::jsonb, end_time ="
                       " CASE WHEN $1 IN ('succeeded', 'error') "
                       "THEN NOW() ELSE end_time END "
                       "WHERE iteration_id = $3";
      txn.exec(qq, pqxx::params{status, output_str, iteration_id});
    }
    InnerLog(txn,std::nullopt,std::nullopt, "info",
             fmt::format("Iteration {} status updated to {}",
                         iteration_id,
                         status)
                         );
    return ResType();
  });
}

void DbService::UpdateIterationStatus(IndexType iteration_id, IterationStatus status, const Json::Value &output_data) {
  UpdateIterationStatus(iteration_id,shared::EnumToStr(status,kIterStatusToStr),output_data);
}

Iteration DbService::GetIteration(IndexType iteration_id) {
  Iteration result;
  ExecuteTransaction([&](TransactionT &txn) {
    std::string qq = "SELECT * FROM \"Iteration\" WHERE iteration_id = $1";


    auto row = txn.exec(qq, pqxx::params{iteration_id}).one_row();
    result = std::move(Iteration(row));
    return ResType();
  });

  return result;
}

IndexType DbService::RegisterNode(std::string_view ip_address,
                                  const shared::BenchResVec &benchmark_score) {

  IndexType node_id;
  ExecuteTransaction([&](TransactionT &txn) {
    using namespace print_utils;

    std::string qq = "INSERT INTO \"Node\" (ip_address, benchmark_score, status, last_ping)"
                     " VALUES ($1, $2, 'active', NOW())"
                     "RETURNING node_id";
    auto bench_vec = OneDimToString(benchmark_score, false,
                                    EIGENF(EigenPrintFormats::VECTOR_DB_FORMAT));

    node_id =txn.exec(qq,
                      pqxx::params{ip_address,bench_vec}
                      ).one_row()[0].as<IndexType>();

    InnerLog(txn, std::nullopt,node_id, "info", fmt::format("Node registered with IP {}", ip_address));
    return ResType();
  });
  return node_id;
}

void DbService::UpdateNodeStatus(IndexType node_id, std::string_view status) {
  ExecuteTransaction([&](TransactionT &txn) {
    std::string qq = "UPDATE \"Node\" SET status = $1::node_status, last_ping = NOW() WHERE node_id = $2";
    txn.exec(qq, pqxx::params{status,node_id});
    InnerLog(txn,std::nullopt, node_id, "info", fmt::format("Node status updated to {}", node_id));
    return ResType();
  });
}

void DbService::UnregisterNode(IndexType node_id) {
  ExecuteTransaction([&](TransactionT &txn) {
    std::string qq = "UPDATE \"Node\" SET status = 'inactive', last_ping = NOW() WHERE node_id = $1";
    txn.exec(qq, pqxx::params{node_id});
    InnerLog(txn,std::nullopt,node_id, "info", fmt::format("Node {} unregistered.", node_id));
    return ResType();
  });
}

Node DbService::GetNode(IndexType node_id) {
  auto result=ExecuteTransaction([&](TransactionT &txn) {
    using namespace print_utils;
    auto qq="SELECT * FROM \"Node\" WHERE node_id = $1";
    txn.exec(qq, pqxx::params{node_id});
    auto res = txn.exec(qq, pqxx::params{node_id});
    return res;
  });
  return Node(result[0]);
}

IndexType DbService::Log(std::optional<IndexType> experiment_id,
                    std::optional<IndexType> node_id,
                    std::string_view severity,
                    std::string_view message) {
  auto res=ExecuteTransaction([&](TransactionT &txn) {
    std::string qq = "INSERT INTO \"Log\" (node_id,experiment_id, severity, message)"
                     " VALUES ($1,$2, $3::log_severity, $4)"
                     " RETURNING log_id";
    return txn.exec(qq, pqxx::params{node_id,experiment_id,severity,message});
  });
  return res[0][0].as<IndexType>();
}

IndexType DbService::Log(const db_common::Log &log) {
  return Log(log.experiment_id, log.node_id,
             print_utils::kSevToStr[static_cast<unsigned long>(log.severity)],
             log.message);
}
IndexType DbService::InnerLog(TransactionT &txn,
                         std::optional<IndexType> experiment_id,
                         std::optional<IndexType> node_id,
                         std::string_view severity,
                         std::string_view message) {

  return ExecuteSubTransaction(txn, [&](Subtransaction &s) {
    std::string qq = "INSERT INTO \"Log\" (node_id,experiment_id, severity, message)"
                     " VALUES ($1,$2, $3::log_severity, $4)"
                     " RETURNING log_id";
    return s.exec(qq, pqxx::params{node_id,experiment_id,severity,message});
  }, "Log")[0][0].as<IndexType>();
}

const PostgreSQLCStr &DbService::GetConnStr() const {
  return conn_str_;
}
void DbService::SetConnStr(const PostgreSQLCStr &conn_str) {
  conn_str_ = conn_str;
}
void DbService::Reconnect() {
  if (CheckConnection(conn_)) {
    throw Already_Connected(service_name, conn_str_.GetVerboseName());
  }
  conn_ = TryConnect(conn_str_, service_name);
}
bool DbService::AuthenticateUser(const User &user) {
  return AuthenticateUser(user.login, user.hashed_password);
}
IndexType DbService::CreateUser(const User &user) {
  return CreateUser(user.login, user.hashed_password, EnumToStr(user.role, kUserRoleToStr));
}
std::vector<Iteration> DbService::ListIterations(IndexType experiment_id,
                                                 IndexType page_num,
                                                 IndexType page_size) {
  auto result=ExecuteTransaction([&](TransactionT &txn) {
    std::string qq = "SELECT * FROM \"Iteration\" WHERE experiment_id =$1";
    auto req=PaginateRequest(qq,
                             page_num, page_size);
    return txn.exec(req,pqxx::params{experiment_id});
  });
  return ParseArray<std::vector,Iteration>(result);
}
std::vector<Node> DbService::ListNodes(IndexType page_num, IndexType page_size) {

  auto result=ExecuteTransaction([&](TransactionT &txn) {
    std::string qq = "SELECT * FROM \"Node\"";
    auto req=PaginateRequest(qq,
                             page_num, page_size);
    return txn.exec(req);
  });
  return ParseArray<std::vector,Node>(result);
}
IndexType DbService::CreateIteration(const Iteration &iter) {
  return CreateIteration(iter.experiment_id,
                         iter.node_id,
                         EnumToStr(iter.iter_t, kIterTypeToStr));
}
IndexType DbService::RegisterNode(const Node &node) {
  return RegisterNode(node.ip_address,node.benchmark_score);
}

std::vector<db_common::Log> DbService::ListLogs(IndexType page_num, IndexType page_size) {
  auto result=ExecuteTransaction([&](TransactionT &txn) {
    std::string qq = "SELECT * FROM \"Log\"";
    auto req=PaginateRequest(qq,
                             page_num, page_size);
    return txn.exec(req);
  });
  return ParseArray<std::vector,db_common::Log>(result);
}


} // namespace db_service