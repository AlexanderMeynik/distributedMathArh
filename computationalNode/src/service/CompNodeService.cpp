#include "service/CompNodeService.h"
#include "common/Printers.h"

namespace comp_services {

Json::Value ComputationNodeService::GetStatus() {
  Json::Value res_JSON;
  res_JSON["request"] = "status";
  res_JSON["worker_status"] = consumer_service_.IsConnected() ? "running" : "not running";
  if (Computed())//todo what will this thing impact
    res_JSON["bench"] = print_utils::ContinuousToJson(bench_res_, true, true);
  res_JSON["status"] = drogon::HttpStatusCode::k200OK;
  return res_JSON;
}

Json::Value ComputationNodeService::Disconnect() {
  Json::Value res_JSON;
  res_JSON["request"] = "disconnect";
  if (!consumer_service_.IsConnected()) {

    res_JSON["status"] = HttpStatusCode::k409Conflict;
    res_JSON["message"] = "This worker node is currently not connected to any cluster";
    return res_JSON;
  }

  consumer_service_.Disconnect();

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
  res_JSON["bench"] = print_utils::ContinuousToJson(bench_res_, true, true);

  if (consumer_service_.IsConnected()) {
    res_JSON["status"] = HttpStatusCode::k409Conflict;
    res_JSON["message"] = fmt::format("Node is already connected to RabbitMQ {}!",
                                      consumer_service_.GetCString());
    //todo remove user data

    return res_JSON;
  }

  auto c = amqp_common::ConstructCString(ip, user, pass);

  consumer_service_.SetParameters(c, name);

  consumer_service_.SetMessageCallback(n_message_callback);

  try {
    consumer_service_.Connect();
  }
  catch (std::runtime_error &err) {
    res_JSON["status"] = HttpStatusCode::k409Conflict;
    res_JSON["message"] = fmt::format("Queue connection error msg :\"{}\"!", err.what());
    return res_JSON;
  }

  res_JSON["status"] = drogon::HttpStatusCode::k200OK;
  return res_JSON;
}
bool ComputationNodeService::CheckConnection() {
  return consumer_service_.IsConnected();
}

void ComputationNodeService::RunBench() {

  characteristic_computed_.store(false, std::memory_order_release);

  computation_thread_ = std::jthread([this]() {
    std::cout << "Job start\n";
    bench_res_ = benchmarkRunner.Run().first;
    //bench_res_ = DefaultBench();
    std::cout << "Job done\n";
    characteristic_computed_.store(true, std::memory_order_release);
  });
}

}
