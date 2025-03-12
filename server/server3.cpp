#include <drogon/drogon.h>

using namespace drogon;

int main() {
    app().setLogPath("./")
            .setLogLevel(trantor::Logger::kWarn)
            .addListener("0.0.0.0", 8080) // Change to 80 if you have the necessary permissions
            .setThreadNum(16);

    // Register the handler before running the app
    app().registerHandler("/test?username={name}",
                          [](const HttpRequestPtr& req,
                             std::function<void (const HttpResponsePtr &)> &&callback,
                             const std::string &name)
                          {
                              Json::Value json;
                              json["result"] = "ok";
                              json["message"] = std::string("hello, ") + name;
                              auto resp = HttpResponse::newHttpJsonResponse(json);
                              callback(resp);
                          },
                          {Get, "LoginFilter"});

    // Now run the application
    app().run();
}