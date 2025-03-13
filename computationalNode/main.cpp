#include <drogon/drogon.h>
#include <controller/CompNode.h>
using namespace drogon;
using rest::v1::CompNode;
int main() {
    app().loadConfigFile("config/server_config.json").run();

}