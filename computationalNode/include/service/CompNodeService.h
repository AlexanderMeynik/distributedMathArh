#pragma once

#include <drogon/HttpSimpleController.h>
#include "common/sharedDeclarations.h"
#include "math_core/TestRunner.h"
#include "network_shared/amqpCommon.h"

using namespace drogon;

namespace comp_service
{
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

  //todo parameter assignment
  std::optional<amqp_common::amqpConsumerService> serv;

  std::atomic<bool> con_;
  std::array<std::string, 2> queues_;//todo combine with producerService
  std::jthread event_loop_;
  std::shared_ptr<int> cc_;
};

}