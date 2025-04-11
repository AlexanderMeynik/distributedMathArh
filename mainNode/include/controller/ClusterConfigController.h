#pragma once

#include <drogon/HttpController.h>
#include "math_core/TestRunner.h"
#include "common/sharedDeclarations.h"
#include <drogon/HttpClient.h>
#include <drogon/HttpRequest.h>

using namespace drogon;
using shared::BenchResVec;

namespace rest {
namespace v1 {

/// nodeStatus Enum
enum class NodeStatus {
  /// Node is connected to cluster and is ready to receive it's tasks
  ACTIVE,
  /// Node is present in cluster but is not ready to recieve tasks
  INACTIVE,
  /// An error occurred moving node to a failed state
  FAILED
};

//todo std::array
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

/**
 * @brief Drogon service for main node
 */
class ClusterConfigController : public drogon::HttpController<ClusterConfigController> {
  std::unordered_map<std::string, ComputationalNode> clients_;
 public:
  ClusterConfigController() {
    //client=HttpClient::newHttpClient("http://localhost:8081");
    //client->get

  }//todo /rebalance
  //todo ping(measures latencies)?
  using Cont = ClusterConfigController;

  METHOD_LIST_BEGIN
    ADD_METHOD_TO(Cont::GetStatus, "v1/status", Get);
    ADD_METHOD_TO(Cont::ConnectHandler, "v1/Connect?ip={ip}&qip={qip}&name={queue}", Post);
    ADD_METHOD_TO(Cont::DisconnectHandler, "v1/Disconnect?ip={ip}", Post);
  METHOD_LIST_END

  void GetStatus(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);

  void ConnectHandler(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback,
                      const std::string &host_port, const std::string &qip, const std::string &name);

  void DisconnectHandler(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback,
                         const std::string &host_port);
};
}
}
