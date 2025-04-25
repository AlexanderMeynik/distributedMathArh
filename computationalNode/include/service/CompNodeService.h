#pragma once

#include <future>

#include <drogon/HttpSimpleController.h>
#include "common/sharedDeclarations.h"
#include "network_shared/AMQPConsumerService.h"

#include "service/BenchmarkRunnerService.h"

using namespace drogon;

/// Namespace for comp_node services
namespace comp_services {
using namespace shared;
class ComputationNodeService {
 public:
  ComputationNodeService() {
    RunBench();
  }

  Json::Value GetStatus();

  Json::Value Connect(const HttpRequestPtr &req);

  Json::Value Disconnect();

  void RunBench();
 private:
  bool CheckConnection();
  bool Computed() {
    return characteristic_computed_.load(std::memory_order_acquire);
  }

  amqp_common::AMQPConsumerService amqp_prod_;
  BenchResVec bench_res_;
  std::jthread computation_thread_;
  std::atomic<bool> characteristic_computed_;

};

}