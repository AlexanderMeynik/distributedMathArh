#include "network_shared/AMQPConsumerService.h"
#include <fmt/format.h>

namespace amqp_common {

void AMQPConsumerService::Reconnect() {
  if (IsConnected()) {
    return;
  }
  if (!connection_ || !connection_->ready()) {
    if (c_string_.operator std::string_view().empty()) {
      throw shared::zeroSize(VARIABLE_NAME(c_string_));
    }
    connection_ = std::make_unique<AMQP::TcpConnection>(handler_.get(),
                                                        AMQP::Address(c_string_.to_string()));
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

  auto start_cb = [this](const std::string &consumer_tag) {
    GuardType kGuard{s_mutex_};
    if (!promise_set_) {
      connection_promise_.set_value("");
      promise_set_ = true;
      std::cout << "Consumption started successfully with consumer tag: " << consumer_tag << '\n';
    }
  };

  ///@todo find way to signal
  auto error_cb = [](const char *message) {
    std::cout << "Consumption error: " << message << '\n';
  };

  channel_->consume(queue_)
      .onReceived(([this](const AMQP::Message &message, uint64_t delivery_tag, bool redelivered) {
        if (message_callback_) {
          message_callback_(message, delivery_tag, redelivered);
        } else {
          amqp_common::DefaultMessageCallback(message, delivery_tag, redelivered);
        }
        channel_->ack(delivery_tag);
      }))
      .onSuccess(start_cb)
      .onError(error_cb);

}

AMQPConsumerService::AMQPConsumerService() : AMQPService() {

}

AMQPConsumerService::AMQPConsumerService(const AMQPSQLCStr &connection_string,
                                         const std::string &queue_name) :
    AMQPService(connection_string),
    queue_(queue_name) {}

void AMQPConsumerService::SetParameters(const AMQPSQLCStr &connection_string, const std::string &queue_name) {
  c_string_ = connection_string;
  queue_ = queue_name;
}
void AMQPConsumerService::SetMessageCallback(MessageCallback callback) {
  message_callback_ = std::move(callback);
}
std::string AMQPConsumerService::ServiceName() {
  return "AMQPConsumerService";
}

}