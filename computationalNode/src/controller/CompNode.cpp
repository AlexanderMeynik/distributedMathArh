#include "controller/CompNode.h"

namespace rest::v1 {
void CompNode::GetStatus(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
  WrapServiceCall(req, std::move(callback),
                  [this](const drogon::HttpRequestPtr){
                    return handler_->GetStatus();
                  });
}

void CompNode::ConnectHandler(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
  WrapServiceCall(req, std::move(callback),
                  [this](const drogon::HttpRequestPtr&req){
                    return handler_->Connect(req);
                  });
}

void CompNode::DisconnectHandler(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
  WrapServiceCall(req, std::move(callback),
                  [this](const drogon::HttpRequestPtr&){
                    return handler_->Disconnect();
                  });
}
void CompNode::RebalanceHandler(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
  WrapServiceCall(req, std::move(callback),
                  [this](const drogon::HttpRequestPtr&){
                    return handler_->Rebalance();
                  });
}
void CompNode::SoftTerminate(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {

  WrapServiceCall(req, std::move(callback),
                  [this](const drogon::HttpRequestPtr&){
    Json::Value res;
    res["status"]=k200OK;
    res["message"]="Computational node {} received soft_terminate request";
    res["request"]="soft_terminate";

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