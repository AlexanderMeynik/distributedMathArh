#pragma once


#include <unordered_map>

#include "service/MainNodeService.h"
#include <drogon/HttpController.h>





namespace rest {
namespace v1 {

using namespace drogon;

using namespace main_service;

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
