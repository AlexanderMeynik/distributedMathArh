#pragma once

#include <pqxx/pqxx>
#include "network_shared/networkTypes.h"
#include "common/Printers.h"
#include "common/Parsers.h"
#include "parallelUtils/timingUtils.h"


/// Namespace that contains database related utils
namespace db_common {
using namespace enum_utils;
using network_types::myConnString;
using namespace timing;

using ConnPtr = std::shared_ptr<pqxx::connection>;
using TransactionT = pqxx::transaction<pqxx::isolation_level::read_committed>;
using NonTransType = pqxx::nontransaction;
using ResType = pqxx::result;
using Subtransaction = pqxx::subtransaction;

using IndexType = uint64_t;
using shared::SQL_ERROR;
using shared::Already_Connected;
using shared::Broken_Connection;
static const char *const SampleTempDb = "template1";

enum class UserRole {
  USER,///< default user role
  ADMINISTRATOR ///< administrator user role
};

static const std::vector<EnumMapping<UserRole>> kUserRoleMappings = {
    {UserRole::USER, "user"},
    {UserRole::ADMINISTRATOR, "admin"},

};
static const auto kStrToUserRole =
    createStrToEnumMap(kUserRoleMappings);
const auto kUserRoleToStr=
    createEnumToStrMap(kUserRoleMappings);


enum class ExperimentStatus {
  PENDING,///< waiting to be executed
  RUNNING, ///< currently executing
  COMPLETED, ///< all iteration were computed
  ERROR, ///< error occurred during execution
  ARHIVED ///< experiment data is unloaded
};

static const std::vector<EnumMapping<ExperimentStatus>> kExpStatusMappings = {
    {ExperimentStatus::PENDING, "pending"},
    {ExperimentStatus::RUNNING, "running"},
    {ExperimentStatus::COMPLETED, "completed"},
    {ExperimentStatus::ERROR, "failed"},
    {ExperimentStatus::ARHIVED, "archived"},
};
static const auto kStrToExpStatus =
    createStrToEnumMap(kExpStatusMappings);
const auto kExpStatusToStr=
    createEnumToStrMap(kExpStatusMappings);


enum class IterationStatus {
  PENDING,///< waiting to be executed
  COMPLETED, ///< all iteration were computed
  ERROR, ///< error occurred during execution
};

static const std::vector<EnumMapping<IterationStatus>> kIterStatusMappings = {
    {IterationStatus::PENDING, "pending"},
    {IterationStatus::COMPLETED, "completed"},
    {IterationStatus::ERROR, "failed"},
};
static const auto kStrToIterStatus =
    createStrToEnumMap(kIterStatusMappings);
const auto kIterStatusToStr=
    createEnumToStrMap(kIterStatusMappings);

enum class IterationType {
  GENERATE,///< generate coordinates
  SOLVE, ///< solve system of equations
  MESH, ///< generate mesh
};

static const std::vector<EnumMapping<IterationType>> kIterTypeMappings = {
    {IterationType::GENERATE, "generate"},
    {IterationType::SOLVE, "solve"},
    {IterationType::MESH, "mesh"},
};
static const auto kStrToIterType =
    createStrToEnumMap(kIterTypeMappings);
const auto kIterTypeToStr=
    createEnumToStrMap(kIterTypeMappings);


enum class NodeStatus {
  ACTIVE,///< node is processing messages
  BUSY, ///< node is busy
  INACTIVE, ///< node is not listening to queue
  ERROR, ///< error occurred in node
};

static const std::vector<EnumMapping<NodeStatus>> kNodeStMappings = {
    {NodeStatus::ACTIVE, "active"},
    {NodeStatus::BUSY, "busy"},
    {NodeStatus::INACTIVE, "inactive"},
    {NodeStatus::ERROR, "error"},
};
static const auto kStrToNodeSt =
    createStrToEnumMap(kNodeStMappings);
const auto kNodeStToStr=
    createEnumToStrMap(kNodeStMappings);

/**
 * @brief User DAO
 */
struct User {
  IndexType user_id;
  std::string login;
  std::string hashed_password;
  UserRole role;
  IndexType created_at;
  std::optional<IndexType> last_login;

