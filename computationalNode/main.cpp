#include <controller/CompNode.h>
#include <csignal>

using namespace drogon;
using rest::v1::CompNode;

void signalHandler(int) {
  //todo remake
  fmt::print("Received sigterm, initiating shutdown...\n");
  app().getLoop()->queueInLoop([]() {
    app().quit();
  });
}

int main(int argc, char *argv[]) {

  app().loadConfigFile("config/server_config.json");
  signal(SIGTERM, signalHandler);
  int port;
  if (argc >= 2) {
    try {
      port = std::stoi(argv[1]);
    }
    catch (std::invalid_argument &inv) {
      fmt::print("Invalid argument  exception {} occurred \n argv[1] = {} ",
                               inv.what(), argv[1]);
      return -1;
    }

    if (argc == 3) {
      CompNode::test_ = argv[2][0]!='0';
    }

    app().addListener("0.0.0.0", port);
    app().run();


    fmt::print("Computational node {} has shut down gracefully.\n",port);
    return 0;
  } else {
    fmt::print("usage: compNode <port> |<mode>\n <port> -will be used to create listener\n<mode> - is optional and if is not 0"
               " will replace benchmark run with dummy programm, defaults to false");
    return -1;
  }

}