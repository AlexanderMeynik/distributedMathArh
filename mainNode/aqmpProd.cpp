

#include <iomanip>
#include "network_shared/amqpPublisherService.h"

#include <fmt/format.h>
#include <json/json.h>

///amqpCommon namespace
using namespace amqp_common;

static volatile std::sig_atomic_t signalReceived = 0;

void SignalHandler(int signal) {
  if (signal == SIGINT) {
    signalReceived = 1;
  }
}

using namespace amqp_common;

int main(int argc, char *argv[]) {
  if (argc != 4) {
    std::cerr << "Usage: " << argv[0] << " <cString> <number_of_messages>  <number_of_queues>\n";
    return 1;
  }

  auto c_string = argv[1];

  int num_messages;
  int numb_queues;
  try {
    num_messages = std::stoi(argv[2]);
    numb_queues = std::stoi(argv[3]);
    if (num_messages <= 0) {
      throw std::invalid_argument("Number of messages must be positive.");
    }
    if (numb_queues <= 0) {
      throw std::invalid_argument("Number of queues must be positive.");
    }
  } catch (const std::exception &e) {
    std::cerr << "Error: Invalid number of messages - " << e.what() << '\n';
    return 1;
  }

  std::signal(SIGINT, SignalHandler);

  amqpPublisherService service(c_string);

  service.RestartLoop();

  for (int i = 0; i < numb_queues; ++i) {
    service.AddQueue(fmt::format("queue{}", i), true);
  }

  for (int i = 0; i < numb_queues; ++i) {
    std::string q = fmt::format("queue{}", i);
    for (int j = 0; j < num_messages; ++j) {

      Json::Value message;
      message["number"] = i;

      auto t = std::time(nullptr);
      auto tm = *std::localtime(&t);

      std::stringstream ss;
      ss << std::put_time(&tm, "%d-%m-%Y %H-%M-%S");
      message["timestamp"] = ss.str();

      auto str = message.toStyledString();
      auto envelope = std::make_shared<AMQP::Envelope>(str);

      envelope->setPersistent(true);
      envelope->setPriority(num_messages - j);
      AMQP::Table headers;
      headers["messageNum"] = j;
      headers["tt"] = ss.str();

      envelope->setHeaders(headers);

      service.Publish(envelope, i);
    }
  }

  while (!signalReceived) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  service.EndLoop();

  return 0;
}



