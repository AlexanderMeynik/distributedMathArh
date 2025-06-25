#pragma once

#include "daoClasses.h"
#include <memory>

/// Namespace that contains database services
namespace db_service {
using namespace db_common;

class DbService {
 public:
  DbService();
  DbService(const PostgreSQLCStr &conn_str);
  ~DbService();

  bool Connect();
  void Disconnect();
  bool IsConnected() const;

  /**
   * @brief Creates new user record
   * @param login
   * @param password
   * @param role
   * @return userd_id
   */
  IndexType CreateUser(std::string_view login, std::string_view password, std::string_view role = "user");
  IndexType CreateUser(const User &user);
  /**
   * @brief Performs authentication attempt
   * @param login
   * @param password
   * @return is_authenticated
   */
  bool AuthenticateUser(std::string_view login, std::string_view password);
  bool AuthenticateUser(const User &user);

  /**
   * @brief Deletes user record
   * @param user_id
   */
  void DeleteUser(IndexType user_id);

  /**
   * @brief Retrieves user list in paginated form
   * @note pagination starts from 1
   * @param page_num
   * @param page_size
   * @return userList
   */
  std::vector<User> ListUsers(IndexType page_num, IndexType page_size = 50);

  /**
   * @brief Create new Experiment  record
   * @param user_id
   * @param parameters
   * @return experiment_id
   */
  IndexType CreateExperiment(IndexType user_id, const Json::Value &parameters);
  IndexType CreateExperiment(Experiment exp);

  /**
   * @brief Retrieves list of Experiments for user_id
   * @note pagination starts from 1
   * @param user_id
   * @param page_num
   * @param page_size
   * @return Experiment List
   */
  std::vector<Experiment> ListExperiments(IndexType user_id,
                                          IndexType page_num,
                                          IndexType page_size = 50);
  void UpdateExperimentStatus(IndexType experiment_id,
                              std::string_view status);
  void UpdateExperimentStatus(IndexType experiment_id,
                                   ExperimentStatus status);
  /**
   * @brief Retrieves experiment using its id
   * @param experiment_id
   * @return experiment object
   */
  Experiment GetExperiment(IndexType experiment_id);

  /**
   * @brief Updated iteration status
   * @param iteration_id
   * @param status
   * @param output_data
   */
  void UpdateIterationStatus(IndexType iteration_id,
                             std::string_view status,
                             const Json::Value &output_data = Json::Value());
  void UpdateIterationStatus(IndexType iteration_id,
                             IterationStatus status,
                             const Json::Value &output_data = Json::Value());

  /**
   * @brief Creates experiment record
   * @param experiment_id
   * @param node_id
   * @param iter_type
   * @return iteration_id
   */
  IndexType CreateIteration(IndexType experiment_id,
                            IndexType node_id,
                            std::string_view iter_type);

  IndexType CreateIteration(const Iteration & iter);

  /**
   * @brief Gets iteration using its id
   * @param iteration_id
   * @return iteration
   */
  Iteration GetIteration(IndexType iteration_id);

  /**
   * @brief Retrieves iteration list for experiment_id
   * @note pagination starts from 1
   * @param experiment_id
   * @param page_num
   * @param page_size
   * @return list of Iteration
   */
  std::vector<Iteration> ListIterations(IndexType experiment_id,
                                        IndexType page_num,
                                        IndexType page_size = 50);

  /**
   * @brief Create node record
   * @param ip_address
   * @param benchmark_score
   * @return
   */
  IndexType RegisterNode(std::string_view ip_address,
                         const shared::BenchResVec &benchmark_score);
  IndexType RegisterNode(const Node & node);

  /**
   * @brief Changes node status
   * @param node_id
   * @param status
   */
  void UpdateNodeStatus(IndexType node_id, std::string_view status);
  /**
   * @brief Deletes node
   * @param node_id
   */

  void UnregisterNode(IndexType node_id);
  /**
   * @brief Retrieves node record for id
   * @param node_id
   * @return node record
   */
  Node GetNode(IndexType node_id);

  /**
   * @brief Retrieves node list in paginated format
   * @param page_num
   * @param page_size
   * @return list of nodes
   */
  std::vector<Node>ListNodes(IndexType page_num,
                             IndexType page_size = 50);


  /**
   * @brief Logs with specified message
   * @param experiment_id
   * @param node_id
   * @param severity
   * @param message
   */
  IndexType Log(std::optional<IndexType> experiment_id,
           std::optional<IndexType> node_id,
           std::string_view severity,
           std::string_view message);
  IndexType Log(const db_common::Log&log);


  /**
   * @brief Lists logs in paginated form
   * @param page_num
   * @param page_size
   * @return list of logs
   */
  std::vector<db_common::Log> ListLogs(IndexType page_num,
                                       IndexType page_size = 50);



  [[nodiscard]] const PostgreSQLCStr &GetConnStr() const;
  void SetConnStr(const PostgreSQLCStr &conn_str);

  /**
   * @brief Wrapper for the db_common::ExecuteTransaction
   * @param func
   * @return Result object
   */
  ResType ExecuteTransaction(const std::function<ResType(TransactionT &)> &func);

  /**
   * @brief Wrapper for the db_common::ExecuteSubTransaction
   * @param txn
   * @param func
   * @param sub_name
   * @return
   */
  ResType ExecuteSubTransaction(TransactionT &txn,
                                const std::function<ResType(Subtransaction &)> &func,
                                std::string_view sub_name = "");

 private:
  PostgreSQLCStr conn_str_;//< connection string for the service
  ConnPtr conn_;//< connection pointer
  /**
   * @brief implements reconnection logic and checks
   */
  void Reconnect();

  /**
   * @brief Shorthand for the service events logging
   * @param txn
   * @param experiment_id
   * @param node_id
   * @param severity
   * @param message
   * @return
   */
  IndexType InnerLog(TransactionT &txn,
                std::optional<IndexType> experiment_id,
                std::optional<IndexType> node_id,
                std::string_view severity,
                std::string_view message);

  static inline const char *service_name = "DbService";//<service name
};

} // namespace db_service