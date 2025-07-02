#include "service/CompNodeService.h"
#include "network_shared/sharedConstants.h"

namespace comp_services {

Json::Value ComputationNodeService::GetStatus(bool rebalance) {
  Json::Value res_json;
  res_json["request"] = "status";
  res_json["worker_status"] = IsConnected() ? "running" : "not running";
  if (bench_res_.has_value()) {
    auto key = rebalance ? "old_bench" : "bench";
    res_json[key] = print_utils::ContinuousToJson(bench_res_.value(), true, true);
  }
  if (!c_data_.empty()) {
    res_json["connected_to"] = c_data_;
  }
  res_json["status"] = drogon::HttpStatusCode::k200OK;
  return res_json;
}

Json::Value ComputationNodeService::Disconnect() {
  Json::Value res_json;
  res_json["request"] = "disconnect";
  if (!IsConnected()) {

    res_json["status"] = HttpStatusCode::k409Conflict;
    res_json["message"] = "This worker node is currently not connected to any cluster";
    return res_json;
  }

  consumer_service_.Disconnect();
  c_data_.clear();

  res_json["status"] = drogon::HttpStatusCode::k200OK;

  return res_json;
}
Json::Value ComputationNodeService::Connect(const HttpRequestPtr &req) {
  Json::Value res_json;
  Json::Value val;
  Json::Reader reader;
  res_json["request"] = "connect";

  bool parsing_successful = reader.parse(req->bodyData(), val);
  if (!parsing_successful) {
    val["status"] = HttpStatusCode::k415UnsupportedMediaType;
    return res_json;
  }

  std::string ip = val["ip"].asString();
  std::string name = val["name"].asString();
  std::string user = val["user"].asString();
  std::string pass = val["password"].asString();

  c_data_["ip"] = ip;
  c_data_["qname"] = name;

  res_json["input"] = ip;
  res_json["name"] = name;
  res_json["bench"] = print_utils::ContinuousToJson(bench_res_.value(), true, true);

  auto c = network_types::AMQPSQLCStr(ip, user, pass);

  if (IsConnected()) {
    res_json["status"] = HttpStatusCode::k409Conflict;
    res_json["message"] = fmt::format("Node is already connected to RabbitMQ {}!",
                                      consumer_service_.GetCString().GetVerboseName());

    return res_json;
  }

  consumer_service_.SetParameters(c, name);

  consumer_service_.SetMessageCallback(n_message_callback);

  try {
    consumer_service_.Connect();
  }
  catch (std::runtime_error &err) {
    res_json["status"] = HttpStatusCode::k409Conflict;
    res_json["message"] = fmt::format("Queue connection error msg :\"{}\"!", err.what());
    return res_json;
  }

  res_json["status"] = drogon::HttpStatusCode::k200OK;
  return res_json;
}
bool ComputationNodeService::IsConnected() {
  return consumer_service_.IsConnected();
}

void ComputationNodeService::RunBench() {

  characteristic_computed_.store(false, std::memory_order_release);

  computation_thread_ = std::jthread([this]() {
    std::cout << "Job start\n";
    bench_res_ = benchmark_runner_->Run(true,0,test_).first;

    std::cout << "Job done\n";
    characteristic_computed_.store(true, std::memory_order_release);
  });
}
ComputationNodeService::ComputationNodeService() {
  benchmark_runner_ = std::make_unique<BenchmarkRunner>(shared::ns, shared::iter_count);
  RunBench();
}
Json::Value ComputationNodeService::Rebalance() {
  auto node_st = GetStatus(true);
  node_st["request"] = "rebalance_node";
  if (!Computed()) {
    node_st["status"] = drogon::HttpStatusCode::k409Conflict;
    node_st["message"] = "Rabalancing is already running try again later";
    return node_st;
  }
  RunBench();
  return node_st;
}
bool ComputationNodeService::Computed() {
  return characteristic_computed_.load(std::memory_order_acquire);
}
bool ComputationNodeService::IsTest() const {
  return test_;
}
void ComputationNodeService::SetTest(bool test) {
  test_ = test;
}

}
