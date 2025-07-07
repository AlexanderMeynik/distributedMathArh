#include "controller/ClusterConfigController.h"

/// Namespace for main node rest api handlers
namespace rest::v1 {

void
ClusterConfigController::GetStatus(const HttpRequestPtr &req,
                                   std::function<void(const HttpResponsePtr &)> &&callback) {

  auto json_output = main_node_service_->Status();

  auto res = HttpResponse::newHttpJsonResponse(json_output);
  res->setStatusCode(static_cast<HttpStatusCode>(json_output["status"].asUInt()));

  callback(res);

}

void ClusterConfigController::ConnectNodeHandler(const HttpRequestPtr &req,
                                                 std::function<void(const HttpResponsePtr &)> &&callback,
                                                 const std::string &host_port) {

  Json::Value res;

  res = main_node_service_->ConnectNode(host_port, host_port);

  auto http_response = HttpResponse::newHttpJsonResponse(res);
  http_response->setStatusCode(static_cast<HttpStatusCode>(res["status"].asUInt()));

  callback(http_response);

}

void ClusterConfigController::DisconnectNodeHandler(const HttpRequestPtr &req,
                                                    std::function<void(const HttpResponsePtr &)> &&callback,
                                                    const std::string &host_port) {

  Json::Value res;

  res = main_node_service_->DisconnectNode(host_port);

  auto http_response = HttpResponse::newHttpJsonResponse(res);
  http_response->setStatusCode(static_cast<HttpStatusCode>(res["status"].asUInt()));

  callback(http_response);

}


void ClusterConfigController::SentToExecution(const HttpRequestPtr &req,
                                              std::function<void(const HttpResponsePtr &)> &&callback) {
  network_types::TestSolveParam ts(*req->getJsonObject());
  auto json_output = main_node_service_->SendToExecution(ts);

  auto http_response = HttpResponse::newHttpJsonResponse(json_output);
  http_response->setStatusCode(static_cast<HttpStatusCode>(json_output["status"].asUInt()));
  callback(http_response);
}
void ClusterConfigController::ConnectQ(const HttpRequestPtr &req,
                                       std::function<void(const HttpResponsePtr &)> &&callback) {

  auto json = *req->getJsonObject();
  auto user = json["user"].asString();
  auto password = json["password"].asString();
  auto qname = json["queue_host"].asString();

  auto json_output = main_node_service_->Connect(user,password,qname);

  auto http_response = HttpResponse::newHttpJsonResponse(json_output);
  http_response->setStatusCode(static_cast<HttpStatusCode>(json_output["status"].asUInt()));
  callback(http_response);
}
void ClusterConfigController::DisconnectQ(const HttpRequestPtr &req,
                                          std::function<void(const HttpResponsePtr &)> &&callback) {

  auto json_output = main_node_service_->Disconnect();

  auto http_response = HttpResponse::newHttpJsonResponse(json_output);
  http_response->setStatusCode(static_cast<HttpStatusCode>(json_output["status"].asUInt()));
  callback(http_response);

}
void ClusterConfigController::Rebalance(const HttpRequestPtr &req,
                                        std::function<void(const HttpResponsePtr &)> &&callback) {


  auto json_output = main_node_service_->Rebalance();

  auto http_response = HttpResponse::newHttpJsonResponse(json_output);
  http_response->setStatusCode(static_cast<HttpStatusCode>(json_output["status"].asUInt()));
  callback(http_response);
}
void ClusterConfigController::SoftTerminate(const HttpRequestPtr &req,
                                            std::function<void(const HttpResponsePtr &)> &&callback) {

  Json::Value res=main_node_service_->CleanUp();
  res["status"]=k200OK;
  res["message"]="Main node {} received soft_terminate request";
  res["request"]="soft_terminate";

  app().getLoop()->queueInLoop([]() {
    app().quit();
  });

  auto response = HttpResponse::newHttpJsonResponse(res);

  response->setStatusCode(static_cast<HttpStatusCode>(res["status"].asUInt()));

  callback(response);
}
}
