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
  auto conn= TryConnect(c_string,"Outer");
  ExecuteTransaction(conn,[&](TransactionT &txn) {
    txn.exec(script);
  },"Outer",c_string);
}
void ExecuteTransaction(ConnPtr &ptr,
                        const std::function<void(TransactionT &)> &func,
                        std::string_view service_name,
                        const myConnString &conn_str) {
  if (!CheckConnection(ptr)) {
    throw Broken_Connection(service_name, conn_str.GetVerboseName());
  }
  TransactionT txn(*ptr);
  try {
    func(txn);
    txn.commit();
  }
  catch (const pqxx::sql_error &e) {
    throw SQL_ERROR(e.what(), e.query(), e.sqlstate());
  }
  catch (const std::exception &e) {
    throw shared::MyException(fmt::format("Some other error {} {}:{}",
                                          e.what(),__FILE__, __LINE__), shared::Severity::info);
  }
}
void ExecuteSubTransaction(TransactionT &txn,
                           const std::function<void(Subtransaction &)> &func,
                           std::string_view sub_name) {


  Subtransaction s(txn, sub_name);
  try {
    func(s);
    s.commit();
  }
  catch (const pqxx::sql_error &e) {
    throw SQL_ERROR(e.what(), e.query(), e.sqlstate());
  }
  catch (const std::exception &e) {
    throw shared::MyException(fmt::format("Some other error {} {}:{}",
                                          e.what(),__FILE__, __LINE__), shared::Severity::info);
  }
}

}