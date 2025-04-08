#include "amqpCommon.h"

#include <fmt/format.h>

namespace amqp_common {

void DeclareQueue(AMQP::Channel &channel,
                  const std::string &queue1,
                  const std::string &exchange1) {
  channel.declareQueue(queue1)
      .onSuccess([queue1]() {
        std::cout << fmt::format("Queue \"{}\" declared\n", queue1);
      })
      .onError([](const char *msg) {
        std::cerr << "Queue error: " << msg << "\n";
      });

  channel.bindQueue(exchange1, queue1, queue1);
}

void amqpConsumerService::Connect() {
  auto message_cb =
      [this](const AMQP::Message &message,
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
        m_channel.ack(delivery_tag);
      };

  auto start_cb = [](const std::string &consumer_tag) {
    std::cout << "Consumption started successfully with consumer tag: " << consumer_tag << '\n';
  };

  auto error_cb = [](const char *message) {
    std::cout << "Consumption error: " << message << '\n';
  };

  m_channel.consume(m_queue)
      .onReceived(message_cb)
      .onSuccess(start_cb)
      .onError(error_cb);

  m_serviceThread = std::thread([this]() { m_service.run(); });
}

void amqpConsumerService::Disconnect() {

  m_service.post([this]() {
    m_connection.close();
  });

  if (m_serviceThread.joinable()) {
    m_serviceThread.join();
  }
}

amqpConsumerService::~amqpConsumerService() {
  Disconnect();
}

amqpConsumerService::amqpConsumerService(const std::string &connection_string,
                                         const std::string &queue_name) :
    m_service(1),
    m_work(std::make_unique<boost::asio::io_service::work>(m_service)),
    m_handler(m_service, m_work),
    m_connection(&m_handler, AMQP::Address(connection_string)),
    m_channel(&m_connection),
    m_queue(queue_name) {

  m_channel.onError([](const char *message) {
    std::cout << "Channel error: " << message << '\n';
  });

}

MyHandler::MyHandler(boost::asio::io_service &service,
                     std::unique_ptr<boost::asio::io_service::work> &work_ref) :
    AMQP::LibBoostAsioHandler(service),
    m_work(work_ref) {}

amqpPublisherService::amqpPublisherService(const std::string &connection_string,
                                           const std::vector<std::string> &queues)
    :
    m_service(1),
    m_work(std::make_unique<boost::asio::io_service::work>(m_service)),
    m_handler(m_service, m_work),
    m_connection(&m_handler, AMQP::Address(connection_string)),
    m_channel(&m_connection),
    m_queues(queues) {

  m_channel.onError([](const char *message) {
    std::cout << fmt::format("Channel error: {}\n", message);
  });

  m_channel.declareExchange(defaultExhc, AMQP::direct).onSuccess(
      [] {
        std::cout << fmt::format("Exchange \"{}\" declared\n", defaultExhc);
      }).onError([](const char *msg) {
    std::cerr << "Exchange error: " << msg << "\n";
  });

  for (const auto &kQ : m_queues) {
    std::cout << m_queues.size() << '\n';
    DeclareQueue(m_channel, kQ, kQ);
  }

}

void amqpPublisherService::RemoveQueue(size_t i) {
  if (i >= m_queues.size()) {
    throw shared::outOfRange(i, 0, m_queues.size() - 1);
  }
  m_queues.erase(m_queues.begin() + i);

}

void amqpPublisherService::AddQueue(const std::string &queue, bool create) {
  if (create) {
    DeclareQueue(m_channel, queue, defaultExhc);
  }
  m_queues.push_back(queue);
}

amqpPublisherService::~amqpPublisherService() {
  EndLoop();
  m_queues.clear();
}

void amqpPublisherService::Publish(EnvelopePtr message, size_t i) {
  if (i >= m_queues.size()) {
    throw shared::outOfRange(i, 0, m_queues.size() - 1);
  }
  message->setTimestamp(std::chrono::high_resolution_clock().now().time_since_epoch().count());
  m_channel.publish(defaultExhc, m_queues[i], *message);
  //todo try https://github.com/CopernicaMarketingSoftware/AMQP-CPP?tab=readme-ov-file#publisher-confirms
}

void amqpPublisherService::EndLoop() {
  if (m_work) {
    m_work.reset();
  }

  m_service.post([this]() {
    m_connection.close();
  });

  if (m_serviceThread.joinable()) {
    m_serviceThread.join();
  }
  m_service.reset();

}

void amqpPublisherService::RestartLoop() {
  if (m_serviceThread.joinable()) {
    m_serviceThread.join();
  }
  if (!m_work) {
    m_work = std::make_unique<boost::asio::io_service::work>(m_service);
  }
  m_serviceThread = std::thread([this]() {
    m_service.run();
  });
}
}