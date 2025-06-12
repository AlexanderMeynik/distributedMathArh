#pragma once

#include "service/MainNodeService.h"
#include <drogon/HttpController.h>

/// Namespace for main node rest api handlers
namespace rest::v1 {

using namespace drogon;

/// namespace for services used in main node
using namespace main_services;

/**
 * @class ClusterConfigController
 * @brief Drogon service for main node
 */
class ClusterConfigController : public drogon::HttpController<ClusterConfigController> {
  std::unique_ptr<MainNodeService> main_node_service_;
 public:
  /**
   * @brief Initializes MainNodeService with preconfigured credentials provider
   */
  ClusterConfigController() {
    ///@todo pass args(create role for q creation/ message send)
    main_node_service_ = std::make_unique<MainNodeService>("sysadmin", "syspassword");

  }///@todo /rebalance
  ///@todo ping(measures latencies)?
  using Cont = ClusterConfigController;

  METHOD_LIST_BEGIN
    ADD_METHOD_TO(Cont::GetStatus, "v1/status", Get);
    ADD_METHOD_TO(Cont::ConnectNodeHandler, "v1/connect_node?ip={ip}", Post);
    ADD_METHOD_TO(Cont::DisconnectNodeHandler, "v1/disconnect_node?ip={ip}", Post);

    ADD_METHOD_TO(Cont::ConnectQ, "v1/connect_publisher", Post);
    ADD_METHOD_TO(Cont::DisconnectQ, "v1/disconnect_publisher", Post);

    ADD_METHOD_TO(Cont::SentToExecution, "v1/send_task", Put);

  METHOD_LIST_END

  /**
   * @brief Retrieves the status of whole cluster
   * @param req
   * @param callback
   */
  void GetStatus(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);

  /**
   * @brief Connects node to computational cluster
   * @param req
   * @param callback
   * @param host_port
   * Requires main node to be connected to RabbitMQ
   * @see ClusterConfigController::ConnectQ
   */
  void ConnectNodeHandler(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback,
                          const std::string &host_port);

  /**
   * @brief Disconnects node to computational cluster
   * @param req
   * @param callback
   * @param host_port
   * Requires main node to be connected to RabbitMQ
   * @see ClusterConfigController::ConnectQ
   */
  void DisconnectNodeHandler(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback,
                             const std::string &host_port);



  /**
   * @brief Sends task to be sharded and published into queues for workers
   * @param req
   * @param callback
   * Requires main node to be connected to RabbitMQ
   * @see ClusterConfigController::ConnectQ
   */
  void SentToExecution(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);

  /**
   * @brief Connects main node to RabbitMQ
   * @brief
   * @param req
   * @param callback
   * Main node connects to RabbitMQ in the following manner:
   * 1. It initializes event loop of AMQPPublisherService
   * 2. It creates all the objects(if not exists) specified in members
   * @see amqp_common::AMQPPublisherService
   */
  void ConnectQ(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);

  /**
   * @brief Disconnects main node from RabbitMQ
   * @param req
   * @param callback
   * @see ClusterConfigController::ConnectQ
   */
  void DisconnectQ(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);

};
}