  User() = default;
  User(pqxx::row &row);
  bool operator==(const User &rhs) const;
};

/**
 * @brief Experiment DAO
 */
struct Experiment {
  IndexType experiment_id;
  IndexType user_id;
  ExperimentStatus status;
  Json::Value parameters;
  TimepointType created_at;
  std::optional<TimepointType> start_time;
  std::optional<TimepointType> end_time;

  Experiment() = default;
  Experiment(pqxx::row &row);
  bool operator==(const Experiment &rhs) const;
};

/**
 * @brief Iteration DAO
 */
struct Iteration {
  IndexType iteration_id;
  IndexType experiment_id;
  IndexType node_id;
  IterationType iter_t;
  IterationStatus status;
  Json::Value output_data;
  TimepointType start_time;
  std::optional<TimepointType> end_time;

  Iteration() = default;
  Iteration(pqxx::row &row);
  bool operator==(const Iteration &rhs) const;
};

/**
 * @brief Node DAO
 */
struct Node {
  IndexType node_id;
  std::string ip_address;
  shared::BenchResVec benchmark_score;
  NodeStatus status;
  std::optional<TimepointType> last_ping;

  Node() = default;
  Node(pqxx::row &row);
  bool operator==(const Node &rhs) const;
};


/**
 * @brief Checks connection
 * @param conn_ptr
 * @return conenction status
 */
bool CheckConnection(const ConnPtr &conn_ptr);

/**
 * @brief Closes connection and reset conn_ptr
 * @param conn_ptr
 */
void Disconnect(ConnPtr &conn_ptr);

/**
 * @brief Attempt to connect to PostgrSQL with conn_str
 * @param conn_str
 * @param service_name
 * @return connection pointer
 * @throws Broken_Connection - if connection cannot be established
 */
ConnPtr TryConnect(const myConnString &conn_str,
                   const std::string service_name);

/**
 * @brief Checks db_name database existence
 * @param non_trans
 * @param db_name
 * @return Number of databases with name db_name
 */
size_t CheckDatabaseExistence(NonTransType &non_trans,
                              std::string_view db_name);

/**
 * @brief terminates all connections for the specified database
 * @param no_trans_exec
 * @param db_name
 * @return ResType for the specified terminate requests
 */
ResType
TerminateAllDbConnections(NonTransType &no_trans_exec,
                          std::string_view db_name);

/**
 * @brief Creates database with db_name
 * @param db_name
 * @param c_string
 * @return connection string pointer for the created database
 */
ConnPtr CreateDatabase(network_types::myConnString c_string, std::string_view db_name);

/**
 * @brief Drops specified database
 * @details Will call TerminateAllDbConnections to end all other connections
 * @param db_name
 * @param c_string
 */
void DropDatabase(myConnString c_string, std::string_view db_name);

/**
 * @brief Executes the script ot inidb
 * @param c_string
 * @param script
 */
void FillDatabase(myConnString c_string, std::string_view script);

/**
 * @brief Executes function in the form of transaction
 * @details Creates new transaction to execute provided function
 * @param ptr
 * @param func
 * @param service_name
 * @param conn_str
 * @throws SQL_ERROR if any sql syntax or semantics issue arises
 * @throws MyException if any other error happens
 */
ResType ExecuteTransaction(ConnPtr &ptr,
                           const std::function<ResType(TransactionT &)> &func,
                           std::string_view service_name,
                           const myConnString &conn_str);

/**
 * @brief Executes function inside subtransaction of txn
 * @param txn
 * @param func
 * @param sub_name - name for subtransaction
 * @throws SQL_ERROR if any sql syntax or semantics issue arises
 * @throws MyException if any other error happens
 */
ResType ExecuteSubTransaction(TransactionT &txn,
                              const std::function<ResType(Subtransaction &)> &func,
                              std::string_view sub_name = "");

}