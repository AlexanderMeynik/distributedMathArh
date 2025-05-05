#include "network_shared/DbService.h"

#include <stdexcept>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <iostream>

namespace db_service {

DbService::DbService() : conn_str_(), conn_(nullptr) {

}

DbService::DbService(const myConnString &conn_str) : conn_str_(conn_str), conn_(nullptr) {

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

void DbService::ExecuteTransaction(const std::function<void(TransactionT &)> &func) {
  ::db_common::ExecuteTransaction(conn_,func,service_name,conn_str_);
}
void DbService::ExecuteSubTransaction(TransactionT &txn,
                                      const std::function<void(Subtransction &)> &func,
                                      std::string_view sub_name) {
  ::db_common::ExecuteSubTransaction(txn,func,sub_name);

}

IndexType DbService::CreateUser(const std::string &login, const std::string &password, const std::string &role) {
  IndexType user_id;
  ExecuteTransaction([&](TransactionT &txn) {

    std::string qq = "INSERT INTO \"User\" (login, hashed_password, role)"
                     " VALUES ({}, {}, {})"
                     " RETURNING user_id";
    user_id = txn.exec(
        fmt::format(fmt::runtime(qq), txn.quote(login), txn.quote(password), txn.quote(role))
    ).one_row()[0].as<IndexType>();
  });
  return user_id;
}

bool DbService::AuthenticateUser(const std::string &login, const std::string &password) {
  bool authenticated = false;
  ExecuteTransaction([&](TransactionT &txn) {

    std::string qq = "SELECT hashed_password FROM \"User\" WHERE login = {}";
    auto result = txn.exec(fmt::format(fmt::runtime(qq), txn.quote(login))).one_row();
    if (result[0].as<std::string>() == password) {
      authenticated = true;
      std::string qq2 = "UPDATE \"User\" SET last_login = NOW() WHERE login = {}";
      txn.exec(fmt::format(fmt::runtime(qq2), txn.quote(login)));
    }
  });
  return authenticated;
}

void DbService::UpdateUserRole(IndexType user_id, const std::string &new_role) {
  ExecuteTransaction([&](TransactionT &txn) {
    std::string qq = "UPDATE \"User\" SET role = {} WHERE user_id = {}";
    txn.exec(fmt::format(fmt::runtime(qq), txn.quote(new_role), txn.quote(user_id)));
  });
}

void DbService::DeleteUser(IndexType user_id) {
  ExecuteTransaction([&](TransactionT &txn) {
    std::string qq = "DELETE FROM \"User\" WHERE user_id = {}";
    txn.exec(fmt::format(fmt::runtime(qq), user_id));
  });
}

IndexType DbService::CreateExperiment(IndexType user_id, const Json::Value &parameters) {
  IndexType experiment_id;
  ExecuteTransaction([&](TransactionT &txn) {
    Json::StreamWriterBuilder builder;
    std::string params_str = Json::writeString(builder, parameters);

    std::string qq = "INSERT INTO \"Experiment\" (user_id, status, parameters)"
                     " VALUES ({}, 'pending', {}::jsonb)"
                     " RETURNING experiment_id";
    experiment_id = txn.exec(
        fmt::format(fmt::runtime(qq), user_id, txn.quote(params_str))
    ).one_row()[0].as<IndexType>();

    InnerLog(txn,1, "info", fmt::format("User {} created experiment ", user_id, experiment_id));
  });
  return experiment_id;
}

void DbService::UpdateExperimentStatus(IndexType experiment_id, const std::string &status) {
  ExecuteTransaction([&](TransactionT &txn) {

    std::string qq = "UPDATE \"Experiment\" SET status = {}, start_time = CASE WHEN"
                     " {} = 'running' THEN NOW() ELSE start_time END,"
                     " end_time = CASE WHEN {} IN ('completed', 'failed')"
                     " THEN NOW() ELSE end_time END WHERE experiment_id = {}";
    txn.exec(fmt::format(fmt::runtime(qq), txn.quote(status), txn.quote(status), experiment_id));

    InnerLog(txn,0, "info", fmt::format("Experiment {}  status updated to {}", experiment_id, status));
  });
}

Json::Value DbService::GetExperiment(IndexType experiment_id) {
  Json::Value result;
  ExecuteTransaction([&](TransactionT &txn) {
    std::string qq = "SELECT * FROM \"Experiment\" WHERE experiment_id = {}";
    auto row = txn.exec(fmt::format(fmt::runtime(qq), experiment_id)).one_row();
    result["experiment_id"] = row["experiment_id"].as<std::string>();
    result["user_id"] = row["user_id"].as<std::string>();
    result["status"] = row["status"].as<std::string>();
    Json::CharReaderBuilder readerBuilder;
    std::unique_ptr<Json::CharReader> reader(readerBuilder.newCharReader());
    std::string params_str = row["parameters"].as<std::string>();
    Json::Value params;
    reader->parse(params_str.c_str(), params_str.c_str() + params_str.length(), &params, nullptr);
    result["parameters"] = params;
    if (!row["start_time"].is_null()) result["start_time"] = row["start_time"].as<std::string>();
    if (!row["end_time"].is_null()) result["end_time"] = row["end_time"].as<std::string>();
  });
  return result;
}

IndexType DbService::CreateIteration(IndexType experiment_id,
                                     IndexType node_id,
                                     const std::string &iter_type) {
  IndexType iteration_id;
  //todo use stream api
  ExecuteTransaction([&](TransactionT &txn) {
    std::string qq = "INSERT INTO \"Iteration\" (experiment_id, node_id, iter_t, status)"
                     " VALUES ({}, {}, {}, 'pending')"
                     " RETURNING iteration_id";
    iteration_id = txn.exec(
        fmt::format(fmt::runtime(qq), experiment_id, node_id, txn.quote(iter_type))
    ).one_row()[0].as<IndexType>();

    InnerLog(txn,node_id, "info", fmt::format("Iteration {} created for experiment {}", iteration_id, experiment_id));
  });
  return iteration_id;
}

void DbService::UpdateIterationStatus(IndexType iteration_id,
                                      const std::string &status,
                                      const Json::Value &output_data) {
  ExecuteTransaction([&](TransactionT &txn) {

    if (output_data.isNull()) {

      std::string qq = "UPDATE \"Iteration\" SET status = {}, end_time = CASE WHEN {} IN ('completed', 'failed')"
                       " THEN NOW() ELSE end_time END "
                       " WHERE iteration_id = { }";

      txn.exec(fmt::format(fmt::runtime(qq), txn.quote(status), txn.quote(status), iteration_id));
    } else {
      Json::StreamWriterBuilder builder;
      std::string output_str = Json::writeString(builder, output_data);

      std::string qq = "UPDATE \"Iteration\" SET status = {}, output_data = {}::jsonb, end_time ="
                       " CASE WHEN {} IN ('completed', 'failed') "
                       "THEN NOW() ELSE end_time END "
                       "WHERE iteration_id = {}";

      txn.exec(fmt::format(fmt::runtime(qq), txn.quote(status),
                           txn.quote(output_str), txn.quote(status)
                           , iteration_id));
    }
    InnerLog(txn,0, "info", fmt::format("Iteration {} status updated to {}", iteration_id, status));

  });
}

Json::Value DbService::GetIteration(IndexType iteration_id) {
  Json::Value result;
  ExecuteTransaction([&](TransactionT &txn) {
    std::string qq = "SELECT * FROM \"Iteration\" WHERE iteration_id = {}";
    auto row = txn.exec(fmt::format(fmt::runtime(qq), iteration_id)).one_row();
    result["iteration_id"] = row["iteration_id"].as<std::string>();
    result["experiment_id"] = row["experiment_id"].as<std::string>();
    result["node_id"] = row["node_id"].as<std::string>();
    result["iter_t"] = row["iter_t"].as<std::string>();
    result["status"] = row["status"].as<std::string>();
    if (!row["output_data"].is_null()) {
      Json::CharReaderBuilder readerBuilder;
      std::unique_ptr<Json::CharReader> reader(readerBuilder.newCharReader());
      std::string output_str = row["output_data"].as<std::string>();
      Json::Value output;
      reader->parse(output_str.c_str(), output_str.c_str() + output_str.length(), &output, nullptr);
      result["output_data"] = output;
    }
  });
  return result;
}

IndexType DbService::RegisterNode(const std::string &ip_address, double benchmark_score) {

  IndexType node_id;
  ExecuteTransaction([&](TransactionT &txn) {

    std::string qq = "INSERT INTO \"Node\" (ip_address, benchmark_score, status, last_ping)"
                     " VALUES ({}, {}, 'active', NOW())"
                     "RETURNING node_id";

    node_id = txn.exec(
        fmt::format(fmt::runtime(qq), txn.quote(ip_address), benchmark_score)
    ).one_row()[0].as<IndexType>();

    InnerLog(txn,node_id, "info", fmt::format("Node registered with IP {}", ip_address));
  });
  return node_id;
}

void DbService::UpdateNodeStatus(IndexType node_id, const std::string &status) {
  ExecuteTransaction([&](TransactionT &txn) {
    std::string qq = "UPDATE \"Node\" SET status = {}, last_ping = NOW() WHERE node_id = {}";
    txn.exec(fmt::format(fmt::runtime(qq), txn.quote(status), node_id));
    InnerLog(txn,node_id, "info", "Node status updated to " + status);
  });
}

void DbService::UnregisterNode(IndexType node_id) {
  ExecuteTransaction([&](TransactionT &txn) {
    std::string qq = "UPDATE \"Node\" SET status = 'inactive', last_ping = NOW() WHERE node_id = {}";
    txn.exec(fmt::format(fmt::runtime(qq), node_id));

    InnerLog(txn,node_id, "info", fmt::format("Node {} unregistered.", node_id));
  });
}

Json::Value DbService::GetNode(IndexType node_id) {
  Json::Value result;
  ExecuteTransaction([&](TransactionT &txn) {
    auto row = txn.exec_params("SELECT * FROM \"Node\" WHERE node_id = $1", node_id).one_row();
    result["node_id"] = row["node_id"].as<std::string>();
    result["ip_address"] = row["ip_address"].as<std::string>();
    result["benchmark_score"] = row["benchmark_score"].as<double>();
    result["status"] = row["status"].as<std::string>();
  });
  return result;
}

void DbService::Log(IndexType node_id, const std::string &severity, const std::string &message) {
  ExecuteTransaction([&](TransactionT &txn) {
    std::string qq= "INSERT INTO \"Log\" (node_id, severity, message) VALUES ({}, {}, {})";
    txn.exec(
        fmt::format(fmt::runtime(qq),node_id, txn.quote(severity), txn.quote(message))
    );
  });

  //std::cout << "Node: " << node_id << " - " << message;

}

void DbService::InnerLog(TransactionT &txn,
                         IndexType node_id,
                         const std::string &severity,
                         const std::string &message) {

  ExecuteSubTransaction(txn,[&](Subtransction &s)
  {
    std::string qq= "INSERT INTO \"Log\" (node_id, severity, message) VALUES ({}, {}, {})";
    s.exec(
        fmt::format(fmt::runtime(qq),node_id, txn.quote(severity), txn.quote(message))
    );
  },"Log");
}

const myConnString &DbService::GetConnStr() const {
  return conn_str_;
}
void DbService::SetConnStr(const myConnString &conn_str) {
  conn_str_ = conn_str;
}
void DbService::Reconnect() {
  if (CheckConnection(conn_)) {
    throw Already_Connected(service_name, conn_str_.GetVerboseName());
  }
  conn_ = TryConnect(conn_str_, service_name);
}


} // namespace db_service