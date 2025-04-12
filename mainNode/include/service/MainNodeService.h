#pragma once


#include <drogon/HttpClient.h>
#include <drogon/HttpRequest.h>

#include "network_shared/amqpRestService.h"
#include "network_shared/AMQPPublisherService.h"
#include "common/sharedDeclarations.h"

namespace main_service
{

using drogon::HttpClientPtr;
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

//todo move to networ_shared?
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
  BenchResVec power_;
  NodeStatus st_;
};


class MainNodeService
{
  MainNodeService(const std::string &user,const std::string &password)
  {
    auth_=std::make_unique<BasicAuthHandler>(user,password);
    rest_service_=std::make_unique<amqp_common::RabbitMQRestService>();
  }
  void Connect(const std::string &qip,
               const std::string &name)
  {

  }
 private:
  std::unique_ptr<BasicAuthHandler> auth_;
  std::unique_ptr<amqp_common::RabbitMQRestService> rest_service_;
  std::unique_ptr<amqp_common::AMQPPublisherService> publisher_service_;
  std::unordered_map<std::basic_string<char>, ComputationalNode> worker_nodes_;
};




}