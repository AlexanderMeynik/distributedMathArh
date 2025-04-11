#pragma once

#include <drogon/HttpSimpleController.h>
#include "common/sharedDeclarations.h"
#include "math_core/TestRunner.h"
#include "network_shared/amqpCommon.h"

using namespace drogon;
//todo not all message are worke on
namespace comp_service
{
class AMQPHandler {
 public:
  AMQPHandler() {
  }

  bool CheckConnection() {
    return amqp_prod_.IsConnected();
  }

  void Connect(const std::string &user,
               const std::string &pass,
               const std::string &ip,
               const std::string &queue) {
    auto c=amqp_common::ConstructCString(ip,user,pass);
    amqp_prod_.SetParameters(c,queue);
    amqp_prod_.Connect();
  }

  void Disconnect() {
    amqp_prod_.Disconnect();//todo hard and mild disconnect
  }

  amqp_common::amqpConsumerService amqp_prod_;
};

}