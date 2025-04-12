#include "network_shared/AMQPPublisherService.h"

#include <fmt/format.h>


namespace amqp_common {

AMQPPublisherService::AMQPPublisherService(const std::string &connection_string,
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

void AMQPPublisherService::RemoveQueue(size_t i) {
  if (i >= m_queues.size()) {
    throw shared::outOfRange(i, 0, m_queues.size() - 1);
  }
  m_queues.erase(m_queues.begin() + i);

}

void AMQPPublisherService::AddQueue(const std::string &queue, bool create) {
  if (create) {
    DeclareQueue(m_channel, queue, defaultExhc);
  }
  m_queues.push_back(queue);
}

AMQPPublisherService::~AMQPPublisherService() {
  EndLoop();
  m_queues.clear();
}

void AMQPPublisherService::Publish(EnvelopePtr message, size_t i) {
  if (i >= m_queues.size()) {
    throw shared::outOfRange(i, 0, m_queues.size() - 1);
  }
  message->setTimestamp(std::chrono::steady_clock::now().time_since_epoch().count());
  m_channel.publish(defaultExhc, m_queues[i], *message);
}

void AMQPPublisherService::EndLoop() {
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

void AMQPPublisherService::RestartLoop() {
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

bool AMQPPublisherService::IsConnected() const {
  return m_handler.IsConnected();
}
}