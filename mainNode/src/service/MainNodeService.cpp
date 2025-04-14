#include "service/MainNodeService.h"
namespace main_service {
Json::Value MainNodeService::Status() {
  Json::Value res_JSON;

  res_JSON["request"] = "status";

  if (publisher_service_->IsConnected()) {
    res_JSON["rabbitmq_service"]["status"] = "Connected";
    res_JSON["rabbitmq_service"]["c_string"] = publisher_service_->GetConnectionString();
  } else {
    res_JSON["RabbitmqService"]["status"] = "Not Connected";
  }

  auto &ss = res_JSON["clients"];

  int i = 0;
  for (auto &[str, node] : worker_nodes_) {
    //root["data"][i]=Json::Value();
    ss["data"][i]["host"] = str;
    ss["data"][i]["status"] = kNodeStatusToStr.at(node.st_);
    ss["data"][i]["benchRes"] = print_utils::ContinuousToJson(node.power_);
    i++;
  }

  res_JSON["status"] = drogon::HttpStatusCode::k200OK;
  return res_JSON;

}
Json::Value MainNodeService::ConnectNode(const std::string &host_port, const std::string &name) {
  Json::Value res_JSON;

  res_JSON["request"] = "connect_node";

  if (!publisher_service_->IsConnected()) {
    res_JSON["status"] = drogon::HttpStatusCode::k409Conflict;
    res_JSON["message"] = "Queue service is currently unavailable try using connect_publisher/ request";
    return res_JSON;
  }

  if (!worker_nodes_.count(host_port)) {
    ComputationalNode cn;

    cn.http_client_ = HttpClient::newHttpClient("http://" + host_port);
    cn.st_ = NodeStatus::INACTIVE;
    worker_nodes_[host_port] = std::move(cn);
  } else {
    if (worker_nodes_[host_port].st_ == NodeStatus::ACTIVE) {
      res_JSON["status"] = drogon::HttpStatusCode::k409Conflict;
      res_JSON["message"] = fmt::format("Worker node {} is already connected to cluster!", host_port);
      return res_JSON;
    }
  }

  try {
    auto ls = rest_service_->ListQueues(vhost_);
    if (std::find(ls.begin(), ls.end(), name) == ls.end()) {
      auto queue = network_types::queue(name, auth_->Retrive().first);
      rest_service_->CreateQueue(vhost_, queue, Json::Value());
      rest_service_->BindQueueToExchange(vhost_, name, publisher_service_->GetDefaultExchange(), name);
    }
  }
  catch (shared::HttpError &err) {
    res_JSON["status"] = drogon::HttpStatusCode::k409Conflict;
    res_JSON["message"] = "Error during queue creation";
    res_JSON["exception"]["message"] = err.get<1>();
    res_JSON["exception"]["code"] = err.get<0>();
    return res_JSON;
  }

  Json::Value res = auth_->ToJson();
  res["ip"] = q_host_;
  res["name"] = name;
  auto req1 = HttpRequest::newHttpJsonRequest(res);
  req1->setPath("/v1/Connect");
  req1->setMethod(Post);
  auto [code, resp] = worker_nodes_[host_port].http_client_->sendRequest(req1);

  if (code == drogon::ReqResult::BadServerAddress) {
    res_JSON["message"] = fmt::format("Unable to access worker on {}! Is working node running?", host_port);
    res_JSON["status"] = drogon::HttpStatusCode::k504GatewayTimeout;
    return res_JSON;
  }
  if (resp->getStatusCode() >= HttpStatusCode::k400BadRequest) {
    res_JSON["message"] = fmt::format("Error occurred during worker {} connection!", host_port);
    res_JSON["status"] = resp->getStatusCode();
    res_JSON["node_output"] = resp->getJsonObject()->toStyledString();
    return res_JSON;
  }

  auto jsoncpp = resp->getJsonObject();
  worker_nodes_[host_port].power_ = print_utils::JsonToContinuous<BenchResVec>((*jsoncpp)["bench"]);
  worker_nodes_[host_port].st_ = NodeStatus::ACTIVE;
  res_JSON = *jsoncpp;

  res_JSON["status"] = drogon::HttpStatusCode::k200OK;

  RecomputeWeights();

  return res_JSON;
}
Json::Value MainNodeService::DisconnectNode(const std::string &host_port) {
  Json::Value res_JSON;

  res_JSON["request"] = "disconnect_node";
  if (!publisher_service_->IsConnected()) {
    res_JSON["status"] = drogon::HttpStatusCode::k409Conflict;
    res_JSON["message"] = "Queue service is currently unavailable try using connect_publisher/ request";
    return res_JSON;
  }

  if (!worker_nodes_.count(host_port) || worker_nodes_.at(host_port).st_ != NodeStatus::ACTIVE) {
    res_JSON["status"] = drogon::HttpStatusCode::k409Conflict;
    res_JSON["message"] = fmt::format("Worker node {} was not connected to cluster!", host_port);
    return res_JSON;
  }
  auto req1 = HttpRequest::newHttpRequest();

  req1->setPath("/v1/Disconnect");
  req1->setMethod(Post);

  auto ct = HttpClient::newHttpClient(host_port);
  auto [code, resp] = worker_nodes_[host_port].http_client_->sendRequest(req1);

  if (code == drogon::ReqResult::BadServerAddress) {
    res_JSON["message"] = fmt::format("Unable to access worker on {}! Is working node running?", host_port);
    res_JSON["status"] = drogon::HttpStatusCode::k504GatewayTimeout;
    return res_JSON;
  }

  if (resp->getStatusCode() >= HttpStatusCode::k400BadRequest) {
    res_JSON["message"] = fmt::format("Unable to disconnect worker {} from cluster!", host_port);
    res_JSON["status"] = resp->getStatusCode();
    res_JSON["node_output"] = resp->getJsonObject()->toStyledString();
    return res_JSON;
  }

  worker_nodes_[host_port].st_ = NodeStatus::INACTIVE;

  res_JSON = *resp->getJsonObject();
  RecomputeWeights();
  res_JSON["status"] = drogon::HttpStatusCode::k200OK;
  return res_JSON;
}
Json::Value MainNodeService::Connect(const std::string &qip, const std::vector<std::string> &names) {

  Json::Value res_JSON;

  res_JSON["request"] = "connect_publisher";

  if (publisher_service_->IsConnected()) {
    res_JSON["status"] = drogon::HttpStatusCode::k409Conflict;

    res_JSON["message"] =
        fmt::format("Queue service is currently working {}", publisher_service_->GetConnectionString());
    return res_JSON;
  }

  q_host_ = qip;

  rest_service_->SetParams(fmt::format("http://{}:15672", q_host_), auth_.get());
  auto r = auth_->Retrive();
  publisher_service_->SetParameters(amqp_common::ConstructCString(q_host_,
                                                                  r.first,
                                                                  r.second),
                                    names);

  try {
    auto exchanges = rest_service_->GetExchanges(vhost_);
    if (std::find_if(exchanges.begin(), exchanges.end(), [this](const amqp_common::exchange &e) {
      return e.name == publisher_service_->GetDefaultExchange();
    }) == exchanges.end()) {

      auto user = auth_->Retrive().first;
      amqp_common::exchange exchange{publisher_service_->GetDefaultExchange(), user, AMQP::direct};

      rest_service_->CreateExchange(vhost_, exchange, Json::Value());//todo pass more args?
    }
  }
  catch (shared::HttpError &err) {
    res_JSON["status"] = drogon::HttpStatusCode::k409Conflict;
    res_JSON["message"] = "Error during queue creation";
    res_JSON["exception"]["message"] = err.get<1>();
    res_JSON["exception"]["code"] = err.get<0>();
    return res_JSON;
  }

  publisher_service_->Connect();

  res_JSON["status"] = drogon::HttpStatusCode::k200OK;

  return res_JSON;
}
Json::Value MainNodeService::Disconnect() {
  Json::Value res_JSON;
  res_JSON["request"] = "disconnect_publisher";
  if (!publisher_service_->IsConnected()) {
    res_JSON["status"] = drogon::HttpStatusCode::k409Conflict;
    res_JSON["message"] = "Queue service is currently unavailable try using connect_publisher/ request";
    return res_JSON;
  }

  if (!publisher_service_->IsConnected()) {
    res_JSON["status"] = drogon::HttpStatusCode::k409Conflict;
    res_JSON["message"] = "Queue service is already shut down";
    return res_JSON;
  }

  publisher_service_->Disconnect();

  res_JSON["status"] = drogon::HttpStatusCode::k200OK;
  return res_JSON;
}
Json::Value MainNodeService::Publish(network_types::TestSolveParam &ts, std::string node) {
  Json::Value res_JSON;

  res_JSON["request"] = "message";
  if (!publisher_service_->IsConnected()) {
    res_JSON["status"] = drogon::HttpStatusCode::k409Conflict;
    res_JSON["message"] = "Queue service is already shut down";
    return res_JSON;
  }
  auto str = ts.ToJson().toStyledString();

  auto envelope = std::make_shared<AMQP::Envelope>(str);

  envelope->setPersistent(true);
  AMQP::Table headers;
  headers["messageNum"] = ts.experiment_id;
  headers["time"] = std::chrono::steady_clock::now().time_since_epoch().count();
  envelope->setHeaders(headers);

  publisher_service_->Publish(envelope, node);

  res_JSON["status"] = drogon::HttpStatusCode::k200OK;
  return res_JSON;
}


Json::Value MainNodeService::Publish2(network_types::TestSolveParam &ts, std::string node) {
  Json::Value res_JSON;

  res_JSON["request"] = "message";
  if (!publisher_service_->IsConnected()) {
    res_JSON["status"] = drogon::HttpStatusCode::k409Conflict;
    res_JSON["message"] = "Queue service is already shut down";
    return res_JSON;
  }
  auto str = ts.ToJson().toStyledString();

  auto envelope = std::make_shared<AMQP::Envelope>(str);

  envelope->setPersistent(true);
  AMQP::Table headers;
  headers["messageNum"] = ts.experiment_id;
  headers["time"] = std::chrono::steady_clock::now().time_since_epoch().count();
  envelope->setHeaders(headers);

  publisher_service_->Publish(envelope, node);

  res_JSON["status"] = drogon::HttpStatusCode::k200OK;
  return res_JSON;
}
void MainNodeService::RecomputeWeights() {
  if(worker_nodes_.empty())
  {
    return;
  }
  auto max=std::numeric_limits<print_utils::BenchResultType>::max();

  normalized_.resize(worker_nodes_.begin()->second.power_.size());
  normalized_=0;
  for (auto &[key,val]:worker_nodes_) {
    if(val.st_==NodeStatus::ACTIVE) {
      val.w_ = max / val.power_;
      normalized_ += val.w_;
    }
  }


}
Json::Value MainNodeService::SendToExecution(network_types::TestSolveParam &ts) {
  Json::Value res_JSON;

  res_JSON["request"] = "send_task";
  //todo test later
  const std::string* kk= nullptr;
  for (auto &[key,val]:worker_nodes_) {
    if (val.st_ == NodeStatus::ACTIVE) {
      kk=&key;
      //todo use proper logick to find weight
      print_utils::BenchResultType iters=val.w_[2]*(ts.range.second-ts.range.first+1);
      iters/=normalized_[2];//todo integer undeflow
      if(ts.Slice(iters))//zero itearations(?)
      {
        Publish2(ts,key);
      }
    }
  }

  if(kk!= nullptr) {
    Publish2(ts, *kk);
  }
  else
  {
    res_JSON["message"]="No nodes to compute task";
    res_JSON["status"]=drogon::HttpStatusCode::k409Conflict;
  }

  return res_JSON;
}

}


