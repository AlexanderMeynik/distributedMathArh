#include "network_shared/daoClasses.h"

namespace db_common {

/*template<typename T>
GetFunctionOpt<T,StrToTimepoint>*/
Experiment::Experiment(const pqxx::row &row) {
  experiment_id = row["experiment_id"].as<IndexType>();
  user_id = row["user_id"].as<IndexType>();

  auto rr = row["status"].as<std::string>();
  status = StrToEnum(rr, kStrToExpStatus);
  parameters = Json::Value(row["parameters"].as<std::string>());
  created_at = *StrToTimepoint(row["created_at"].as<std::string>());
  start_time = GetFunctionOpt<TimepointType,StrToTimepoint>(row["start_time"]);;
  end_time = GetFunctionOpt<TimepointType,StrToTimepoint>(row["end_time"]);
}

Iteration::Iteration(const pqxx::row &row) {

  iteration_id = row["iteration_id"].as<IndexType>();
  experiment_id = row["experiment_id"].as<IndexType>();
  node_id = row["node_id"].as<IndexType>();

  auto type = row["iter_t"].as<std::string>();
  iter_t = StrToEnum(type, kStrToIterType);

  auto st = row["status"].as<std::string>();
  status = StrToEnum(st, kStrToIterStatus);

  output_data = Json::Value(row["output_data"].as<std::string>());

  start_time = *StrToTimepoint(row["start_time"].as<std::string>());

  end_time = GetFunctionOpt<TimepointType,StrToTimepoint>(row["end_time"]);

}

User::User(const pqxx::row &row) {
  user_id = row["user_id"].as<IndexType>();
  login = row["login"].as<std::string>();
  hashed_password = row["hashed_password"].as<std::string>();
  auto rr = row["role"].as<std::string>();
  role = StrToEnum(rr, kStrToUserRole);
  user_id = row["user_id"].as<IndexType>();

  created_at = *StrToTimepoint(row["created_at"].as<std::string>());

  last_login = GetFunctionOpt<TimepointType,StrToTimepoint>(row["last_login"]);
}
std::string User::GetInertValues() const {
  return "";
  /*TransactionT::quote("s");
  return fmt::format("{}, {}, {}",pqxx::work::quote(login), txn.quote(password), txn.quote(role));
*/}

bool User::operator==(const User &rhs) const {
  return login == rhs.login &&
      hashed_password == rhs.hashed_password &&
      role == rhs.role;
}


bool Experiment::operator==(const Experiment &rhs) const {
  return experiment_id == rhs.experiment_id;
}

bool Iteration::operator==(const Iteration &rhs) const {
  return iteration_id == rhs.iteration_id;
}

bool Node::operator==(const Node &rhs) const {
  return ip_address == rhs.ip_address;
}
Node::Node(const pqxx::row &row) {

  node_id = row["node_id"].as<IndexType>();
  ip_address = row["ip_address"].as<std::string>();
  auto sql_arr = row["benchmark_score"].as<std::string>();
  auto arr = row["benchmark_score"].as_sql_array<shared::BenchResultType>();
  using namespace print_utils;
  benchmark_score = print_utils::ParseOneDimS<shared::BenchResVec>(sql_arr,
                                                                   arr.size(),
                                                                   EIGENF(EigenPrintFormats::VECTOR_DB_FORMAT));
  auto st = row["status"].as<std::string>();
  status = StrToEnum(st, kStrToNodeSt);

  last_ping = GetFunctionOpt<TimepointType,StrToTimepoint>(row["last_ping"]);

}

Log::Log(const pqxx::row &row) {
  log_id = row["log_id"].as<IndexType>();
  node_id = std::move(GetOpt<IndexType>(row["node_id"]));
  experiment_id = std::move(GetOpt<IndexType>(row["experiment_id"]));

  auto sev = row["severity"].as<std::string>();
  severity = shared::kStrToSev.at(sev);
  message = row["message"].as<std::string>();

  timestamp = *StrToTimepoint(row["timestamp"].as<std::string>());
}
bool Log::operator==(const Log &rhs) const {
  return log_id == rhs.log_id;
}
}