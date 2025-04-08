#include "controller/ClusterConfigController.h"
#include "common/Parsers.h"
#include "common/Printers.h"

using namespace rest::v1;

void
ClusterConfigController::GetStatus(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
  /*auto r=HttpResponse::newHttpResponse();*/

  Json::Value root;

  int i = 0;
  root["size"] = clients_.size();
  for (auto &[str, node] : clients_) {
    //root["data"][i]=Json::Value();
    root["data"][i]["host"] = str;
    root["data"][i]["status"] = kNodeStatusToStr.at(node.st_);
    root["data"][i]["benchRes"] = print_utils::ContinuousToJson(node.power_);
    i++;
  }
  //todo call all of nodes;

  callback(HttpResponse::newHttpJsonResponse(root));

}

void ClusterConfigController::ConnectHandler(const HttpRequestPtr &req,
                                             std::function<void(const HttpResponsePtr &)> &&callback,
                                             const std::string &host_port, const std::string &qip,
                                             const std::string &name) {

  Json::Value res;

  if (!clients_.count(host_port)) {
    ComputationalNode cn;
    //todo ad https as option
    cn.http_client_ = HttpClient::newHttpClient("http://" + host_port);
    cn.st_ = NodeStatus::INACTIVE;
    clients_[host_port] = std::move(cn);
  }

  auto req1 = HttpRequest::newHttpRequest();
  req1->setPath("/v1/Connect");
  req1->setParameter("ip", qip);
  req1->setParameter("name", name);
  req1->setMethod(Post);

  auto [code, resp] = clients_[host_port].http_client_->sendRequest(req1);
  auto json_ptr = resp->jsonObject();

  res["ip"] = host_port;
  res["qip"] = qip;
  res["qname"] = name;
  if (!resp) {

    res["message"] = "Unable to Connect to node";
    auto r = HttpResponse::newHttpJsonResponse(res);
    callback(r);
    return;//todo maybe some guard like class to handle this
  }
  if (resp->getStatusCode() != HttpStatusCode::k200OK) {
    res["code"] = resp->getStatusCode();
    auto r = HttpResponse::newHttpJsonResponse(res);
    callback(r);
    return;
  }

  clients_[host_port].power_ = print_utils::JsonToContinuous<std::valarray<double>>((*json_ptr)["bench"]);

  clients_[host_port].st_ = NodeStatus::ACTIVE;
  res["benchRes"] = print_utils::ContinuousToJson(clients_[host_port].power_);

  auto r = HttpResponse::newHttpJsonResponse(res);

  callback(r);

}

void ClusterConfigController::DisconnectHandler(const HttpRequestPtr &req,
                                                std::function<void(const HttpResponsePtr &)> &&callback,
                                                const std::string &host_port) {
  Json::Value res;

  auto req1 = HttpRequest::newHttpRequest();

  req1->setPath("/v1/Disconnect");
  req1->setMethod(Post);

  auto ct = HttpClient::newHttpClient(host_port);
  auto [code, resp] = clients_[host_port].http_client_->sendRequest(req1);

  if (resp->getStatusCode() != HttpStatusCode::k200OK) {
    if (!resp->body().empty())//todo use this for conenct if body is empty
    {
      res = *resp->getJsonObject();
    }

    res["ip"] = host_port;
    res["code"] = resp->getStatusCode();

    auto r = HttpResponse::newHttpJsonResponse(res);
    callback(r);
    return;
  }

  clients_[host_port].st_ = NodeStatus::INACTIVE;
  //todo what to do with http client;

  res = *resp->getJsonObject();
  res["ip"] = host_port;
  res["benchRes"] = print_utils::ContinuousToJson(clients_[host_port].power_);

  auto r = HttpResponse::newHttpJsonResponse(res);

  callback(r);

}
