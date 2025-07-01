#pragma once

#include "common/Parsers.h"
#include "common/Printers.h"

#include <drogon/HttpClient.h>
#include <drogon/HttpRequest.h>

/// Namespace for services used in main node
namespace main_services {

using namespace drogon;
using namespace shared;

/// nodeStatus Enum
enum class NodeStatus {
  ACTIVE,///< Node is connected to cluster and is ready to receive it's tasks
  INACTIVE,///< Node is present in cluster but is not ready to recieve tasks
  FAILED///< An error occurred moving node to a failed state
};

/// Look-up table to cast nodeStatus to string
const std::unordered_map<const NodeStatus, std::string> kNodeStatusToStr
    {
        {NodeStatus::ACTIVE, "Active"},
        {NodeStatus::INACTIVE, "Inactive"},
        {NodeStatus::FAILED, "Failed"},
    };

/// This variable is used as large bench value to determine node "speed"
static constexpr inline BenchResultType kBenchInfinity = 1000'000'000'000ULL;

/**
 * @brief Computational node class
 *
 */
class ComputationalNode {
 public:
  /// Drogon return code and Response pair
  using RequestResult=std::pair<ReqResult,HttpResponsePtr>;


  std::string to_string();

  /**
   * @brief Parses and recomputes node performance metrics
   * @param val - json file to parse bench results from
   */
  void RecomputeCoefficients(const Json::Value &val);

  /**
   * @brief Syntax sugar to send request
   * @param ptr
   * @return RequestResult
   */
  RequestResult inline PerformHttpRequest(HttpRequestPtr ptr)
  {
    return http_client_->sendRequest(ptr);
  }

  HttpClientPtr http_client_; ///< http client used to send requests
  NodeStatus st_;
  BenchResVec bench_result_;///< time measurements of worker performance
  BenchResVec node_speed_;///< weighed speed measure of node
};

/**
 * @class WorkerManagementService
 * @brief Class for worker node enumeration and management
 * @details This class is dedicated to store and manage worker nodes information.
 * Currently only Active(connected and running) nodes are used for message processing.
 */
class WorkerManagementService {
  using NodeStorageType = std::unordered_map<NodeStatus, std::unordered_map<std::string, ComputationalNode>>;

 public:

  /**
   * @brief returns list of lists with all nodes meta information
   * @return Json::Value
   */
  Json::Value GetStatus();

  /**
   *
   */
  WorkerManagementService();

  /**
   * @brief Upsets new node into node list
   * This method doesn't perform connection operation, but is used to update node information.
   * @param host_port
   * @return empty Json::Value
   * @return Json:: Value with "status"=409 - if node exists and is Active
   */
  Json::Value AddNewNode(const std::string &host_port);

  /**
   * @brief Connects node to computational cluster
   * This method performs http request to connect worker
   * and retrieve benchmark results.
   * Successful request will result in node bench reassignment
   * @param host_port
   * @param req1
   * @return Json::Value with "node_output" that contains results
   * @return Json:: Value with "status"=504 - if worker cannot be accessed for specified host_port
   * @return Json:: Value with "status">=400 - if any abnormal http code is received
   */
  Json::Value ConnectNode(const std::string &host_port,
                          drogon::HttpRequestPtr req1);

  /**
   * @brief Disconnects worker node from cluster
   * This method calls disconnect http request
   * @param host_port
   * @return Json::Value with node Output
   * @return Json:: Value with "status"=409 - if there is no Active node with specified host and port
   * @return Json:: Value with "status"=504 - if worker cannot be accessed for specified host_port
   * @return Json:: Value with "status">=400 - if any abnormal http code is received
   */
  Json::Value DisconnectNode(const std::string &host_port);

  /**
   *
   * @return begin iterator to map of Active nodes
   * @see NodeStatus::ACTIVE
   */
  auto begin() {
    return worker_nodes_[NodeStatus::ACTIVE].begin();
  }

  /**
   *
   * @return end iterator to map of Active nodes
   * @see NodeStatus::ACTIVE
   */
  auto end() {
    return worker_nodes_[NodeStatus::ACTIVE].end();
  }

  /**
   *
   * @return normalization coefficients for benchmark speed results
   */
  const BenchResVec GetSum() const;

 private:
  /**
   * Moves node between map entries
   * @param host_port
   * @param from
   * @param to
   */
  void MoveNode(const std::string &host_port,
                const NodeStatus &from,
                const NodeStatus &to);

  NodeStorageType worker_nodes_; ///< map of maps to manage worker nodes
  std::string q_host_; ///< queue host

  std::valarray<uint64_t> normalized_;/// < normalization coefficient for all active nodes
};

}