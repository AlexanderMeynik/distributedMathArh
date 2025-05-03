#pragma once

#include <pqxx/pqxx>
#include <string>
#include <memory>
#include <vector>
#include <json/json.h>

namespace db_service {

using conn_ptr = std::shared_ptr<pqxx::connection>;
using transaction_t = pqxx::work;

class DbService {
 public:
  DbService(const std::string& conn_str);
  ~DbService();

  // Database connection management
  bool Connect();
  void Disconnect();
  bool IsConnected() const;

  // User management
  std::string CreateUser(const std::string& login, const std::string& password, const std::string& role = "user");
  bool AuthenticateUser(const std::string& login, const std::string& password);
  void UpdateUserRole(const std::string& user_id, const std::string& new_role);
  void DeleteUser(const std::string& user_id);

  // Experiment management
  std::string CreateExperiment(const std::string& user_id, const Json::Value& parameters);
  void UpdateExperimentStatus(const std::string& experiment_id, const std::string& status);
  Json::Value GetExperiment(const std::string& experiment_id);

  // Iteration management
  std::string CreateIteration(const std::string& experiment_id, const std::string& node_id, const std::string& iter_type);
  void UpdateIterationStatus(const std::string& iteration_id, const std::string& status, const Json::Value& output_data = Json::Value());
  Json::Value GetIteration(const std::string& iteration_id);

  // Node management
  std::string RegisterNode(const std::string& ip_address, double benchmark_score);
  void UpdateNodeStatus(const std::string& node_id, const std::string& status);
  void UnregisterNode(const std::string& node_id);
  Json::Value GetNode(const std::string& node_id);

  // Logging
  void Log(const std::string& node_id, const std::string& severity, const std::string& message);

 private:
  std::string conn_str_;
  conn_ptr conn_;

  // Helper methods
  std::string GenerateUuid();
  void EnsureConnection();
  void ExecuteTransaction(const std::function<void(transaction_t&)>& func);
  void InitializeDatabase();
};

} // namespace db_service