
#include "network_shared/AMQPConsumerService.h"
#include <thread>

static volatile std::sig_atomic_t signalReceived = 0;

void SignalHandler(int signal) {
  if (signal == SIGINT) {
    signalReceived = 1;
  }
}

int main(int argc, char *argv[]) {

  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <cString> <queue_name>\n";
    return 1;
  }
  amqp_common::MessageCallback a = [](const AMQP::Message &ms, uint64_t delivery_tag, bool redelivered) {
    std::cout << delivery_tag << '\n';
  };
  auto cString = argv[1];
  auto qname = argv[2];

  std::signal(SIGINT, SignalHandler);
  using namespace amqp_common;

  AMQPConsumerService listener(cString, qname);
  listener.SetMessageCallback(a);
  listener.Connect();

  while (!signalReceived) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  listener.Disconnect();
  return 0;
}
