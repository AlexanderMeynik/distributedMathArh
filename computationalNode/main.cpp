#include <drogon/drogon.h>
#include <controller/CompNode.h>

using namespace drogon;
using rest::v1::CompNode;

int main(int argc, char *argv[]) {
  app().loadConfigFile("config/server_config.json");
  int port;
  if (argc == 2) {
    try {
      port = std::stoi(argv[1]);
    }
    catch (std::invalid_argument &inv) {
      //todo use logger
      std::cout << fmt::format("Invalid argument  exception {} occurred \n argv[1] = {} ",
                               inv.what(), argv[1]);
      return -1;
    }
    app().addListener("0.0.0.0", port);
    std::cout << port << '\n';
    app().run();
  } else {
    return -1;
  }

}