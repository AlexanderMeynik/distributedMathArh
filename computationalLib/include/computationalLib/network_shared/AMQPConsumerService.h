#pragma once

#include "amqpCommon.h"

namespace amqp_common {
/**
 * @brief Service that allow to start and end reading event Loop from specified queue
 */
class AMQPConsumerService {
 public:
  /**
   *
   */
  AMQPConsumerService();
  /**
   * @brief
   * @param connection_string
   * @param queue_name
   */
  AMQPConsumerService(const std::string &connection_string,
                      const std::string &queue_name);


  /**
   * @brief
   * @param connection_string
   * @param queue_name
   */
  void SetParameters(const std::string &connection_string,
                     const std::string &queue_name);


  void SetMessageCallback(MessageCallback callback);
  /**
   * @brief Starts event Loop to read from queue1
   */
  void Connect();

  /**
   * @brief Ends event Loop()
   */
  void Disconnect();

  ~AMQPConsumerService();

  bool IsConnected() const;

 private:

  /**
   * @brief Reconnects to specified queue
   * Restarts amqp connection and channel objects.
   * Specifies message management callbacks.
   */
  void Reconnect();

  boost::asio::io_service service_;
  std::unique_ptr<boost::asio::io_service::work> work_;

  std::unique_ptr<MyHandler> handler_;
  std::unique_ptr<AMQP::TcpConnection> connection_;
  std::unique_ptr<AMQP::TcpChannel> channel_;

  std::string queue_;
  std::string c_string_;
  std::thread service_thread_;

  MessageCallback message_callback_;///< callback that processes incomming messages
};

}