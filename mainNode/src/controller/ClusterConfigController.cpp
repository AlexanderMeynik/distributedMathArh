#include "controller/ClusterConfigController.h"
#include "common/Parsers.h"
#include "common/Printers.h"

using namespace rest::v1;

void
ClusterConfigController::GetStatus(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {

  auto json_output = main_node_service_->Status();

  auto res = HttpResponse::newHttpJsonResponse(json_output);
  res->setStatusCode(static_cast<HttpStatusCode>(json_output["status"].asUInt()));

  callback(res);

}

void ClusterConfigController::ConnectHandler(const HttpRequestPtr &req,
                                             std::function<void(const HttpResponsePtr &)> &&callback,
                                             const std::string &host_port) {

  Json::Value res;

  res = main_node_service_->ConnectNode(host_port, host_port);

  auto http_response = HttpResponse::newHttpJsonResponse(res);
  http_response->setStatusCode(static_cast<HttpStatusCode>(res["status"].asUInt()));

  callback(http_response);

}

void ClusterConfigController::DisconnectHandler(const HttpRequestPtr &req,
                                                std::function<void(const HttpResponsePtr &)> &&callback,
                                                const std::string &host_port) {

  Json::Value res;

  res = main_node_service_->DisconnectNode(host_port);

  auto http_response = HttpResponse::newHttpJsonResponse(res);
  http_response->setStatusCode(static_cast<HttpStatusCode>(res["status"].asUInt()));

  callback(http_response);

}
void ClusterConfigController::SentMessage(const HttpRequestPtr &req,
                                          std::function<void(const HttpResponsePtr &)> &&callback,
                                          const std::string &node) {

  network_types::TestSolveParam ts(*req->getJsonObject());
  auto json_output = main_node_service_->Publish(ts, node);

  auto http_response = HttpResponse::newHttpJsonResponse(json_output);
  http_response->setStatusCode(static_cast<HttpStatusCode>(json_output["status"].asUInt()));
  callback(http_response);

}
void ClusterConfigController::ConnectQ(const HttpRequestPtr &req,
                                       std::function<void(const HttpResponsePtr &)> &&callback) {

  auto json = *req->getJsonObject();

  auto qname = json["queue_host"].asString();
  auto szs = json["queues"].size();
  auto queus = print_utils::JsonToContinuous<std::vector<std::string>>(
      json["queues"], szs);

  auto json_output = main_node_service_->Connect(qname, queus);

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
