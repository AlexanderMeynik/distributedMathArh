#include "network_shared/amqpCommon.h"

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

std::string ConstructCString(const std::string &host_port,
                             const std::string &user,
                             const std::string &password,
                             bool secure) {
  auto res = fmt::format("amqp{}://{}:{}@{}/", (secure ? "s" : ""), user, password, host_port);
  return res;
}

AMQP::Address ConstructCAddress(const std::string &host_port,
                                const std::string &user,
                                const std::string &password,
                                bool secure) {
  return AMQP::Address(ConstructCString(host_port, user, password, secure));
}

MyHandler::MyHandler(boost::asio::io_service &service,
                     std::unique_ptr<boost::asio::io_service::work> &work_ref) :
    AMQP::LibBoostAsioHandler(service),
    m_work(work_ref),
    connected_(false) {}

bool MyHandler::IsConnected() const {
  return connected_;
}

}