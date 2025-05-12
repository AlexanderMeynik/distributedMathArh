#pragma once
#include "network_shared/dbCommon.h"
#include <pqxx/pqxx>

namespace db_common
{

enum class UserRole {
  USER,///< default user role
  ADMINISTRATOR ///< administrator user role
};

static const std::vector<EnumMapping<UserRole>> kUserRoleMappings = {
    {UserRole::USER, "user"},
    {UserRole::ADMINISTRATOR, "admin"},

};
static const auto kStrToUserRole =
    CreateStrToEnumMap(kUserRoleMappings);
const auto kUserRoleToStr=
    CreateEnumToStrMap(kUserRoleMappings);


enum class ExperimentStatus {
  CREATED,///< created by user
  QUEUED,///< waiting to be executed
  RUNNING, ///< currently executing
  SUCCEEDED, ///< all iteration were computed
  ERROR, ///< error occurred during execution
  ARHIVED ///< experiment data is unloaded
};

static const std::vector<EnumMapping<ExperimentStatus>> kExpStatusMappings = {
    {ExperimentStatus::CREATED, "created"},
    {ExperimentStatus::QUEUED, "queued"},
    {ExperimentStatus::RUNNING, "running"},
    {ExperimentStatus::SUCCEEDED, "succeeded"},
    {ExperimentStatus::ERROR, "error"},
    {ExperimentStatus::ARHIVED, "archived"},
};
static const auto kStrToExpStatus =
    CreateStrToEnumMap(kExpStatusMappings);
const auto kExpStatusToStr=
    CreateEnumToStrMap(kExpStatusMappings);


enum class IterationStatus {
  QUEUED,///< waiting to be executed
  RUNNING,///< currently executing
  SUCCEEDED, ///< all iteration were computed
  ERROR, ///< error occurred during execution
};

static const std::vector<EnumMapping<IterationStatus>> kIterStatusMappings = {
    {IterationStatus::QUEUED, "queued"},
    {IterationStatus::RUNNING, "running"},
    {IterationStatus::SUCCEEDED, "succeeded"},
    {IterationStatus::ERROR, "error"},
};
static const auto kStrToIterStatus =
    CreateStrToEnumMap(kIterStatusMappings);
const auto kIterStatusToStr=
    CreateEnumToStrMap(kIterStatusMappings);

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
    CreateStrToEnumMap(kIterTypeMappings);
const auto kIterTypeToStr=
    CreateEnumToStrMap(kIterTypeMappings);


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
    CreateStrToEnumMap(kNodeStMappings);
const auto kNodeStToStr=
    CreateEnumToStrMap(kNodeStMappings);

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
 * @brief Log DAO
 */
struct Log {
  IndexType log_id;
  std::optional<IndexType> node_id;
  std::optional<IndexType> experiment_id;
  shared::Severity severity;
  std::string message;
  TimepointType timestamp;

  Log() = default;
  Log(pqxx::row &row);
  bool operator==(const Log &rhs) const;
};


};