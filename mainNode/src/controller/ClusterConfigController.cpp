#include "controller/ClusterConfigController.h"

/// Namespace for main node rest api handlers
namespace rest::v1 {

void
ClusterConfigController::GetStatus(const HttpRequestPtr &req,
                                   std::function<void(const HttpResponsePtr &)> &&callback) {

  WrapServiceCall(req, std::move(callback),
                  [this](const drogon::HttpRequestPtr &) {
                    return main_node_service_->Status();
                  });
}

void ClusterConfigController::ConnectNodeHandler(const HttpRequestPtr &req,
                                                 std::function<void(const HttpResponsePtr &)> &&callback,
                                                 const std::string &host_port) {
  WrapServiceCall(req, std::move(callback),
                  [this, host_port](const drogon::HttpRequestPtr &) {
                    return main_node_service_->ConnectNode(host_port, host_port);
                  });

}

void ClusterConfigController::DisconnectNodeHandler(const HttpRequestPtr &req,
                                                    std::function<void(const HttpResponsePtr &)> &&callback,
                                                    const std::string &host_port) {
  WrapServiceCall(req, std::move(callback),
                  [this, host_port](const drogon::HttpRequestPtr &) {
                    return main_node_service_->DisconnectNode(host_port);
                  });
}

void ClusterConfigController::SentToExecution(const HttpRequestPtr &req,
                                              std::function<void(const HttpResponsePtr &)> &&callback) {
  WrapServiceCall(req, std::move(callback),
                  [this](const drogon::HttpRequestPtr &req) {
                    network_types::TestSolveParam ts(*req->getJsonObject());
                    return main_node_service_->SendToExecution(ts);
                  });
}
void ClusterConfigController::ConnectQ(const HttpRequestPtr &req,
                                       std::function<void(const HttpResponsePtr &)> &&callback) {
  WrapServiceCall(req, std::move(callback),
                  [this](const drogon::HttpRequestPtr &req) {
                    auto json = *req->getJsonObject();
                    auto user = json["user"].asString();
                    auto password = json["password"].asString();
                    auto qname = json["queue_host"].asString();
                    return main_node_service_->Connect(user, password, qname);
                  });
}
void ClusterConfigController::DisconnectQ(const HttpRequestPtr &req,
                                          std::function<void(const HttpResponsePtr &)> &&callback) {
  WrapServiceCall(req, std::move(callback),
                  [this](const drogon::HttpRequestPtr &) {
                    return main_node_service_->Disconnect();
                  });
}
void ClusterConfigController::Rebalance(const HttpRequestPtr &req,
                                        std::function<void(const HttpResponsePtr &)> &&callback) {
  WrapServiceCall(req, std::move(callback),
                  [this](const drogon::HttpRequestPtr &) {
                    return main_node_service_->Rebalance();
                  });
}
void ClusterConfigController::SoftTerminate(const HttpRequestPtr &req,
                                            std::function<void(const HttpResponsePtr &)> &&callback) {

  WrapServiceCall(req, std::move(callback),
                  [this](const drogon::HttpRequestPtr &) {
                    Json::Value res = main_node_service_->CleanUp();
                    res["status"] = k200OK;
                    res["message"] = "Main node {} received soft_terminate request";
                    res["request"] = "soft_terminate";

                    app().getLoop()->queueInLoop([]() {
                      app().quit();
                    });
                    return res;
                  });
}
void WrapServiceCall(const HttpRequestPtr &req,
                     std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                     std::function<Json::Value(const drogon::HttpRequestPtr &)> service_call) {
  auto json_output = service_call(req);
  auto res = drogon::HttpResponse::newHttpJsonResponse(json_output);
  res->setStatusCode(static_cast<drogon::HttpStatusCode>(json_output["status"].asUInt()));
  callback(res);
}
}
