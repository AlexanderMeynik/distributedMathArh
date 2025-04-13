#pragma once

#include <string>
#include <vector>
#include <thread>
#include <memory>

#include "common/errorHandling.h"

#include <amqpcpp.h>
#include <amqpcpp/linux_tcp.h>

#include <boost/asio/io_service.hpp>
#include <amqpcpp/libboostasio.h>

namespace amqp_common {

using AMQP::Envelope;
using EnvelopePtr = std::shared_ptr<Envelope>;
using AMQP::MessageCallback;

/**
 * @brief Constructs amqp connection string
 * @param host_port
 * @param user
 * @param password
 * @param secure sets with amqps protocol
 */
std::string ConstructCString(const std::string& host_port,
                            const std::string& user,
                            const std::string&password,
                            bool secure=false);
AMQP::Address ConstructCAddress(const std::string& host_port,
                                const std::string& user,
                                const std::string&password,
                                bool secure=false);
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

  void onClosed(AMQP::TcpConnection *connection) override {
    m_work.reset();
    connected_= false;

    std::cout << "Connection closed.\n";
  }

  void onError(AMQP::TcpConnection *connection,
               const char *message) override {
    std::cout << "Connection error: " << message << '\n';
  }

  void onConnected(AMQP::TcpConnection *connection) override {
    std::cout << "Connection established successfully." << '\n';
    connected_= true;
  }

  bool IsConnected() const;

 private:
  std::unique_ptr<boost::asio::io_service::work> &m_work;
  bool connected_;
};


static auto inline d_message_callback =
    [](const AMQP::Message &message,
           uint64_t delivery_tag,
           bool redelivered) {

      std::cout << "Body: " << std::string(message.body(), message.bodySize()) << '\n';
      std::cout << "Priority: " << (int) message.priority() << '\n';
      std::cout << "Persistent: " << message.persistent() << '\n';
      std::cout << "Content-Type: " << message.contentType() << '\n';
      std::cout << "Timestamp: " << message.timestamp() << '\n';
      for (const auto &key : message.headers().keys()) {
        std::cout << "Header [" << key << "] = " << message.headers().operator[](key) << '\t'
                  << message.headers().operator[](key).typeID() << '\n';//typeId
      }
      std::cout << '\n';
      //channel_->ack(delivery_tag);
    };




}
