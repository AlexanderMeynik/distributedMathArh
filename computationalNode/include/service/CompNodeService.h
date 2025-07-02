#pragma once

#include "network_shared/AMQPConsumerService.h"
#include "service/BenchmarkRunnerService.h"

#include <drogon/HttpSimpleController.h>

using namespace drogon;

/// Namespace for comp_node services
namespace comp_services {
using namespace shared;
class ComputationNodeService {

  ///@todo make new type?
  using ConnectionData=Json::Value;
 public:
  ComputationNodeService();

  /**
   * @brief Retrieves computational node characteristics
   * @details This function is used to get the following data
   * 1. It's connection status.
   * 2. It's rebalance param changes.
   * 3. It's performance metric
   * @param rebalance - when node is rebalanced
   * @return Json with status =200
   */
  Json::Value GetStatus(bool rebalance = false);

  /**
   * @brief Connects computational node to queue
   * @param req - used to get
   * @return Json with status =200
   * @return Json with status =409 if node was already connected
   * @return Json with status =415 if body type is not json
   */
  Json::Value Connect(const HttpRequestPtr &req);

  /**
   * @brief Disconnects computational node from queue
   * @return Json with status =200
   * @return Json with status =409 on repeated disconnect
   */
  Json::Value Disconnect();

  /**
   * @brief Starts node benhcmark and returns old version of it
   * @return GetStatus output
   * @return Json with status =409 if computation is interrupted
   */
  Json::Value Rebalance();

  bool IsTest() const;
  void SetTest(bool test);
 private:
  void RunBench();
  bool IsConnected();
  bool Computed();

  amqp_common::AMQPConsumerService consumer_service_;///< service that handles queue events
  std::optional<BenchResVec> bench_res_;///< performance metric of the node
  ConnectionData c_data_;///< contains data about service node is connected to
  bool test_ = false;///<if true will run empty benchmark

  std::jthread computation_thread_;
  std::atomic<bool> characteristic_computed_;
  std::unique_ptr<BenchmarkRunner> benchmark_runner_;

};

}