#include "controller/CompNode.h"
#include "common/Printers.h"

/*using namespace rest::v1;*/
namespace rest::v1 {
void CompNode::GetStatus(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {

  Json::Value res;
  res["status"] = handler_->amqp_prod_.IsConnected() ? "running" : "not running";

  res["bench"] = print_utils::ContinuousToJson(bench_res_);

  callback(HttpResponse::newHttpJsonResponse(res));

}

void CompNode::ConnectHandler(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {

  Json::Reader reader;
  Json::Value val;

  Json::Value res;

  bool parsing_successful = reader.parse(req->bodyData(), val);
  if (!parsing_successful) {
    auto r = HttpResponse::newHttpJsonResponse(res);
    r->setStatusCode(HttpStatusCode::k415UnsupportedMediaType);
    callback(r);
    return;
  }

  std::string ip = val["ip"].asString();
  std::string name = val["name"].asString();
  std::string user = val["user"].asString();
  std::string pass = val["password"].asString();

  res["input"] = ip;
  res["name"] = name;

  if (handler_->amqp_prod_.IsConnected()) {
    auto r = HttpResponse::newHttpJsonResponse(res);
    r->setStatusCode(HttpStatusCode::k208AlreadyReported);
    callback(r);
    return;
  }

  handler_->Connect(user, pass, ip, name);

  res["bench"] = print_utils::ContinuousToJson(bench_res_);
  auto r = HttpResponse::newHttpJsonResponse(res);

  callback(r);

}

void CompNode::DisconnectHandler(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
  Json::Value res;
  res["request"] = "Disconnect";

  if (!handler_->amqp_prod_.IsConnected()) {
    auto r = HttpResponse::newHttpJsonResponse(res);
    r->setStatusCode(HttpStatusCode::k409Conflict);
    callback(r);
    return;
  }
  auto r = HttpResponse::newHttpJsonResponse(res);
  handler_->Disconnect();

  callback(r);

}
}