#pragma once

#include <string>
#include <vector>
#include <thread>
#include <memory>

#include "common/errorHandling.h"
#include "network_shared/networkTypes.h"

#include <amqpcpp.h>
#include <amqpcpp/linux_tcp.h>

#include <boost/asio/io_service.hpp>
#include <amqpcpp/libboostasio.h>

namespace amqp_common {

using AMQP::Envelope;
using EnvelopePtr = std::shared_ptr<Envelope>;
using AMQP::MessageCallback;
using WorkPtr=std::unique_ptr<boost::asio::io_service::work>;

/**
 * @brief Constructs amqp connection string
 * @param host_port
 * @param user
 * @param password
 * @param secure sets with amqps protocol
 */
std::string ConstructCString(const std::string &host_port,
                             const std::string &user,
                             const std::string &password,
                             bool secure = false);
AMQP::Address ConstructCAddress(const std::string &host_port,
                                const std::string &user,
                                const std::string &password,
                                bool secure = false);
/**
 * @brief
 * @param channel
 * @param queue1
 * @param exchange1
 */
void DeclareQueue(AMQP::Channel &channel,
                  const std::string &queue1,
                  const std::string &exchange1);

/**
 * @brief
 * @param channel
 * @param exchange1
 * @param routingKey
 * @param body
 * @param persistent
 */
void sendMessage(AMQP::Channel &channel,
                 std::string_view exchange1,
                 std::string_view routingKey,
                 std::string_view body,
                 bool persistent = true);

/**
 * @brief
 * @param channel
 * @param queueName
 */
void consumeMessages(AMQP::Channel &channel,
                     std::string_view queueName);

/**
 * @brief Custom for connection events logging
 */
class MyHandler : public AMQP::LibBoostAsioHandler {
 public:
  MyHandler(boost::asio::io_service &service,
            std::unique_ptr<boost::asio::io_service::work> &work_ref);


  /**
   * @brief Method that is called on connection close
   * @param connection
   */
  void onClosed(AMQP::TcpConnection *connection) override {
    ResetLoop();

    std::cout << "Connection closed.\n";
  }

  /**
   * @brief Method that is called on connection error
   * @param connection
   * @param message
   */
  void onError(AMQP::TcpConnection *connection,
               const char *message) override {
    std::cout << "Connection error: " << message << '\n';//todo log
    ResetLoop();
  }

  /**
   * @brief Method that is called when connection is established
   * @param connection
   */
  void onConnected(AMQP::TcpConnection *connection) override {
    std::cout << "Connection established successfully." << '\n';
    connected_ = true;
  }

  /**
   * @brief Gets handles connectio status
   *
   */
  bool IsConnected() const;

 private:
  /**
   * @brief Resets work to stop event loop
   */
  void ResetLoop()
  {
    m_work_.reset();
    connected_ = false;
  }
  WorkPtr &m_work_; /// < work pointer to handle event loop
  bool connected_;
};


/**
 * @brief Default message processing callback
 * @param message
 * @param delivery_tag
 * @param redelivered
 */
void DefaultMessageCallback(const AMQP::Message &message,
                            uint64_t delivery_tag,
                            bool redelivered);
}



