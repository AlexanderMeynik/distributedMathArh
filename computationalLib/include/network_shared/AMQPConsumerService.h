#pragma once

#include "amqpCommon.h"

#include <future>

namespace amqp_common {
/**
 * @brief Service that allow to start and end reading event Loop from specified queue
 */
class AMQPConsumerService : public AMQPService {
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

  /**
   * @brief Sets message processing callback
   * @param callback
   */
  void SetMessageCallback(MessageCallback callback);

 private:

  /**
   * @brief Reconnects to specified queue
   * Restarts amqp connection and channel objects.
   * Specifies message management callbacks.
   */
  void Reconnect() override;

  std::string ServiceName() override;

  std::string queue_; ///< queue to which the srvice will listen
  MessageCallback message_callback_;///< callback that processes incoming messages
};

}