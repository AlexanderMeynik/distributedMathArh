#include "network_shared/amqpCommon.h"
#include "network_shared/connectionString.h"

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

MyHandler::MyHandler(boost::asio::io_service &service,
                     std::unique_ptr<boost::asio::io_service::work> &work_ref) :
    AMQP::LibBoostAsioHandler(service),
    m_work_(work_ref),
    connected_(false) {}

bool MyHandler::IsConnected() const {
  return connected_;
}
void MyHandler::onClosed(AMQP::TcpConnection *connection) {
  ResetLoop();

  std::cout << "Connection closed.\n";
}
void MyHandler::onError(AMQP::TcpConnection *connection, const char *message) {
  std::cout << "Connection error: " << message << '\n';///@todo log
  ResetLoop();
}
void MyHandler::onConnected(AMQP::TcpConnection *connection) {
  std::cout << "Connection established successfully." << '\n';
  connected_ = true;
}
void MyHandler::ResetLoop() {
  m_work_.reset();
  connected_ = false;
}

std::string ConstructCString(const std::string &host_port,
                             const std::string &user,
                             const std::string &password,
                             bool secure) {

  return network_types::AMQPSQLCStr(host_port,user,password,secure).to_string();
}

AMQP::Address ConstructCAddress(const std::string &host_port,
                                const std::string &user,
                                const std::string &password,
                                bool secure) {
  return AMQP::Address(ConstructCString(host_port, user, password, secure));

}

void DefaultMessageCallback(const AMQP::Message &message, uint64_t delivery_tag, bool redelivered) {
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
}
std::optional<std::string> ExtractHost(std::string_view url) {
  size_t protocolEnd = url.find("://");
  if (protocolEnd == std::string_view::npos) {
    return std::nullopt;
  }

  size_t hostStart = protocolEnd + 3;
  if (hostStart >= url.length()) {
    return std::nullopt;
  }

  size_t hostEnd = url.find_first_of(":/?", hostStart);
  if (hostEnd == std::string_view::npos) {
    hostEnd = url.length();
  }

  return std::string(url.substr(hostStart, hostEnd - hostStart));
}

bool AMQPService::IsConnected() const {
  return handler_ && handler_->IsConnected();
}
const AMQPSQLCStr &AMQPService::GetCString() const {
  return c_string_;
}
void AMQPService::Connect() {
  if (IsConnected()) {
    throw shared::Already_Connected(ServiceName(), c_string_.to_string());
  }
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
void AMQPService::Disconnect() {
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
AMQPService::AMQPService() : service_(1),
                             handler_(std::make_unique<MyHandler>(service_, work_)) {
}
AMQPService::AMQPService(const AMQPSQLCStr &connection_string) :
    service_(1),
    handler_(std::make_unique<MyHandler>(service_, work_)),
    c_string_(connection_string) {

}



void AMQPService::SetParameters(const AMQPSQLCStr &connection_string) {
  c_string_ = connection_string;
}
AMQPService::~AMQPService() {
  Disconnect();
}

}