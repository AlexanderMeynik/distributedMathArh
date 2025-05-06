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
  DbService(const myConnString &conn_str);
  ~DbService();

  bool Connect();
  void Disconnect();
  bool IsConnected() const;

  IndexType CreateUser(std::string_view login, std::string_view password, std::string_view role = "user");
  IndexType CreateUser(const User &user);
  bool AuthenticateUser(std::string_view login, std::string_view password);
  bool AuthenticateUser(const User &user);

  void DeleteUser(IndexType user_id);
  std::vector<User> GetUsers(IndexType page_num, IndexType page_size = 50);

  IndexType CreateExperiment(IndexType user_id, const Json::Value &parameters);
  void UpdateExperimentStatus(IndexType experiment_id, std::string_view status);
  Json::Value GetExperiment(IndexType experiment_id);

  IndexType CreateIteration(IndexType experiment_id, IndexType node_id, std::string_view iter_type);
  void UpdateIterationStatus(IndexType iteration_id,
                             std::string_view status,
                             const Json::Value &output_data = Json::Value());
  Json::Value GetIteration(IndexType iteration_id);

  IndexType RegisterNode(std::string_view ip_address, const shared::BenchResVec &benchmark_score);
  void UpdateNodeStatus(IndexType node_id, std::string_view status);
  void UnregisterNode(IndexType node_id);
  Json::Value GetNode(IndexType node_id);

  void Log(IndexType node_id, std::string_view severity, std::string_view message);
  [[nodiscard]] const myConnString &GetConnStr() const;
  void SetConnStr(const myConnString &conn_str);

  ResType ExecuteTransaction(const std::function<ResType(TransactionT &)> &func);

  ResType ExecuteSubTransaction(TransactionT &txn,
                                const std::function<ResType(Subtransaction &)> &func,
                                std::string_view sub_name = "");

 private:
  myConnString conn_str_;

  void Reconnect();

  ConnPtr conn_;

  void InnerLog(TransactionT &txn, IndexType node_id, std::string_view severity, std::string_view message);

  static inline const char *service_name = "DbService";
};

} // namespace db_service