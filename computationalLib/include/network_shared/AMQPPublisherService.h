#pragma once

#include "amqpCommon.h"
#include <future>

namespace amqp_common {

/**
 * @brief Service that allow to Publish messages to multiple queues
 */
class AMQPPublisherService : public AMQPService {
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
   * @brief Calls Disconnect and frees used memory with smart pointers
   * @see AMQPPublisherService Disconnect
   */
  ~AMQPPublisherService();

 private:

  void Reconnect() override;

  std::string ServiceName() override;

  std::vector<std::string> queues_;///< list of used queues ///@todo remove

  std::thread service_thread_;///< thread to run boost service

  std::string default_exchange_;///< default exchange to bind queues


};
}