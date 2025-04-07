#include <drogon/drogon.h>
#include <controller/ClusterConfigController.h>

using namespace drogon;
using rest::v1::ClusterConfigController;

int main() {
    app().loadConfigFile("config/server_config.json").run();
}