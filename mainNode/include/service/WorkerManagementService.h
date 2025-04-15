#pragma once

#include <drogon/HttpClient.h>
#include <drogon/HttpRequest.h>

#include "common/sharedDeclarations.h"
#include "common/Parsers.h"
#include "common/Printers.h"

/// Namespace for services used in main node
namespace main_services {

using namespace drogon;
using shared::BenchResVec;
using shared::BenchResultType;
/// nodeStatus Enum
enum class NodeStatus {
  /// Node is connected to cluster and is ready to receive it's tasks
  ACTIVE,
  /// Node is present in cluster but is not ready to recieve tasks
  INACTIVE,
  /// An error occurred moving node to a failed state
  FAILED
};

/// Look-up table to cast nodeStatus to string
const std::unordered_map<const NodeStatus, std::string> kNodeStatusToStr
    {
        {NodeStatus::ACTIVE, "Active"},
        {NodeStatus::INACTIVE, "Inactive"},
        {NodeStatus::FAILED, "Failed"},
    };

/**
 * @brief Computational node class
 */
class ComputationalNode {
 public:
  HttpClientPtr http_client_;

  std::string GetPath() {
    return http_client_->getHost() + ":" + std::to_string(http_client_->getPort());
  }
  NodeStatus st_;
  BenchResVec power_;
  BenchResVec w_;
};

class WorkerManagementService {
  using NodeStorageType = std::unordered_map<NodeStatus, std::unordered_map<std::string, ComputationalNode>>;

 public:

  Json::Value GetStatus();

  WorkerManagementService() {
    normalized_.resize(9, 0);
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
  static constexpr inline BenchResultType max_ = 1000'000'000'000ULL;
};

}