#include "network_shared/AMQPConsumerService.h"
#include <fmt/format.h>

namespace amqp_common {

void AMQPConsumerService::Connect() {
  service_.reset();
  work_ = std::make_unique<boost::asio::io_service::work>(service_);

  promise_set_ = false;
  connection_promise_ = std::promise<std::string>();
  auto connection_future = connection_promise_.get_future();

  Reconnect();
  service_thread_ = std::thread([this]() { service_.run(); });

  std::string error = connection_future.get();
  if (!error.empty()) {
    Disconnect();
    throw std::runtime_error(error);
  }

}

void AMQPConsumerService::Reconnect() {
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
    const GuardType kGuard{s_mutex_};
    if (!promise_set_) {
      connection_promise_.set_value(message);
      promise_set_ = true;
      std::cout << "Channel error: " << message << '\n';
    }
  });

  auto start_cb = [this](const std::string &consumer_tag) {
    const GuardType kGuard{s_mutex_};
    if (!promise_set_) {
      connection_promise_.set_value("");
      promise_set_ = true;
      std::cout << "Consumption started successfully with consumer tag: " << consumer_tag << '\n';
    }
  };

  auto error_cb = [this](const char *message) {
    const GuardType kGuard{s_mutex_};
    if (!promise_set_) {
      connection_promise_.set_value(message);
      std::cout << "Consumption error: " << message << '\n';
      promise_set_ = true;
    }

  };

  channel_->consume(queue_)
      .onReceived(([this](const AMQP::Message &message, uint64_t delivery_tag, bool redelivered) {
        if (message_callback_) {
          message_callback_(message, delivery_tag, redelivered);
        } else {
          amqp_common::d_message_callback(message, delivery_tag, redelivered);
        }
        channel_->ack(delivery_tag);
      }))
      .onSuccess(start_cb)
      .onError(error_cb);

}

void AMQPConsumerService::Disconnect() {

  service_.post([this]() {
    channel_->close();
    connection_->close();
  });

  if (service_thread_.joinable()) {
    service_thread_.join();
  }
  service_.stop();
  connection_.reset();
  channel_.reset();
}

AMQPConsumerService::~AMQPConsumerService() {
  Disconnect();
}

AMQPConsumerService::AMQPConsumerService() : service_(1),
                                             handler_(std::make_unique<MyHandler>(service_, work_)) {

}

AMQPConsumerService::AMQPConsumerService(const std::string &connection_string,
                                         const std::string &queue_name) :
    service_(1),
    handler_(std::make_unique<MyHandler>(service_, work_)),
    queue_(queue_name),
    c_string_(connection_string) {

}

void AMQPConsumerService::SetParameters(const std::string &connection_string, const std::string &queue_name) {
  c_string_ = connection_string;
  queue_ = queue_name;
}

void AMQPConsumerService::SetMessageCallback(MessageCallback callback) {
  message_callback_ = std::move(callback);
}

bool AMQPConsumerService::IsConnected() const {
  return handler_ && handler_->IsConnected();
}
const std::string &AMQPConsumerService::GetCString() const {
  return c_string_;
}

}