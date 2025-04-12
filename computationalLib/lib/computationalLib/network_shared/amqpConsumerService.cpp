#include "network_shared/amqpConsumerService.h"
#include <fmt/format.h>

namespace amqp_common {

void amqpConsumerService::Connect() {

  m_service.reset();
  Reconnect();
  m_serviceThread = std::thread([this]() { m_service.run(); });
}

void amqpConsumerService::Reconnect() {

  m_work=std::make_unique<boost::asio::io_service::work>(m_service);

  if(IsConnected())
  {
    return;
  }
  if(!m_connection||!m_connection->ready())
  {
    if(m_c_string.empty())
    {
      throw shared::zeroSize(VARIABLE_NAME(m_c_string));
    }
    m_connection=std::make_unique<AMQP::TcpConnection>(m_handler.get(),AMQP::Address(m_c_string));
    m_channel=std::make_unique<AMQP::TcpChannel>(m_connection.get());

  }



  m_channel->onError([](const char *message) {
    std::cout << "Channel error: " << message << '\n';
  });

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
        m_channel->ack(delivery_tag);
      };

  auto start_cb = [](const std::string &consumer_tag) {
    std::cout << "Consumption started successfully with consumer tag: " << consumer_tag << '\n';
  };

  auto error_cb = [](const char *message) {
    std::cout << "Consumption error: " << message << '\n';
  };

  m_channel->consume(m_queue)
      .onReceived(message_cb)
      .onSuccess(start_cb)
      .onError(error_cb);

}

void amqpConsumerService::Disconnect() {

  m_service.post([this]() {
    m_channel->close();
    m_connection->close();
  });


  if (m_serviceThread.joinable()) {
    m_serviceThread.join();
  }
  m_service.stop();
  m_connection.reset();
  m_channel.reset();
}

amqpConsumerService::~amqpConsumerService() {
  Disconnect();
}

amqpConsumerService::amqpConsumerService():m_service(1),
                                           m_handler(std::make_unique<MyHandler>(m_service, m_work))
{

}


amqpConsumerService::amqpConsumerService(const std::string &connection_string,
                                         const std::string &queue_name) :
    m_service(1),
    //m_work(std::make_unique<boost::asio::io_service::work>(m_service)),
    m_handler(std::make_unique<MyHandler>(m_service, m_work)),
    m_queue(queue_name),
    m_c_string(connection_string)
{

}

void amqpConsumerService::SetParameters(const std::string &connection_string, const std::string &queue_name) {
  m_c_string=connection_string;
  m_queue=queue_name;
}
bool amqpConsumerService::IsConnected() const {
  return m_handler&&m_handler->IsConnected();
}
}