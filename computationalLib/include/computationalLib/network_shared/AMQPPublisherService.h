#pragma once

#include "amqpCommon.h"

namespace amqp_common {

/**
 * @brief Service that allow to Publish messages to multiple queues
 */
 //todo test for connect->disconnect->connect
class AMQPPublisherService {
 public:


  AMQPPublisherService();

  void SetParameters(const std::string &connection_string, const std::vector<std::string> &queues);

  /**
   * @details Will declare all queues in provide array
   * @param connection_string
   * @param queues
   */
  AMQPPublisherService(const std::string &connection_string,
                       const std::vector<std::string> &queues = {});


  void Connect();

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

  void Disconnect();


  ~AMQPPublisherService();

  bool IsConnected() const;

 private:

  void RestartLoop();

  boost::asio::io_service service_;
  std::unique_ptr<boost::asio::io_service::work> work_;
  MyHandler handler_;
  std::unique_ptr<AMQP::TcpConnection> connection_;
  std::unique_ptr<AMQP::TcpChannel> channel_;
  std::vector<std::string> queues_;
  std::string connection_string_;
  std::thread service_thread_;


  std::string defaultExhc = "testexch";//todo setter

};
}