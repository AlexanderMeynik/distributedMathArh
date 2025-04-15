#pragma once

#include "service/WorkerManagementService.h"

#include "network_shared/amqpRestService.h"
#include "network_shared/AMQPPublisherService.h"

/// Namespace for services used in main node
namespace main_services {

class MainNodeService {
 public:

  MainNodeService(const std::string &user, const std::string &password) {
    auth_ = std::make_unique<JsonAuthHandler>(user, password);
    rest_service_ = std::make_unique<amqp_common::RabbitMQRestService>();

    publisher_service_ = std::make_unique<amqp_common::AMQPPublisherService>();
    worker_management_service_ = std::make_unique<WorkerManagementService>();
  }

  Json::Value Status();

  Json::Value ConnectNode(const std::string &host_port,
                          const std::string &name);

  Json::Value DisconnectNode(const std::string &host_port);

  Json::Value Connect(const std::string &qip,
                      const std::vector<std::string> &names);

  Json::Value Disconnect();

  Json::Value Publish(network_types::TestSolveParam &ts, std::string node);

  Json::Value SendToExecution(network_types::TestSolveParam &ts);
 private:
  std::unique_ptr<JsonAuthHandler> auth_;
  std::unique_ptr<amqp_common::RabbitMQRestService> rest_service_;
  std::unique_ptr<amqp_common::AMQPPublisherService> publisher_service_;
  std::unique_ptr<WorkerManagementService> worker_management_service_;

  std::string q_host_;

  Json::Value Publish2(network_types::TestSolveParam &ts, std::string node);

  static inline std::string vhost_ = "%2F";
};

}