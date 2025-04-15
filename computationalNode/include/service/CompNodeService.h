#pragma once

#include <drogon/HttpSimpleController.h>
#include "common/sharedDeclarations.h"
#include "math_core/TestRunner.h"
#include "network_shared/AMQPConsumerService.h"

#include "common/Printers.h"

using namespace drogon;

/// Namespace for comp_node services
namespace comp_services {
class ComputationNodeService {
 public:
  ComputationNodeService() {
    bench_res_ = RunBench();
  }

  Json::Value GetStatus() {
    Json::Value res_JSON;
    res_JSON["request"] = "status";
    res_JSON["worker_status"] = amqp_prod_.IsConnected() ? "running" : "not running";
    res_JSON["bench"] = print_utils::ContinuousToJson(bench_res_, true, true);
    res_JSON["status"] = drogon::HttpStatusCode::k200OK;
    return res_JSON;
  }

  Json::Value Connect(const HttpRequestPtr &req);

  Json::Value Disconnect();

  BenchResVec RunBench() {
    return shared::DefaultBench();//todo use bench
  }
 private:
  bool CheckConnection();

  amqp_common::AMQPConsumerService amqp_prod_;
  BenchResVec bench_res_;
};
//todo set message worker(test runner?, or any else)
}