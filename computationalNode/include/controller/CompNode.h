#pragma once

#include <drogon/HttpController.h>
#include "service/CompNodeService.h"

using namespace drogon;
using namespace shared;

namespace rest::v1 {
using comp_services::ComputationNodeService;

class CompNode : public drogon::HttpController<CompNode> {
  //std::unordered_map<std::string, std::thread> threads_;
  std::unique_ptr<ComputationNodeService> handler_;

 public:

  CompNode() {
    handler_ = std::make_unique<ComputationNodeService>();
  }

  using Cont = CompNode;

  METHOD_LIST_BEGIN
    ADD_METHOD_TO(Cont::GetStatus, "v1/status", Get);
    ADD_METHOD_TO(Cont::ConnectHandler, "v1/Connect", Post);
    ADD_METHOD_TO(Cont::DisconnectHandler, "v1/Disconnect", Post);
  METHOD_LIST_END

  void GetStatus(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);

  void ConnectHandler(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);

  void DisconnectHandler(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
};
}

