#pragma once

#include "amqpCommon.h"

namespace amqp_common {

/**
 * @brief Service that allow to Publish messages to multiple queues
 */
class AMQPPublisherService {
 public:

  AMQPPublisherService();

  void SetParameters(const std::string &connection_string,
                     const std::vector<std::string> &queues,
                     const std::string &exchange = "testexch");

  /**
   * @details Will declare all queues in provide array
   * @param connection_string
   * @param queues
   * @param exchange
   */
  AMQPPublisherService(const std::string &connection_string,
                       const std::vector<std::string> &queues = {},
                       const std::string &exchange = "testexch");

  /**
   * @brief Starts service operation
   */
  void Connect();

  const std::string &GetConnectionString() const;

  const std::string &GetDefaultExchange() const;

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
  /**
    * @brief publishe message to i's queue
    * @param message
    * @param qname
    * */
  void Publish(EnvelopePtr message, const std::string qname);

  /**
   * @brief Ends connection and channel to rabbitMQ
   * This is the variant of soft disconnect:
   * while the connection is closing the channel may still process
   * some messages and closes only when it had finished processing
   * the last one(he managed to get).
   */
  void Disconnect();

  /**
   * @brief Calls Disconnect and frees used memory with smart pointers
   * @see AMQPPublisherService Disconnect
   */
  ~AMQPPublisherService();

  bool IsConnected() const;

 private:

  void RestartLoop();

  boost::asio::io_service service_; ///< service used for async event loop
  std::unique_ptr<boost::asio::io_service::work> work_;///< work object to handle loop work
  MyHandler handler_; ///< custom connection handler
  std::unique_ptr<AMQP::TcpConnection> connection_; ///< AMQP::TcpConnection pointer
  std::unique_ptr<AMQP::TcpChannel> channel_; ///< AMQP::TcpChannel pointer
  std::vector<std::string> queues_;///< list of used queues @todo remove
  std::string connection_string_;///< connection string

  std::thread service_thread_;///< thread to run boost service

  std::string default_exchange_;///< default exchange to bind queues

};
}