#include "network_shared/AMQPPublisherService.h"

#include <fmt/format.h>

namespace amqp_common {

AMQPPublisherService::AMQPPublisherService() : AMQPService() {}

void AMQPPublisherService::SetParameters(const std::string &connection_string,
                                         const std::vector<std::string> &queues,
                                         const std::string &exchange) {
  c_string_ = connection_string;
  queues_ = queues;
  default_exchange_ = exchange;
}

AMQPPublisherService::AMQPPublisherService(const std::string &connection_string,
                                           const std::vector<std::string> &queues,
                                           const std::string &exchange)
    : AMQPService(connection_string),
      queues_(queues),
      default_exchange_(exchange) {}

void AMQPPublisherService::Reconnect() {

  if (IsConnected()) {
    return;
  }
  if (!connection_ || !connection_->ready()) {
    if (c_string_.empty()) {
      throw shared::zeroSize(VARIABLE_NAME(c_string_));
    }
    connection_ = std::make_unique<AMQP::TcpConnection>(handler_.get(), AMQP::Address(c_string_));
    channel_ = std::make_unique<AMQP::TcpChannel>(connection_.get());
  }

  channel_->onError([this](const char *message) {
    GuardType kGuard{s_mutex_};
    if (!promise_set_) {
      connection_promise_.set_value(message);
      promise_set_ = true;
      std::cout << fmt::format("Channel error: {}\n", message);
    }
  });

  channel_->onReady([this]() {
    GuardType kGuard{s_mutex_};
    if (!promise_set_) {
      connection_promise_.set_value("");
      promise_set_ = true;
      std::cout << "Channel started successfully\n";
    }
  }
  );

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

const std::string &AMQPPublisherService::GetDefaultExchange() const {
  return default_exchange_;
}
std::string AMQPPublisherService::ServiceName() {
  return "AMQPPublisherService";
}

}