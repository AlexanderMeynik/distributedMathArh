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
  //todo call all of nodes;

  callback(HttpResponse::newHttpJsonResponse(root));

}

void ClusterConfigController::ConnectHandler(const HttpRequestPtr &req,
                                             std::function<void(const HttpResponsePtr &)> &&callback,
                                             const std::string &host_port, const std::string &qip,
                                             const std::string &name) {

  Json::Value res;



  res=main_node_service_->ConnectNode(host_port,name);

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
