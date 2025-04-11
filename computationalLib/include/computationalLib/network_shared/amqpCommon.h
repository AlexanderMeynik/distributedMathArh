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

/**
 * @brief Constructs amqp connection string
 * @param host_port
 * @param user
 * @param password
 * @param secure sets with amqps protocol
 * @return
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

/**
 * @brief Service that allow to Publish messages to multiple queues
 */
 //todo set new queue(connstring)
class amqpPublisherService {
 public:

  /**
   * @details Will declare all queues in provide array
   * @param connection_string
   * @param queues
   */
  amqpPublisherService(const std::string &connection_string,
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

  ~amqpPublisherService();

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
