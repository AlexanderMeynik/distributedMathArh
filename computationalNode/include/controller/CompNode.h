#pragma once

#include <drogon/HttpController.h>
#include "common/sharedDeclarations.h"
#include "computationalLib/math_core/TestRunner.h"

using namespace drogon;
using shared::BenchResVec;

//todo forward results from nodes
namespace rest {
namespace v1 {
class AMQPHandler {
 public:
  AMQPHandler() {
    cc_ = std::make_shared<int>(0);
    con_ = false;
  }

  bool CheckConnection(std::string &ip, std::string &queue) {
    //todo check where logs are written
    con_ = true;
    LOG_INFO << "CheckConnection\t" << ip << '\t' << queue << '\n';
    return con_;
  }

  bool Connect(const std::string &ip, const std::string &queue) {
    con_ = true;
    LOG_INFO << "Connect\t" << ip << '\t' << queue << '\n';
    queues_[0] = queue + "1";
    queues_[1] = queue + "2";

    event_loop_ = std::jthread([this](const std::stop_token &stoken) {
      //todo listen to queues(from 1 to second)
      while (con_) {
        sleep(1);
        std::stringstream ss;
        ss << std::this_thread::get_id();
        LOG_INFO << ss.str() << '\t' << *cc_ << '\n';
        *cc_ = *cc_ + 1;
      }
    });
    return true;
  }

  void Disconnect() {
    con_ = false;
    event_loop_.join();
    Reset();
    LOG_INFO << "Disconnect\n";
  }

  int GetC() {
    return *cc_;
  }

  void Reset() {
    *cc_ = 0;
  }

  std::atomic<bool> con_;
  std::array<std::string, 2> queues_;//todo combine with producerService
  std::jthread event_loop_;
  std::shared_ptr<int> cc_;
};

class CompNode : public drogon::HttpController<CompNode> {
  std::unordered_map<std::string, std::thread> threads_;
  std::shared_ptr<AMQPHandler> handler_;
  BenchResVec bench_res_;

  BenchResVec RunBench() {
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
