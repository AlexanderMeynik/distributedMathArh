#include "controller/ClusterConfigController.h"
#include "common/Parsers.h"
#include "common/Printers.h"

using namespace rest::v1;

void
ClusterConfigController::GetStatus(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {

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


  callback(HttpResponse::newHttpJsonResponse(root));

}

void ClusterConfigController::ConnectHandler(const HttpRequestPtr &req,
                                             std::function<void(const HttpResponsePtr &)> &&callback,
                                             const std::string &host_port, const std::string &qip,
                                             const std::string &name) {

  Json::Value res;



  res=main_node_service_->ConnectNode(host_port,host_port);

  auto r = HttpResponse::newHttpJsonResponse(res);
  r->setStatusCode(static_cast<HttpStatusCode>(res["status"].asUInt()));

  callback(r);

}

void ClusterConfigController::DisconnectHandler(const HttpRequestPtr &req,
                                                std::function<void(const HttpResponsePtr &)> &&callback,
                                                const std::string &host_port) {

  Json::Value res;



  res=main_node_service_->DisconnectNode(host_port);

  auto r = HttpResponse::newHttpJsonResponse(res);
  r->setStatusCode(static_cast<HttpStatusCode>(res["status"].asUInt()));

  callback(r);

}
void ClusterConfigController::SentMessage(const HttpRequestPtr &req,
                                          std::function<void(const HttpResponsePtr &)> &&callback,
                                          const std::string &node) {

  network_types::TestSolveParam ts(*req->getJsonObject());
  main_node_service_->Publish(ts,node);


  auto r = HttpResponse::newHttpJsonResponse("");
  r->setStatusCode(drogon::HttpStatusCode::k200OK);
  callback(r);

}
void ClusterConfigController::ConnectQ(const HttpRequestPtr &req,
                                       std::function<void(const HttpResponsePtr &)> &&callback) {

  auto json=*req->getJsonObject();

  auto qname=json["queue_host"].asString();
  auto szs=json["queues"].size();
  auto queus=print_utils::JsonToContinuous<std::vector<std::string>>(
      json["queues"],szs);


  auto jss=main_node_service_->Connect(qname,queus);

  auto r = HttpResponse::newHttpJsonResponse(jss);
  r->setStatusCode(drogon::HttpStatusCode::k200OK);
  callback(r);
}
void ClusterConfigController::DisconnectQ(const HttpRequestPtr &req,
                                          std::function<void(const HttpResponsePtr &)> &&callback) {

  main_node_service_->Disconnect();


  auto r = HttpResponse::newHttpJsonResponse("");
  r->setStatusCode(drogon::HttpStatusCode::k200OK);
  callback(r);

}
