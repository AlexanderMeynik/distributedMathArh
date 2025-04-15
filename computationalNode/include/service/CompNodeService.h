#pragma once

#include <future>

#include <drogon/HttpSimpleController.h>
#include "common/sharedDeclarations.h"
#include "math_core/TestRunner.h"
#include "network_shared/AMQPConsumerService.h"

#include "service/BenchmarkRunnerService.h"

#include "common/Printers.h"

using namespace drogon;

/// Namespace for comp_node services
namespace comp_services {
class ComputationNodeService {
 public:
  ComputationNodeService() {
    RunBench();
  }

  Json::Value GetStatus();

  Json::Value Connect(const HttpRequestPtr &req);

  Json::Value Disconnect();

  void RunBench() {

   /* if (!characteristic_computed_.load()) {
      return;
    }
    if (computation_thread_.joinable()) {
      computation_thread_.join();
    }*/

    characteristic_computed_.store(false, std::memory_order_release);

    computation_thread_ = std::jthread([this]() {
      bench_res_ = DefaultBench();
      std::cout<<"Job done\n";
      characteristic_computed_.store(true, std::memory_order_release);
    });
  }
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
//todo set message worker(test runner?, or any else)
}