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

ConnPtr TryConnect(const PostgreSQLCStr &conn_str,
                   std::string_view service_name) {
  ConnPtr conn;

  try {
    conn = std::make_shared<pqxx::connection>(conn_str.CStr());
  } catch (const pqxx::broken_connection &e) {
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
ConnPtr CreateDatabase(PostgreSQLCStr c_string, std::string_view db_name) {

  ConnPtr conn;
  c_string.SetDbname(db_name);

  auto t_string = c_string;
  t_string.SetDbname(SampleTempDb);

  auto temp_connection = TryConnect(t_string, "OuterService");

  NonTransType no_trans_exec(*temp_connection);

  try {

    if (CheckDatabaseExistence(no_trans_exec, c_string.GetDbname())) {
      no_trans_exec.abort();
      Disconnect(temp_connection);
      return TryConnect(c_string, "Outer");
    }

    no_trans_exec.exec(fmt::format("CREATE DATABASE \"{}\";",
                                   c_string.GetDbname()));

    no_trans_exec.exec(fmt::format("GRANT ALL ON DATABASE \"{}\" TO {};",
                                   c_string.GetDbname(),
                                   c_string.GetUser()));
    no_trans_exec.commit();

  } catch (const pqxx::sql_error &e) {
    throw SQL_ERROR(e.what(), e.query(), e.sqlstate());
  }

  Disconnect(temp_connection);
  return TryConnect(c_string, "Outer");
}
void DropDatabase(PostgreSQLCStr c_string, std::string_view db_name) {
  c_string.SetDbname(db_name);
  auto t_string = c_string;
  t_string.SetDbname(SampleTempDb);
  auto temp_connection = TryConnect(t_string, "Outer");

  NonTransType no_trans_exec(*temp_connection);
  std::string qq;
  try {

    if (!CheckDatabaseExistence(no_trans_exec, c_string.GetDbname())) {
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

  Disconnect(temp_connection);
}
bool CheckDatabaseExistence(NonTransType &non_trans, std::string_view db_name) {
  std::string qq = "SELECT 1 FROM pg_database WHERE datname = $1";
  return non_trans.exec(qq,pqxx::params{db_name.data()}).size();
}

DbEntry::DbEntry(const pqxx::row &row) {
  oid =row["oid"].as<IndexType>();
  name=row["datname"].as<std::string>();
}
bool DbEntry::operator==(const DbEntry &rhs) const {
  return name == rhs.name;
}
DbEntry::DbEntry(IndexType oid, const std::string &name) : oid(oid), name(name) {}

std::vector<DbEntry> ListDatabases(const PostgreSQLCStr & c_string) {
  auto conn = TryConnect(c_string, "Outer");
  auto res = ExecuteTransaction(conn, [&](TransactionT &txn) {
    auto r = txn.exec("SELECT oid,datname FROM pg_database;");
    return r;
  }, "Outer", c_string);
  return ParseArray<std::vector,DbEntry>(res);
}
void FillDatabase(const PostgreSQLCStr &c_string, std::string_view script) {
  auto conn = TryConnect(c_string, "Outer");
  ExecuteTransaction(conn, [&](TransactionT &txn) {
    auto r = txn.exec(script);
    return r;
  }, "Outer", c_string);
}
ResType ExecuteTransaction(ConnPtr &ptr,
                           const std::function<ResType(TransactionT &)> &func,
                           std::string_view service_name,
                           const PostgreSQLCStr &conn_str) {
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
}
std::string PaginateRequest(std::string_view initial_request, IndexType page_num, IndexType page_size) {
  return fmt::format("{} LIMIT {} OFFSET {}",
                     initial_request,
                     page_size,
                     (page_num - 1) * page_size);
}


}