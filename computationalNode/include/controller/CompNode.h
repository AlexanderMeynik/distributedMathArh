#pragma once

#include <drogon/HttpController.h>
#include "service/CompNodeService.h"

using namespace drogon;
using shared::BenchResVec;

namespace rest::v1 {
using comp_service::AMQPHandler;

class CompNode : public drogon::HttpController<CompNode> {
  std::unordered_map<std::string, std::thread> threads_;
  std::unique_ptr<AMQPHandler> handler_;
  BenchResVec bench_res_;

  BenchResVec RunBench() {//todo use my bench
    return {1, 1, 1, 1, 1, 1, 1, 1, 1};
  }

 public:

  CompNode() {
    handler_ = std::make_unique<AMQPHandler>();
    bench_res_ = RunBench();
  }

  using Cont = CompNode;

  METHOD_LIST_BEGIN
    //todo ping(status)
    ADD_METHOD_TO(Cont::GetStatus, "v1/status", Get);
    ADD_METHOD_TO(Cont::ConnectHandler, "v1/Connect", Post);
    ADD_METHOD_TO(Cont::DisconnectHandler, "v1/Disconnect", Post);
  METHOD_LIST_END

  void GetStatus(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);

  void ConnectHandler(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);

  void DisconnectHandler(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
};
}

