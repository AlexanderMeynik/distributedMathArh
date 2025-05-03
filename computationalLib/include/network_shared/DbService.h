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
  bool connect();
  void disconnect();
  bool isConnected() const;

  // User management
  std::string createUser(const std::string& login, const std::string& password, const std::string& role = "user");
  bool authenticateUser(const std::string& login, const std::string& password);
  void updateUserRole(const std::string& user_id, const std::string& new_role);
  void deleteUser(const std::string& user_id);

  // Experiment management
  std::string createExperiment(const std::string& user_id, const Json::Value& parameters);
  void updateExperimentStatus(const std::string& experiment_id, const std::string& status);
  Json::Value getExperiment(const std::string& experiment_id);

  // Iteration management
  std::string createIteration(const std::string& experiment_id, const std::string& node_id, const std::string& iter_type);
  void updateIterationStatus(const std::string& iteration_id, const std::string& status, const Json::Value& output_data = Json::Value());
  Json::Value getIteration(const std::string& iteration_id);

  // Node management
  std::string registerNode(const std::string& ip_address, double benchmark_score);
  void updateNodeStatus(const std::string& node_id, const std::string& status);
  void unregisterNode(const std::string& node_id);
  Json::Value getNode(const std::string& node_id);

  // Logging
  void log(const std::string& node_id, const std::string& severity, const std::string& message);

 private:
  std::string conn_str_;
  conn_ptr conn_;

  // Helper methods
  std::string generateUUID();
  void ensureConnection();
  void executeTransaction(const std::function<void(transaction_t&)>& func);
  void initializeDatabase();
};

} // namespace db_service