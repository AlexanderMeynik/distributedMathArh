#include "network_shared/dbCommon.h"

namespace db_common {

bool CheckConnection(const ConnPtr &conn_ptr) {
  return conn_ptr && conn_ptr->is_open();
}
void Disconnect(ConnPtr &conn_ptr) {
  if (conn_ptr) {
    conn_ptr->close();
    conn_ptr.reset();
  }
}
ConnPtr TryConnect(const myConnString &conn_str, const std::string service_name) {
  ConnPtr conn;

  try {
    conn = std::make_shared<pqxx::connection>(conn_str.CStr());
  } catch (const std::exception &e) {
    throw Broken_Connection(service_name, conn_str.GetVerboseName());
  }

  if (!CheckConnection(conn)) {
    throw Broken_Connection(service_name, conn_str.GetVerboseName());
  }

  return conn;
}
ResType TerminateAllDbConnections(NonTransType &no_trans_exec,
                                  std::string_view db_name) {
  std::string qq = fmt::format("SELECT pg_terminate_backend(pg_stat_activity.pid)\n"
                               "FROM pg_stat_activity "
                               "WHERE pg_stat_activity.datname = \'{}\' "
                               " AND pid <> pg_backend_pid();", db_name);
  ResType r = no_trans_exec.exec(qq);

  return r;
}
ConnPtr CreateDatabase(network_types::myConnString c_string, std::string_view db_name) {

  ConnPtr conn;
  c_string.SetDbname(db_name);

  auto t_string = c_string;
  t_string.SetDbname(SampleTempDb);

  auto temp_connection = TryConnect(t_string, "OuterService");

  NonTransType no_trans_exec(*temp_connection);

  try {

    if (CheckDatabaseExistence(no_trans_exec, c_string.GetDbname()) != 0) {
      no_trans_exec.abort();
      Disconnect(temp_connection);
      return TryConnect(c_string, "Outer");
    }

    no_trans_exec.exec(fmt::format("CREATE DATABASE \"{}\";",
                                   c_string.GetDbname().c_str()));

    no_trans_exec.exec(fmt::format("GRANT ALL ON DATABASE \"{}\" TO {};",
                                   c_string.GetDbname().c_str(),
                                   c_string.GetUser().c_str()));
    no_trans_exec.commit();

  } catch (const pqxx::sql_error &e) {
    throw SQL_ERROR(e.what(), e.query(), e.sqlstate());
  }
  catch (const std::exception &e) {
    throw shared::MyException(e.what());
  }

  Disconnect(temp_connection);
  return TryConnect(c_string, "Outer");
}
void DropDatabase(network_types::myConnString c_string, std::string_view db_name) {
  c_string.SetDbname(db_name);
  auto t_string = c_string;
  t_string.SetDbname(SampleTempDb);
  auto temp_connection = TryConnect(t_string, "Outer");

  NonTransType no_trans_exec(*temp_connection);
  std::string qq;
  try {

    if (CheckDatabaseExistence(no_trans_exec, c_string.GetDbname()) == 0) {
      no_trans_exec.abort();
      Disconnect(temp_connection);
      return;
    }

    TerminateAllDbConnections(no_trans_exec, c_string.GetDbname());

    qq = fmt::format("DROP DATABASE \"{}\";",
                     c_string.GetDbname());
    no_trans_exec.exec(qq);
    no_trans_exec.commit();

  } catch (const pqxx::sql_error &e) {
    throw SQL_ERROR(e.what(), e.query(), e.sqlstate());
  }
  catch (const std::exception &e) {
    throw shared::MyException(e.what());
  }

  Disconnect(temp_connection);
}
size_t CheckDatabaseExistence(NonTransType &non_trans, std::string_view db_name) {
  std::string qq = fmt::format("SELECT 1 FROM pg_database WHERE datname = \'{}\';", db_name.data());
  return non_trans.exec(qq).size();
}
void FillDatabase(myConnString c_string, std::string_view script) {
  auto conn = TryConnect(c_string, "Outer");
  ExecuteTransaction(conn, [&](TransactionT &txn) {
    auto r = txn.exec(script);
    return r;
  }, "Outer", c_string);
}
ResType ExecuteTransaction(ConnPtr &ptr,
                           const std::function<ResType(TransactionT &)> &func,
                           std::string_view service_name,
                           const myConnString &conn_str) {
  if (!CheckConnection(ptr)) {
    throw Broken_Connection(service_name, conn_str.GetVerboseName());
  }
  TransactionT txn(*ptr);
  try {
    auto r = func(txn);
    txn.commit();
    return r;
  }
  catch (const pqxx::sql_error &e) {
    throw SQL_ERROR(e.what(), e.query(), e.sqlstate());
  }
  catch (const std::exception &e) {
    throw shared::MyException(fmt::format("Some other error {} {}:{}",
                                          e.what(), __FILE__, __LINE__), shared::Severity::info);
  }
}
ResType ExecuteSubTransaction(TransactionT &txn,
                              const std::function<ResType(Subtransaction &)> &func,
                              std::string_view sub_name) {

  ResType res;
  Subtransaction s(txn, sub_name);
  try {

    auto r = func(s);
    s.commit();
    return r;
  }
  catch (const pqxx::sql_error &e) {
    throw SQL_ERROR(e.what(), e.query(), e.sqlstate());
  }
  catch (const std::exception &e) {
    throw shared::MyException(fmt::format("Some other error {} {}:{}",
                                          e.what(), __FILE__, __LINE__), shared::Severity::info);
  }
}

Experiment::Experiment(pqxx::row &row) {
  experiment_id = row["experiment_id"].as<IndexType>();
  user_id = row["user_id"].as<IndexType>();

  auto rr = row["status"].as<std::string>();
  status = strToEnum(rr,kStrToExpStatus);
  parameters = Json::Value(row["parameters"].as<std::string>());
  created_at = *StrToTimepoint(row["created_at"].as<std::string>());
  start_time = (!row["start_time"].is_null()) ?
               std::optional<IndexType>{
                   *StrToTimepoint(row["start_time"].as<std::string>())
               } : std::nullopt;
  end_time = (!row["end_time"].is_null()) ?
               std::optional<IndexType>{
                   *StrToTimepoint(row["end_time"].as<std::string>())
               } : std::nullopt;
}

Iteration::Iteration(pqxx::row &row) {

  iteration_id = row["iteration_id"].as<IndexType>();
  experiment_id = row["experiment_id"].as<IndexType>();
  node_id = row["node_id"].as<IndexType>();

  auto type=row["iter_t"].as<std::string>();
  iter_t = strToEnum(type,kStrToIterType);

  auto st=row["status"].as<std::string>();
  status = strToEnum(st,kStrToIterStatus);

  output_data = Json::Value(row["output_data"].as<std::string>());


  start_time = *StrToTimepoint(row["start_time"].as<std::string>());

  end_time = (!row["end_time"].is_null()) ?
             std::optional<IndexType>{
                 *StrToTimepoint(row["end_time"].as<std::string>())
             } : std::nullopt;
}


User::User(pqxx::row &row) {
  user_id = row["user_id"].as<IndexType>();
  login = row["login"].as<std::string>();
  hashed_password = row["hashed_password"].as<std::string>();
  auto rr = row["role"].as<std::string>();
  role = strToEnum(rr,kStrToUserRole);
  user_id = row["user_id"].as<IndexType>();

  created_at = *StrToTimepoint(row["created_at"].as<std::string>());
  last_login = (!row["last_login"].is_null()) ?
               std::optional<IndexType>{
                   *StrToTimepoint(row["created_at"].as<std::string>())
               } : std::nullopt;

}

bool User::operator==(const User &rhs) const {
  return login == rhs.login &&
      hashed_password == rhs.hashed_password &&
      role == rhs.role;
}

bool Experiment::operator==(const Experiment &rhs) const {
  return experiment_id == rhs.experiment_id;
}

bool Iteration::operator==(const Iteration &rhs) const {
  return iteration_id==rhs.iteration_id;
}

bool Node::operator==(const Node &rhs) const {
  return ip_address==rhs.ip_address;
}
Node::Node(pqxx::row &row) {

  node_id = row["node_id"].as<IndexType>();
  ip_address = row["ip_address"].as<std::string>();
  auto sql_arr = row["benchmark_score"].as<std::string>();
  auto arr = row["benchmark_score"].as_sql_array<shared::BenchResultType>();
  using namespace print_utils;
  benchmark_score =print_utils::ParseOneDimS<shared::BenchResVec>(sql_arr,
                                                                  arr.size(),//todo use enum semntics
                                                                  EIGENF(EigenPrintFormats::VECTOR_DB_FORMAT));
  auto st=row["status"].as<std::string>();
  status = strToEnum(st,kStrToNodeSt);

  last_ping = (!row["last_ping"].is_null()) ?
               std::optional<IndexType>{
                   *StrToTimepoint(row["last_ping"].as<std::string>())
               } : std::nullopt;
}
}