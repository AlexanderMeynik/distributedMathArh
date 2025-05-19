#include "network_shared/dbCommon.h"

namespace db_common {


myConnString::myConnString(std::string_view user,
                           std::string_view password,
                           std::string_view host,
                           std::string_view dbname,
                           unsigned int port)
    : user(user), password(password),
      host(host),
      dbname(dbname),
      port(port) {
  UpdateFormat();
}
myConnString::operator std::string_view() {
  return formatted_string;
}
const char *myConnString::CStr() const {
  return formatted_string.c_str();
}
void myConnString::SetPassword(std::string_view new_password) {
  password = new_password;
  UpdateFormat();
}
void myConnString::SetHost(std::string_view new_host) {
  host = new_host;
  UpdateFormat();
}
void myConnString::SetPort(unsigned int new_port) {
  port = new_port;
  UpdateFormat();
}
void myConnString::SetDbname(std::string_view new_dbname) {
  dbname = new_dbname;
  UpdateFormat();
}
void myConnString::SetUser(std::string_view new_user) {
  user = std::forward<std::string_view>(new_user);
  UpdateFormat();
}
const std::string &myConnString::GetUser() const {
  return user;
}
const std::string &myConnString::GetPassword() const {
  return password;
}
const std::string &myConnString::GetHost() const {
  return host;
}
const std::string &myConnString::GetDbname() const {
  return dbname;
}
unsigned int myConnString::GetPort() const {
  return port;
}
void myConnString::UpdateFormat() {
  formatted_string = fmt::format("postgresql://{}:{}@{}:{}/{}",
                                 user.c_str(), password.c_str(), host.c_str(), port, dbname.c_str());
}
std::string myConnString::GetVerboseName() const {
  return fmt::format("{}:{} db:{}", host, port, dbname);
}
bool myConnString::operator==(const myConnString &rhs) const {
  return user == rhs.user &&
      password == rhs.password &&
      host == rhs.host &&
      dbname == rhs.dbname &&
      port == rhs.port &&
      formatted_string == rhs.formatted_string;
}

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
ConnPtr CreateDatabase(myConnString c_string, std::string_view db_name) {

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
void DropDatabase(myConnString c_string, std::string_view db_name) {
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
std::string PaginateRequest(std::string_view initial_request, IndexType page_num, IndexType page_size) {
  return fmt::format("{} LIMIT {} OFFSET {}",
                     initial_request,
                     page_size,
                     (page_num - 1) * page_size);
}

}