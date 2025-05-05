#pragma once

#include "dbCommon.h"
#include <string>
#include <memory>
#include <vector>
#include <json/json.h>

/// Namespace that contains database services
namespace db_service {
using namespace db_common;

class DbService {
 public:
  DbService();
  DbService(const myConnString& conn_str);
  ~DbService();

  bool Connect();
  void Disconnect();
  bool IsConnected() const;

  IndexType  CreateUser(const std::string& login, const std::string& password, const std::string& role = "user");
  bool AuthenticateUser(const std::string& login, const std::string& password);
  void UpdateUserRole(IndexType user_id, const std::string& new_role);
  void DeleteUser(IndexType user_id);

  IndexType CreateExperiment(IndexType user_id, const Json::Value& parameters);
  void UpdateExperimentStatus(IndexType experiment_id, const std::string& status);
  Json::Value GetExperiment(IndexType experiment_id);

  IndexType CreateIteration(IndexType experiment_id, IndexType node_id, const std::string& iter_type);
  void UpdateIterationStatus(IndexType iteration_id, const std::string& status, const Json::Value& output_data = Json::Value());
  Json::Value GetIteration(IndexType iteration_id);


  IndexType RegisterNode(const std::string& ip_address, double benchmark_score);
  void UpdateNodeStatus(IndexType node_id, const std::string& status);
  void UnregisterNode(IndexType node_id);
  Json::Value GetNode(IndexType node_id);

  void Log(IndexType node_id, const std::string& severity, const std::string& message);
  [[nodiscard]] const myConnString &GetConnStr() const;
  void SetConnStr(const myConnString &conn_str);
 private:
  myConnString conn_str_;

  void Reconnect();

  ConnPtr conn_;

  void InnerLog(TransactionT &txn,IndexType node_id, const std::string& severity, const std::string& message);

  void ExecuteTransaction(const std::function<void(TransactionT&)>& func);

  void ExecuteSubTransaction(TransactionT &txn,
                             const std::function<void(Subtransction &)> &func,
                             std::string_view sub_name="");

  static inline const char * service_name="DbService";
};

} // namespace db_service