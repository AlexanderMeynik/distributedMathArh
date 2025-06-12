#include "network_shared/AMQPPublisherService.h"

#include <fmt/format.h>

namespace amqp_common {

AMQPPublisherService::AMQPPublisherService() : AMQPService() {}

void AMQPPublisherService::SetParameters(const AMQPSQLCStr &connection_string,
                                         const std::string &exchange) {
  c_string_ = connection_string;
  default_exchange_ = exchange;
}

AMQPPublisherService::AMQPPublisherService(const AMQPSQLCStr &connection_string,
                                           const std::string &exchange)
    : AMQPService(connection_string),
      default_exchange_(exchange) {}

void AMQPPublisherService::Reconnect() {

  if (IsConnected()) {
    return;
  }
  if (!connection_ || !connection_->ready()) {
    if (c_string_.operator std::string_view().empty()) {
      throw shared::zeroSize(VARIABLE_NAME(c_string_));
    }
    connection_ = std::make_unique<AMQP::TcpConnection>(handler_.get(), AMQP::Address(c_string_.to_string()));
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

void AMQPPublisherService::AddQueue(const std::string &queue, bool create) {
  if (create) {
    DeclareQueue(*channel_, queue, default_exchange_);
  }
}

AMQPPublisherService::~AMQPPublisherService() {
  Disconnect();
}

void AMQPPublisherService::Publish(EnvelopePtr message, std::string_view qname) {
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