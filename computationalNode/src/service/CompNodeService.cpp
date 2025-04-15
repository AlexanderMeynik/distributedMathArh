#include "service/CompNodeService.h"

namespace comp_services {

Json::Value ComputationNodeService::Disconnect() {
  Json::Value res_JSON;
  res_JSON["request"] = "disconnect";
  if (!amqp_prod_.IsConnected()) {

    res_JSON["status"] = HttpStatusCode::k409Conflict;
    res_JSON["message"] = "This worker node is currently not connected to any cluster";
    return res_JSON;
  }

  amqp_prod_.Disconnect();

  res_JSON["status"] = drogon::HttpStatusCode::k200OK;

  return res_JSON;
}
Json::Value ComputationNodeService::Connect(const HttpRequestPtr &req) {
  Json::Value res_JSON;
  Json::Value val;
  Json::Reader reader;
  res_JSON["request"] = "connect";

  bool parsing_successful = reader.parse(req->bodyData(), val);
  if (!parsing_successful) {
    val["status"] = HttpStatusCode::k415UnsupportedMediaType;
    return res_JSON;
  }

  std::string ip = val["ip"].asString();
  std::string name = val["name"].asString();
  std::string user = val["user"].asString();
  std::string pass = val["password"].asString();

  res_JSON["input"] = ip;
  res_JSON["name"] = name;
  res_JSON["bench"] = ContinuousToJson(bench_res_, true, true);

  if (amqp_prod_.IsConnected()) {
    res_JSON["status"] = HttpStatusCode::k409Conflict;
    res_JSON["message"] = fmt::format("Node is already connected to RabbitMQ {}!",
                                      amqp_prod_.GetCString());
    //todo remove user data

    return res_JSON;
  }

  auto c = amqp_common::ConstructCString(ip, user, pass);

  amqp_prod_.SetParameters(c, name);

  try {
    amqp_prod_.Connect();
  }
  catch (std::runtime_error &err) {
    res_JSON["status"] = HttpStatusCode::k409Conflict;
    res_JSON["message"] = fmt::format("Queue connection error {} !", err.what());
    return res_JSON;
  }

  res_JSON["status"] = drogon::HttpStatusCode::k200OK;
  return res_JSON;
}
bool ComputationNodeService::CheckConnection() {
  return amqp_prod_.IsConnected();
}

}
