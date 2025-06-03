#include "service/MainNodeService.h"
#include "network_shared/sharedConstants.h"

/// Namespace for services used in main node
namespace main_services {

MainNodeService::MainNodeService(const std::string &user, const std::string &password) {
  auth_ = std::make_unique<JsonAuthHandler>(user, password);
  rest_service_ = std::make_unique<amqp_common::RabbitMQRestService>();

  publisher_service_ = std::make_unique<amqp_common::AMQPPublisherService>();
  worker_management_service_ = std::make_unique<WorkerManagementService>();
}

Json::Value MainNodeService::Status() {
  Json::Value res_JSON;

  res_JSON["request"] = "status";

  if (publisher_service_->IsConnected()) {
    res_JSON["rabbitmq_service"]["status"] = "Connected";
    res_JSON["rabbitmq_service"]["c_string"] = publisher_service_->GetCString();
  } else {
    res_JSON["RabbitmqService"]["status"] = "Not Connected";
  }

  res_JSON["clients"] = worker_management_service_->GetStatus();

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

  auto js1 = worker_management_service_->AddNewNode(host_port);
  if (js1.isMember("status")) {
    res_JSON["status"] = js1["status"];
    js1.removeMember("status");
    res_JSON["balancer_output"] = js1;
    return res_JSON;
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

  js1 = worker_management_service_->ConnectNode(host_port, req1);

  res_JSON["balancer_output"] = js1;
  if (js1.isMember("status")) {
    res_JSON["status"] = js1["status"];
    res_JSON["balancer_output"].removeMember("status");

    return res_JSON;
  }

  res_JSON["status"] = drogon::HttpStatusCode::k200OK;

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

  auto js1 = worker_management_service_->DisconnectNode(host_port);

  res_JSON["balancer_output"] = js1;
  if (js1.isMember("status")) {
    res_JSON["status"] = js1["status"];
    res_JSON["balancer_output"].removeMember("status");
    return res_JSON;
  }

  res_JSON["status"] = drogon::HttpStatusCode::k200OK;
  return res_JSON;
}
Json::Value MainNodeService::Connect(const std::string &qip) {

  Json::Value res_JSON;

  res_JSON["request"] = "connect_publisher";

  if (publisher_service_->IsConnected()) {
    res_JSON["status"] = drogon::HttpStatusCode::k409Conflict;

    res_JSON["message"] =
        fmt::format("Queue service is currently working {}", publisher_service_->GetCString());
    return res_JSON;
  }

  q_host_ = qip;

  rest_service_->SetParams(fmt::format("http://{}:15672", q_host_), auth_.get());
  auto r = auth_->Retrive();
  publisher_service_->SetParameters(amqp_common::ConstructCString(q_host_,
                                                                  r.first,
                                                                  r.second));

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
  catch (CurlError &err) {
    res_JSON["status"] = drogon::HttpStatusCode::k409Conflict;
    res_JSON["message"] = "Error during http curl request";
    res_JSON["exception"]["message"] = err.what();
    return res_JSON;
  }

  try {
    publisher_service_->Connect();
  }
  catch (std::runtime_error &err) {
    res_JSON["status"] = HttpStatusCode::k409Conflict;
    res_JSON["message"] = fmt::format("Publisher connection error, msg \"{}\"!", err.what());
    return res_JSON;
  }

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


Json::Value MainNodeService::PublishMessage(network_types::TestSolveParam &ts, std::string node) {
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

Json::Value MainNodeService::SendToExecution(network_types::TestSolveParam &ts) {
  Json::Value res_JSON;

  res_JSON["request"] = "send_task";
  if (worker_management_service_->begin() == worker_management_service_->end()) {
    res_JSON["message"] = "No nodes to compute task";
    res_JSON["status"] = drogon::HttpStatusCode::k409Conflict;
    return res_JSON;
  }

  auto itercount = ts.range.second - ts.range.first + 1;
  network_types::TestSolveParam ts_t;

  auto  n=ts.N_;

  auto it = worker_management_service_->begin();

  auto it2 = worker_management_service_->begin();
  it2++;

  for (; it2 != worker_management_service_->end(); it2++, it++) {
    auto &[key, val] = *it;

    BenchResVec iters = val.node_speed_ * (itercount);
    iters /= worker_management_service_->GetSum();

    auto iters_to_slice = iters[shared::NIndex(n)];

    ts_t = ts.SliceAway(iters_to_slice);
    PublishMessage(ts_t, key);
  }

  PublishMessage(ts, it->first);

  res_JSON["status"] = drogon::HttpStatusCode::k200OK;

  return res_JSON;
}

}


