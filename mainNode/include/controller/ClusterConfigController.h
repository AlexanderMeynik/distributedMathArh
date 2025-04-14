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

  std::unique_ptr<MainNodeService> main_node_service_;
 public:
  ClusterConfigController() {
    //todo pass args(create role for q creation/ message send)
    main_node_service_=std::make_unique<MainNodeService>("sysadmin","syspassword");

  }//todo /rebalance
  //todo ping(measures latencies)?
  using Cont = ClusterConfigController;

  METHOD_LIST_BEGIN
    ADD_METHOD_TO(Cont::GetStatus, "v1/status", Get);
    ADD_METHOD_TO(Cont::ConnectHandler, "v1/Connect?ip={ip}", Post);
    ADD_METHOD_TO(Cont::DisconnectHandler, "v1/Disconnect?ip={ip}", Post);

    ADD_METHOD_TO(Cont::ConnectQ,"v1/connectQ",Post);
    ADD_METHOD_TO(Cont::DisconnectQ,"v1/disconnectQ",Post);
    ADD_METHOD_TO(Cont::SentMessage,"v1/message?node={node}",Put);
  METHOD_LIST_END

  void GetStatus(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);

  void ConnectHandler(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback,
                      const std::string &host_port);

  void DisconnectHandler(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback,
                         const std::string &host_port);


  void SentMessage(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback,
                   const std::string &node);

  void ConnectQ(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);

  void DisconnectQ(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);



};
}
}
