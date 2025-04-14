#pragma once

#include <drogon/HttpClient.h>
#include <drogon/HttpRequest.h>

#include "network_shared/amqpRestService.h"
#include "network_shared/AMQPPublisherService.h"
#include "common/sharedDeclarations.h"
#include "common/Parsers.h"
#include "common/Printers.h"

namespace main_service {
using namespace drogon;
using shared::BenchResVec;

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

class MainNodeService {
 public:

  MainNodeService(const std::string &user, const std::string &password) {
    auth_ = std::make_unique<JsonAuthHandler>(user, password);
    rest_service_ = std::make_unique<amqp_common::RabbitMQRestService>();

    publisher_service_ = std::make_unique<amqp_common::AMQPPublisherService>();
  }

  Json::Value Status();

  Json::Value ConnectNode(const std::string &host_port,
                          const std::string &name);

  Json::Value DisconnectNode(const std::string &host_port);

  Json::Value Connect(const std::string &qip,
                      const std::vector<std::string> &names);

  Json::Value Disconnect();

  Json::Value Publish(network_types::TestSolveParam &ts, std::string node);

  Json::Value SendToExecution(network_types::TestSolveParam &ts);
 private:
  std::unique_ptr<JsonAuthHandler> auth_;
  std::unique_ptr<amqp_common::RabbitMQRestService> rest_service_;
  std::unique_ptr<amqp_common::AMQPPublisherService> publisher_service_;
  ///todo use several maps for different qtypes
  std::unordered_map<std::basic_string<char>, ComputationalNode> worker_nodes_;
  std::string q_host_;

  std::valarray<uint64_t> normalized_;

  void RecomputeWeights();

  //todo use when tasks are ditributed
  Json::Value Publish2(network_types::TestSolveParam &ts, std::string node);

  static inline std::string vhost_ = "%2F";
};

}