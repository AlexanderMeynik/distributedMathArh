#include <drogon/drogon.h>
#include <controller/CompNode.h>

using namespace drogon;
using rest::v1::CompNode;


int main(int argc, char *argv[]) {
  app().loadConfigFile("config/server_config.json");
  if (argc == 2) {
    int port = std::stoi(argv[1]);
    auto list = app().getListeners();
    app().addListener("0.0.0.0", port);
    std::cout << port << '\n';
    app().run();
  } else {
    return -1;
  }

}