#pragma once

#include "amqpCommon.h"

namespace amqp_common {

/**
 * @brief Service that allow to Publish messages to multiple queues
 */
class AMQPPublisherService {
 public:

  /**
   * @details Will declare all queues in provide array
   * @param connection_string
   * @param queues
   */
  AMQPPublisherService(const std::string &connection_string,
                       const std::vector<std::string> &queues = {});

  /**
   * @brief delted i's queue
   * @param i
   * @throws shared::outOfRange
   */
  void RemoveQueue(size_t i);

  /**
   * Adds new queue
   * @param queue
   * @param create
   */
  void AddQueue(const std::string &queue,
                bool create);

  /**
   * @brief publishe message to i's queue
   * @param message
   * @param i
   * @throws shared::outOfRange
   */
  void Publish(EnvelopePtr message, size_t i);

  void EndLoop();

  void RestartLoop();

  ~AMQPPublisherService();

  bool IsConnected() const;

 private:
  boost::asio::io_service m_service;
  std::unique_ptr<boost::asio::io_service::work> m_work;

  MyHandler m_handler;
  AMQP::TcpConnection m_connection;
  AMQP::TcpChannel m_channel;
  std::vector<std::string> m_queues;
  std::thread m_serviceThread;

  static inline const std::string defaultExhc = "testexch";//todo delete

};
}