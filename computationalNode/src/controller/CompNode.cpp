#include "controller/CompNode.h"
#include "common/Printers.h"

using namespace rest::v1;

void CompNode::GetStatus(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {

  Json::Value res;
  res["status"] = handler_->con_ ? "running" : "not running";
  res["cc"] = *(handler_->cc_);

  res["bench"] = print_utils::ContinuousToJson(bench_res_);

  callback(HttpResponse::newHttpJsonResponse(res));

}

void CompNode::ConnectHandler(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback,
                              const std::string &ip, const std::string &name) {

  Json::Value res;

  res["input"] = ip;
  res["name"] = name;

  if (handler_->con_) {
    auto r = HttpResponse::newHttpJsonResponse(res);
    r->setStatusCode(HttpStatusCode::k208AlreadyReported);
    callback(r);
    return;
  }
  handler_->Connect(ip, name);
  res["bench"] = print_utils::ContinuousToJson(bench_res_);
  auto r = HttpResponse::newHttpJsonResponse(res);
  callback(r);

}

void CompNode::DisconnectHandler(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
  Json::Value res;
  res["request"] = "Disconnect";

  if (!handler_->con_) {
    auto r = HttpResponse::newHttpJsonResponse(res);
    r->setStatusCode(HttpStatusCode::k409Conflict);
    callback(r);
    return;
  }
  res["cc"] = *(handler_->cc_);
  auto r = HttpResponse::newHttpJsonResponse(res);
  handler_->Disconnect();

  callback(r);

}
