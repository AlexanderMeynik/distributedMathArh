#pragma once

#include <algorithm>

#include <drogon/HttpClient.h>
#include <drogon/HttpRequest.h>

#include "common/sharedDeclarations.h"
#include "common/Parsers.h"
#include "common/Printers.h"

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
 */
class ComputationalNode {
 public:
  HttpClientPtr http_client_;

  std::string GetPath() {
    return http_client_->getHost() + ":" + std::to_string(http_client_->getPort());
  }

  /**
   * @brief Parses and recomputes node performance metrics
   * @param val - json file to parse bench results from
   */
  void RecomputeCoefficients(const Json::Value &val) {
    bench_result_ = print_utils::JsonToContinuous<BenchResVec>(val, true, true);
    if (node_speed_.size() == 0) {
      node_speed_ = kBenchInfinity / bench_result_;
    }
  }

  NodeStatus st_;
  BenchResVec bench_result_;
  BenchResVec node_speed_;
};

class WorkerManagementService {
  using NodeStorageType = std::unordered_map<NodeStatus, std::unordered_map<std::string, ComputationalNode>>;

 public:

  Json::Value GetStatus();

  WorkerManagementService() {
    normalized_ = DefaultBench(0);
  }

  Json::Value AddNewNode(const std::string &host_port);

  Json::Value ConnectNode(const std::string &host_port,
                          drogon::HttpRequestPtr req1);

  Json::Value DisconnectNode(const std::string &host_port);

  auto begin() {
    return worker_nodes_[NodeStatus::ACTIVE].begin();
  }

  auto end() {
    return worker_nodes_[NodeStatus::ACTIVE].end();
  }

  const BenchResVec GetSum() const;

 private:
  void MoveNode(const std::string &host_port,
                const NodeStatus &from,
                const NodeStatus &to);

  NodeStorageType worker_nodes_;
  std::string q_host_;

  std::valarray<uint64_t> normalized_;
};

}