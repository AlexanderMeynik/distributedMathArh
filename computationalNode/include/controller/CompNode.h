#pragma once

#include <drogon/HttpController.h>
#include "service/CompNodeService.h"

using namespace drogon;
using shared::BenchResVec;

//todo forward results from nodes
namespace rest {
namespace v1 {
using comp_service::AMQPHandler;

class CompNode : public drogon::HttpController<CompNode> {
  std::unordered_map<std::string, std::thread> threads_;
  std::shared_ptr<AMQPHandler> handler_;
  BenchResVec bench_res_;

  BenchResVec RunBench() {//todo use my bench
    return {1, 1, 1, 1, 1, 1, 1, 1, 1};
  }

 public:

  CompNode() {
    handler_ = std::make_shared<AMQPHandler>();
    bench_res_ = RunBench();
  }

  using Cont = CompNode;

  METHOD_LIST_BEGIN
    //todo ping(status)
    ADD_METHOD_TO(Cont::GetStatus, "v1/status", Get);
    ADD_METHOD_TO(Cont::ConnectHandler, "v1/Connect?ip={ip}&name={queue}", Post);
    ADD_METHOD_TO(Cont::DisconnectHandler, "v1/Disconnect", Post);
  METHOD_LIST_END

  void GetStatus(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);

  void ConnectHandler(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback,
                      const std::string &ip, const std::string &name);

  void DisconnectHandler(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
};
}
}
