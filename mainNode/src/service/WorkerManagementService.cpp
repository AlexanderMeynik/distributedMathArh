#include "service/WorkerManagementService.h"

namespace main_services {

Json::Value WorkerManagementService::GetStatus() {
  Json::Value res;
  int j = 0;
  for (auto &[status, map] : worker_nodes_) {
    int i = 0;
    res[j]["status"] = kNodeStatusToStr.at(status);
    auto &ss = res[j]["data"];
    for (auto &[str, node] : map) {
      ss[i]["host"] = str;
      ss[i]["benchRes"] = print_utils::ContinuousToJson(node.bench_result_, false, true);
      i++;
    }
    j++;
  }
  return res;
}

WorkerManagementService::WorkerManagementService() {
  normalized_ = DefaultBench(0);
}

Json::Value WorkerManagementService::AddNewNode(const std::string & host_port) {
  using NodeStatus::ACTIVE;
  using NodeStatus::INACTIVE;

  Json::Value res_JSON;
  if (
      !worker_nodes_[INACTIVE].count(host_port) &&
          !worker_nodes_[ACTIVE].count(host_port)
      ) {
    ComputationalNode cn;

    cn.http_client_ = HttpClient::newHttpClient("http://" + host_port);
    cn.st_ = INACTIVE;
    worker_nodes_[INACTIVE][host_port] = std::move(cn);
  } else {
    if (worker_nodes_[ACTIVE].count(host_port)) {
      res_JSON["status"] = drogon::HttpStatusCode::k409Conflict;
      res_JSON["message"] = fmt::format("Worker node {} is already connected to cluster!", host_port);
      return res_JSON;
    }
  }
  return res_JSON;
}

Json::Value WorkerManagementService::ConnectNode(const std::string & host_port,
                                                 drogon::HttpRequestPtr req1) {
  using NodeStatus::ACTIVE;
  using NodeStatus::INACTIVE;

  Json::Value res_JSON;
  auto [code, resp] =
      worker_nodes_[INACTIVE][host_port].PerformHttpRequest(req1);

  if (code == drogon::ReqResult::BadServerAddress) {
    res_JSON["message"] = fmt::format("Unable to access worker on {}! Is working node running?", host_port);
    res_JSON["status"] = drogon::HttpStatusCode::k504GatewayTimeout;
    return res_JSON;
  }
  if (resp->getStatusCode() >= HttpStatusCode::k400BadRequest) {
    res_JSON["message"] = fmt::format("Error occurred during worker {} connection!", host_port);
    res_JSON["status"] = resp->getStatusCode();
    res_JSON["node_output"] = *resp->getJsonObject();
    return res_JSON;
  }

  auto jsoncpp = resp->getJsonObject();

  worker_nodes_[INACTIVE][host_port].RecomputeCoefficients((*jsoncpp)["bench"]);

  MoveNode(host_port, INACTIVE, ACTIVE);
  normalized_ += worker_nodes_[ACTIVE][host_port].node_speed_;

  res_JSON = *jsoncpp;
  return res_JSON;
}

Json::Value WorkerManagementService::DisconnectNode(const std::string & host_port) {
  using NodeStatus::ACTIVE;
  using NodeStatus::INACTIVE;
  Json::Value res_JSON;

  if (!worker_nodes_[ACTIVE].count(host_port.data())) {
    res_JSON["status"] = drogon::HttpStatusCode::k409Conflict;
    res_JSON["message"] = fmt::format("Worker node {} was not connected to cluster!", host_port);
    return res_JSON;
  }
  auto req1 = HttpRequest::newHttpRequest();

  req1->setPath("/v1/Disconnect");
  req1->setMethod(Post);

  auto ct = HttpClient::newHttpClient(host_port.data());
  auto [code, resp] = worker_nodes_[ACTIVE][host_port.data()].PerformHttpRequest(req1);

  if (code == drogon::ReqResult::BadServerAddress) {
    res_JSON["message"] = fmt::format("Unable to access worker on {}! Is working node running?", host_port);
    res_JSON["status"] = drogon::HttpStatusCode::k504GatewayTimeout;
    return res_JSON;
  }

  if (resp->getStatusCode() >= HttpStatusCode::k400BadRequest) {
    res_JSON["message"] = fmt::format("Unable to disconnect worker {} from cluster!", host_port);
    res_JSON["status"] = resp->getStatusCode();
    res_JSON["node_output"] = *resp->getJsonObject();
    return res_JSON;
  }
  res_JSON = *resp->getJsonObject();

  MoveNode(host_port, ACTIVE, INACTIVE);
  normalized_ -= worker_nodes_[INACTIVE][host_port].node_speed_;

  return res_JSON;
}

const BenchResVec WorkerManagementService::GetSum() const {
  return normalized_;
}

void WorkerManagementService::MoveNode(const std::string &host_port,
                                       const NodeStatus &from, const NodeStatus &to) {
  ComputationalNode aa = worker_nodes_[from][host_port];
  worker_nodes_[from].erase(host_port);
  worker_nodes_[to][host_port] = std::move(aa);
}


std::string ComputationalNode::to_string() {
  return fmt::format("{}:{}",http_client_->getHost(),http_client_->getPort());
}
void ComputationalNode::RecomputeCoefficients(const Json::Value &val) {
  bench_result_ = print_utils::JsonToContinuous<BenchResVec>(val, std::nullopt, true);
  if (node_speed_.size() == 0) {
    node_speed_ = kBenchInfinity / bench_result_;
  }
}
}