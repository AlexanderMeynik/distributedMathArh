#pragma once

#include "service/WorkerManagementService.h"

#include "network_shared/amqpRestService.h"
#include "network_shared/AMQPPublisherService.h"

/// Namespace for services used in main node
namespace main_services {
using rest_utils::JsonAuthHandler;
/**
 * @class MainNodeService
 * @brief Handles most of service side interactions for main Node
 * @details Contains a bunch of other services to maintain the cluster behaviour.
 * @see amqp_common::RabbitMQRestService
 * @see rest_utils::JsonAuthHandler
 * @see amqp_common::AMQPPublisherService
 * @see main_services::WorkerManagementService
 */


class MainNodeService {
 public:
  /**
   * @brief Initializes all components on heap  with default constructors.
   * Creates JsonAuthHandler from user,password pair
   * @param user
   * @param password
   * @see amqp_common::RabbitMQRestService::RabbitMQRestService()
   * @see rest_utils::JsonAuthHandler
   * @see amqp_common::AMQPPublisherService::AMQPPublisherService()
   * @see main_services::WorkerManagementService::WorkerManagementService()
   */
  MainNodeService(const std::string &user, const std::string &password);

  /**
   * @brief Queries the status of the main node and connected workers
   * Status contains:
   * 1. Connection status to a RabbitMQ instance (with data if connected)
   * 2. Several lists(by status) of all nodes with their characteristics(bench result ...)
   */
  Json::Value Status();

  /**
   * @brief Connects the node to the computational cluster
   * @param host_port
   * @param name -queue name(currently same as host_port)
   *
   */
  Json::Value ConnectNode(const std::string &host_port,
                          const std::string &name);

  /**
   *
   * @param host_port
   */
  Json::Value DisconnectNode(const std::string &host_port);

  /**
   * @brief Connects the main node to a RabbiMQ instance at qip
   * @param qip
   */
  Json::Value Connect(const std::string &qip);

  /**
   * @brief Disconnects the main node from a RabbitMQ instance
   */
  Json::Value Disconnect();

  /**
   * @brief Sends the task to be sharded and republished
   * Uses collected node bench information to divide task into non intersecting ranges
   * And sends them to workers to process.
   * @param ts
   * @see amqp_common::AMQPPublisherServicePublish(EnvelopePtr message, const std::string qname)
   */
  Json::Value SendToExecution(network_types::TestSolveParam &ts);
 private:
  std::unique_ptr<JsonAuthHandler> auth_; ///< auth handler for authentication in RabbitMQ
  std::unique_ptr<amqp_common::RabbitMQRestService> rest_service_; ///< rest service for metrics collection and management
  std::unique_ptr<amqp_common::AMQPPublisherService> publisher_service_;///< service to publish messages in
  std::unique_ptr<WorkerManagementService> worker_management_service_; ///< manages connected nodes and their metrics

  std::string q_host_; ///< RabbitMQ queue host

  /**
   * @brief Publishes message to queue with routingKey=node
   * @param ts
   * @param node
   * @return Json with status =200 if
   * @return Json with status = 409 if connection was closed
   */
  Json::Value PublishMessage(network_types::TestSolveParam &ts, std::string node);

  static inline std::string vhost_ = "%2F";///< default vhost
};

}