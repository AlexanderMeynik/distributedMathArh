#pragma once

#include "amqpCommon.h"

namespace amqp_common {
/**
 * @brief Service that allow to start and end reading event Loop from specified queue
 */
class amqpConsumerService {
 public:

  /**
   *
   */
  amqpConsumerService();
  /**
   * @brief
   * @param connection_string
   * @param queue_name
   */
  amqpConsumerService(const std::string &connection_string,
                      const std::string &queue_name);


  /**
   * @brief
   * @param connection_string
   * @param queue_name
   */
  void SetParameters(const std::string &connection_string,
                     const std::string &queue_name);
  /**
   * @brief Starts event Loop to read from queue1
   */
  void Connect();

  /**
   * @brief Ends event Loop()
   */
  void Disconnect();

  ~amqpConsumerService();

  bool IsConnected() const;

 private:

  void Reconnect();

  boost::asio::io_service m_service;
  std::unique_ptr<boost::asio::io_service::work> m_work;

  std::unique_ptr<MyHandler> m_handler;
  std::unique_ptr<AMQP::TcpConnection> m_connection;
  std::unique_ptr<AMQP::TcpChannel> m_channel;

  std::string m_queue;
  std::string m_c_string;
  std::thread m_serviceThread;
};

}