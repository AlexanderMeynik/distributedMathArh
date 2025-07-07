#include "controller/CompNode.h"

namespace rest::v1 {
void CompNode::GetStatus(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {

  Json::Value res = handler_->GetStatus();
  auto response = HttpResponse::newHttpJsonResponse(res);

  response->setStatusCode(static_cast<HttpStatusCode>(res["status"].asUInt()));

  callback(response);

}

void CompNode::ConnectHandler(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {

  Json::Value res = handler_->Connect(req);

  auto response = HttpResponse::newHttpJsonResponse(res);

  response->setStatusCode(static_cast<HttpStatusCode>(res["status"].asUInt()));

  callback(response);

}

void CompNode::DisconnectHandler(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {

  Json::Value res = handler_->Disconnect();

  auto response = HttpResponse::newHttpJsonResponse(res);

  response->setStatusCode(static_cast<HttpStatusCode>(res["status"].asUInt()));

  callback(response);

}
void CompNode::RebalanceHandler(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {

  Json::Value res = handler_->Rebalance();

  auto response = HttpResponse::newHttpJsonResponse(res);

  response->setStatusCode(static_cast<HttpStatusCode>(res["status"].asUInt()));

  callback(response);
}
void CompNode::SoftTerminate(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {


  Json::Value res;
  res["status"]=k200OK;
  res["message"]="Computational node {} received soft_terminate request";
  res["request"]="soft_terminate";

  app().getLoop()->queueInLoop([]() {
    app().quit();
  });

  auto response = HttpResponse::newHttpJsonResponse(res);

  response->setStatusCode(static_cast<HttpStatusCode>(res["status"].asUInt()));

  callback(response);

}
}