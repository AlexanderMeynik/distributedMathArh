#include "network_shared/AMQPPublisherService.h"

#include <fmt/format.h>


namespace amqp_common {


AMQPPublisherService::AMQPPublisherService():service_(1), handler_(service_, work_)  {

}

void AMQPPublisherService::SetParameters(const std::string &connection_string,
                                         const std::vector<std::string> &queues,
                                         const std::string& exchange) {
  connection_string_ = connection_string;
  queues_ = queues;
  default_exchange_=exchange;
}

AMQPPublisherService::AMQPPublisherService(const std::string &connection_string,
                                           const std::vector<std::string> &queues,
                                           const std::string& exchange)
    : service_(1),
      handler_(service_, work_),
      connection_string_(connection_string),
      queues_(queues),
      default_exchange_(exchange)
    {}

const std::string &AMQPPublisherService::GetConnectionString() const {
  return connection_string_;
}

void AMQPPublisherService::RemoveQueue(size_t i) {
  if (i >= queues_.size()) {
    throw shared::outOfRange(i, 0, queues_.size() - 1);
  }
  queues_.erase(queues_.begin() + i);
}

void AMQPPublisherService::AddQueue(const std::string &queue, bool create) {
  if (create) {
    DeclareQueue(*channel_, queue, default_exchange_);
  }
  queues_.push_back(queue);
}

AMQPPublisherService::~AMQPPublisherService() {
  Disconnect();
  queues_.clear();
}

void AMQPPublisherService::Publish(EnvelopePtr message, size_t i) {
  if (i >= queues_.size()) {
    throw shared::outOfRange(i, 0, queues_.size() - 1);
  }
  message->setTimestamp(std::chrono::steady_clock::now().time_since_epoch().count());
  channel_->publish(default_exchange_, queues_[i], *message);
}

void AMQPPublisherService::Publish(EnvelopePtr message, const std::string qname) {
  message->setTimestamp(std::chrono::steady_clock::now().time_since_epoch().count());
  channel_->publish(default_exchange_, qname, *message);
}



void AMQPPublisherService::Disconnect() {
  service_.post([this]() {
    if (channel_) channel_->close();
    if (connection_) connection_->close();
  });

  if (service_thread_.joinable()) {
    service_thread_.join();
  }

  service_.stop();
  work_.reset();
  channel_.reset();
  connection_.reset();

}

void AMQPPublisherService::RestartLoop() {


  if(IsConnected())
  {
    return;
  }
  if(!connection_||!connection_->ready())
  {
    if(connection_string_.empty())
    {
      throw shared::zeroSize(VARIABLE_NAME(c_string_));
    }
    connection_=std::make_unique<AMQP::TcpConnection>(&handler_,AMQP::Address(connection_string_));
    channel_=std::make_unique<AMQP::TcpChannel>(connection_.get());
  }


  channel_->onError([](const char *message) {
    std::cout << fmt::format("Channel error: {}\n", message);
  });


}

bool AMQPPublisherService::IsConnected() const {
  return handler_.IsConnected();
}
void AMQPPublisherService::Connect() {
  service_.reset();
  work_=std::make_unique<boost::asio::io_service::work>(service_);
  RestartLoop();
  service_thread_= std::thread([this]() { service_.run(); });
}
const std::string &AMQPPublisherService::GetDefaultExchange() const {
  return default_exchange_;
}

}