#include <drogon/drogon.h>
#include <controller/CompNode.h>
#include <controller/CompNode2.h>
using namespace drogon;
using rest::v1::CompNode;
int main() {
    app().setLogPath("./")
            .setLogLevel(trantor::Logger::kWarn)
            .addListener("0.0.0.0", 8080)
            .setThreadNum(1);
    app().run();

}