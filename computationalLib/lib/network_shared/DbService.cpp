#include "network_shared/DbService.h"

#include <stdexcept>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <iostream>

namespace db_service {

DbService::DbService(const std::string &conn_str) : conn_str_(conn_str), conn_(nullptr) {
  InitializeDatabase();
  Connect();
}

DbService::~DbService() {
  Disconnect();
}

bool DbService::Connect() {
  try {
    conn_ = std::make_shared<pqxx::connection>(conn_str_);
    return conn_->is_open();
  } catch (const std::exception &e) {
    std::cerr << "Connection failed: " << e.what() << std::endl;
    return false;
  }
}

void DbService::Disconnect() {
  if (conn_ && conn_->is_open()) {
    conn_->close();
  }
}

bool DbService::IsConnected() const {
  return conn_ && conn_->is_open();
}

void DbService::EnsureConnection() {
  if (!IsConnected()) {
    throw std::runtime_error("No active database connection");
  }
}

std::string DbService::GenerateUuid() {
  std::stringstream ss;
  auto now = std::chrono::system_clock::now().time_since_epoch().count();
  ss << std::hex << now << std::rand();
  return ss.str().substr(0, 36); //todo remove
}

void DbService::ExecuteTransaction(const std::function<void(transaction_t &)> &func) {
  EnsureConnection();//interesting workaround but exceptions nneds to be more verbose
  transaction_t txn(*conn_);
  try {
    func(txn);
    txn.commit();
  } catch (const std::exception &e) {
    std::cerr << "Transaction failed: " << e.what() << std::endl;
    throw;
  }
}

void DbService::InitializeDatabase() {
  Connect();
  //todo skip if already exist(check?)
  ExecuteTransaction([](transaction_t &txn) {
    txn.exec(R"(
            CREATE TABLE IF NOT EXISTS "User" (
                user_id UUID PRIMARY KEY,
                login VARCHAR(255) NOT NULL UNIQUE,
                hashed_password TEXT NOT NULL,
                role VARCHAR(50) NOT NULL,
                created_at TIMESTAMP DEFAULT NOW(),
                last_login TIMESTAMP
            );
            CREATE TABLE IF NOT EXISTS Experiment (
                experiment_id UUID PRIMARY KEY,
                user_id UUID NOT NULL REFERENCES "User"(user_id),
                status VARCHAR(50) NOT NULL,
                parameters JSONB NOT NULL,
                start_time TIMESTAMP,
                end_time TIMESTAMP
            );

            CREATE TABLE IF NOT EXISTS Node (
                node_id UUID PRIMARY KEY,
                ip_address INET NOT NULL,
                benchmark_score DOUBLE PRECISION,
                status VARCHAR(50) NOT NULL,
                last_ping TIMESTAMP
            );
            CREATE TABLE IF NOT EXISTS Iteration (
                iteration_id UUID PRIMARY KEY,
                experiment_id UUID NOT NULL REFERENCES Experiment(experiment_id),
                node_id UUID REFERENCES Node(node_id),
                iter_t VARCHAR(50) NOT NULL,
                status VARCHAR(50) NOT NULL DEFAULT 'pending',
                output_data JSONB,
                start_time TIMESTAMP DEFAULT NOW(),
                end_time TIMESTAMP
            );

            CREATE TABLE IF NOT EXISTS Log (
                log_id UUID PRIMARY KEY,
                node_id UUID REFERENCES Node(node_id),
                severity VARCHAR(50) NOT NULL,
                message TEXT NOT NULL,
                timestamp TIMESTAMP DEFAULT NOW()
            );
        )");
  });
}

std::string DbService::CreateUser(const std::string &login, const std::string &password, const std::string &role) {
  int user_id = 2;
  //todo uuuid fauls to be use since it's hexadecimal
  ExecuteTransaction([&](transaction_t &txn) {
    txn.exec(//todo exe params?
        "INSERT INTO \"User\" (user_id, login, hashed_password, role) VALUES ($1, $2, $3, $4)",
        pqxx::params{user_id, login, password, role}
    );
  });
  return std::to_string(user_id);
}

bool DbService::AuthenticateUser(const std::string &login, const std::string &password) {
  bool authenticated = false;
  ExecuteTransaction([&](transaction_t &txn) {
    auto result = txn.exec("SELECT hashed_password FROM \"User\" WHERE login = $1", pqxx::params{login}).one_row();
    if (!result.empty() && result[0].as<std::string>() == password) {
      authenticated = true;
      txn.exec("UPDATE \"User\" SET last_login = NOW() WHERE login = $1", pqxx::params{login});
    }
  });
  return authenticated;
}

void DbService::UpdateUserRole(const std::string &user_id, const std::string &new_role) {
  ExecuteTransaction([&](transaction_t &txn) {
    txn.exec("UPDATE \"User\" SET role = $1 WHERE user_id = $2", pqxx::params{new_role, user_id});
  });
}

void DbService::DeleteUser(const std::string &user_id) {
  ExecuteTransaction([&](transaction_t &txn) {
    txn.exec("DELETE FROM \"User\" WHERE user_id = $1", pqxx::params{user_id});
  });
}

std::string DbService::CreateExperiment(const std::string &user_id, const Json::Value &parameters) {
  std::string experiment_id = GenerateUuid();
  ExecuteTransaction([&](transaction_t &txn) {
    Json::StreamWriterBuilder builder;
    std::string params_str = Json::writeString(builder, parameters);
    txn.exec(
        "INSERT INTO Experiment (experiment_id, user_id, status, parameters) VALUES ($1, $2, 'pending', $3::jsonb)",
        pqxx::params{experiment_id, user_id, params_str}
    );
    Log("main", "info", "User " + user_id + " created experiment " + experiment_id);
  });
  return experiment_id;
}

void DbService::UpdateExperimentStatus(const std::string &experiment_id, const std::string &status) {
  ExecuteTransaction([&](transaction_t &txn) {
    txn.exec(
        "UPDATE Experiment SET status = $1, start_time = CASE WHEN $1 = 'running' THEN NOW() ELSE start_time END, "
        "end_time = CASE WHEN $1 IN ('completed', 'failed') THEN NOW() ELSE end_time END WHERE experiment_id = $2",
        pqxx::params{status, experiment_id}
    );
    Log("main", "info", "Experiment " + experiment_id + " status updated to " + status);
  });
}

Json::Value DbService::GetExperiment(const std::string &experiment_id) {
  Json::Value result;
  ExecuteTransaction([&](transaction_t &txn) {
    auto row = txn.exec("SELECT * FROM Experiment WHERE experiment_id = $1", pqxx::params{experiment_id}).one_row();
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

std::string DbService::CreateIteration(const std::string &experiment_id,
                                       const std::string &node_id,
                                       const std::string &iter_type) {
  std::string iteration_id = GenerateUuid();//todo this one can be passed as param
  //todo use stream api
  ExecuteTransaction([&](transaction_t &txn) {
    txn.exec(
        "INSERT INTO Iteration (iteration_id, experiment_id, node_id, iter_t, status) VALUES ($1, $2, $3, $4, 'pending')",
        pqxx::params{iteration_id,
                     experiment_id,
                     node_id,
                     iter_type}
    );
    Log(node_id, "info", "Iteration " + iteration_id + " created for experiment " + experiment_id);
  });
  return iteration_id;
}

void DbService::UpdateIterationStatus(const std::string &iteration_id,
                                      const std::string &status,
                                      const Json::Value &output_data) {
  ExecuteTransaction([&](transaction_t &txn) {
    if (output_data.isNull()) {
      txn.exec(
          "UPDATE Iteration SET status = $1, end_time = CASE WHEN $1 IN ('completed', 'failed') THEN NOW() ELSE end_time END "
          "WHERE iteration_id = $2",
          pqxx::params{status,
                       iteration_id}
      );
    } else {
      Json::StreamWriterBuilder builder;
      std::string output_str = Json::writeString(builder, output_data);
      txn.exec(
          "UPDATE Iteration SET status = $1, output_data = $2::jsonb, end_time = CASE WHEN $1 IN ('completed', 'failed') THEN NOW() ELSE end_time END "
          "WHERE iteration_id = $3",
          pqxx::params{status,
                       output_str,
                       iteration_id}
      );
    }
    Log("main", "info", "Iteration " + iteration_id + " status updated to " + status);
  });
}

Json::Value DbService::GetIteration(const std::string &iteration_id) {
  Json::Value result;
  ExecuteTransaction([&](transaction_t &txn) {
    auto row = txn.exec("SELECT * FROM Iteration WHERE iteration_id = $1", pqxx::params{iteration_id}).one_row();
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

std::string DbService::RegisterNode(const std::string &ip_address, double benchmark_score) {
  std::string node_id = GenerateUuid();
  ExecuteTransaction([&](transaction_t &txn) {
    txn.exec(
        "INSERT INTO Node (node_id, ip_address, benchmark_score, status, last_ping) VALUES ($1, $2, $3, 'active', NOW())",
        pqxx::params{node_id,
                     ip_address,
                     benchmark_score}
    );
    Log(node_id, "info", "Node registered with IP " + ip_address);
  });
  return node_id;
}

void DbService::UpdateNodeStatus(const std::string &node_id, const std::string &status) {
  ExecuteTransaction([&](transaction_t &txn) {
    txn.exec(
        "UPDATE Node SET status = $1, last_ping = NOW() WHERE node_id = $2",
        pqxx::params{status, node_id}
    );
    Log(node_id, "info", "Node status updated to " + status);
  });
}

void DbService::UnregisterNode(const std::string &node_id) {
  ExecuteTransaction([&](transaction_t &txn) {
    txn.exec(
        "UPDATE Node SET status = 'inactive', last_ping = NOW() WHERE node_id = $1",
        pqxx::params{node_id}
    );
    Log(node_id, "info", "Node " + node_id + " unregistered");
  });
}

Json::Value DbService::GetNode(const std::string &node_id) {
  Json::Value result;
  ExecuteTransaction([&](transaction_t &txn) {
    auto row = txn.exec("SELECT * FROM Node WHERE node_id = $1", pqxx::params{node_id}).one_row();
    result["node_id"] = row["node_id"].as<std::string>();
    result["ip_address"] = row["ip_address"].as<std::string>();
    result["benchmark_score"] = row["benchmark_score"].as<double>();
    result["status"] = row["status"].as<std::string>();
  });
  return result;
}

void DbService::Log(const std::string &node_id, const std::string &severity, const std::string &message) {
  ExecuteTransaction([&](transaction_t &txn) {
    txn.exec(
        "INSERT INTO Log (log_id, node_id, severity, message) VALUES ($1, $2, $3, $4)",
        pqxx::params{GenerateUuid(), node_id, severity, message}
    );
  });

  std::cout<<"Node: " << node_id << " - " << message;
  /*if (severity == "info") {
    LOG_INFO << "Node: " << node_id << " - " << message;
  } else if (severity == "warning") {
    LOG_WARN << "Node: " << node_id << " - " << message;
  } else if (severity == "error") {
    LOG_ERROR << "Node: " << node_id << " - " << message;
  }*/
}

} // namespace db_service