#pragma once

#include "network_shared/AMQPConsumerService.h"
#include "service/BenchmarkRunnerService.h"

#include <drogon/HttpSimpleController.h>

using namespace drogon;

/// Namespace for comp_node services
namespace comp_services {
using namespace shared;
class ComputationNodeService {
 public:
  ComputationNodeService();

  Json::Value GetStatus();

  Json::Value Connect(const HttpRequestPtr &req);

  Json::Value Disconnect();

  void RunBench();
 private:
  bool CheckConnection();
  bool Computed() {
    return characteristic_computed_.load(std::memory_order_acquire);
  }

  amqp_common::AMQPConsumerService consumer_service_;
  BenchResVec bench_res_;
  std::jthread computation_thread_;
  std::atomic<bool> characteristic_computed_;
  std::unique_ptr<BenchmarkRunner> benchmark_runner_;

};

}