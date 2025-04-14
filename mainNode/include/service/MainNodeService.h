#pragma once


#include <drogon/HttpClient.h>
#include <drogon/HttpRequest.h>

#include "network_shared/amqpRestService.h"
#include "network_shared/AMQPPublisherService.h"
#include "common/sharedDeclarations.h"
#include "common/Parsers.h"
#include "common/Printers.h"

namespace main_service
{
using namespace drogon;
using shared::BenchResVec;

/// nodeStatus Enum
enum class NodeStatus {
  /// Node is connected to cluster and is ready to receive it's tasks
  ACTIVE,
  /// Node is present in cluster but is not ready to recieve tasks
  INACTIVE,
  /// An error occurred moving node to a failed state
  FAILED
};

/// Look-up table to cast nodeStatus to string
const std::unordered_map<const NodeStatus, std::string> kNodeStatusToStr
    {
        {NodeStatus::ACTIVE, "Active"},
        {NodeStatus::INACTIVE, "Inactive"},
        {NodeStatus::FAILED, "Failed"},
    };

/**
 * @brief Computational node class
 */
class ComputationalNode {
 public:
  HttpClientPtr http_client_;

  std::string GetPath() {
    return http_client_->getHost() + ":" + std::to_string(http_client_->getPort());
  }
  BenchResVec power_;
  NodeStatus st_;
};


class MainNodeService
{
 public:
  Json::Value Status()
  {
    Json::Value res_JSON;

    res_JSON["request"]="status";

    if(publisher_service_->IsConnected()) {
      res_JSON["rabbitmq_service"]["status"] ="Connected";
      res_JSON["rabbitmq_service"]["c_string"]=publisher_service_->GetConnectionString();
    }
    else
    {
      res_JSON["RabbitmqService"]["status"] ="Not Connected";
    }

    auto& ss=res_JSON["clients"];

    int i = 0;
    for (auto &[str, node] : worker_nodes_) {
      //root["data"][i]=Json::Value();
      ss["data"][i]["host"] = str;
      ss["data"][i]["status"] = kNodeStatusToStr.at(node.st_);
      ss["data"][i]["benchRes"] = print_utils::ContinuousToJson(node.power_);
      i++;
    }

    res_JSON["status"]=drogon::HttpStatusCode::k200OK;
    return res_JSON;

  }
  MainNodeService(const std::string &user,const std::string &password)
  {
    auth_=std::make_unique<JsonAuthHandler>(user,password);
    rest_service_=std::make_unique<amqp_common::RabbitMQRestService>();

    publisher_service_=std::make_unique<amqp_common::AMQPPublisherService>();
  }
  Json::Value Connect(const std::string &qip,
               const std::vector<std::string> &names)
  {

    Json::Value res_JSON;

    res_JSON["request"]="connectQ";

    if(publisher_service_->IsConnected())
    {
      res_JSON["status"]=drogon::HttpStatusCode::k409Conflict;
      //todo use another data to sent
      res_JSON["message"]=fmt::format("Queue service is currently working {}",publisher_service_->GetConnectionString());
      return res_JSON;
    }



    q_host_=qip;

    rest_service_->SetParams(fmt::format("http://{}:15672",q_host_),auth_.get());
    auto r=auth_->Retrive();
    publisher_service_->SetParameters(amqp_common::ConstructCString(q_host_,
                                                                    r.first,
                                                                    r.second),
                                      names);


    try {
      auto exchanges=rest_service_->GetExchanges(vhost_);
      if(std::find_if(exchanges.begin(), exchanges.end(), [this](const amqp_common::exchange&e){
        return e.name==publisher_service_->GetDefaultExchange();
      })==exchanges.end()) {

        auto user=auth_->Retrive().first;
        amqp_common::exchange exchange{publisher_service_->GetDefaultExchange(),
                                       amqp_common::exchange::exchangeData{user,
                                                                           AMQP::direct}};

        rest_service_->CreateExchange(vhost_,exchange,Json::Value());//todo pass more args?
      }
    }
    catch (shared::HttpError&err)
    {
      res_JSON["status"]=drogon::HttpStatusCode::k409Conflict;
      res_JSON["message"]="Error during queue creation";
      res_JSON["exception"]["message"]=err.get<1>();
      res_JSON["exception"]["code"]=err.get<0>();
      return res_JSON;
    }



    publisher_service_->Connect();

    res_JSON["status"]=drogon::HttpStatusCode::k200OK;


    return res_JSON;

  }

  Json::Value ConnectNode(const std::string &host_port,
                   const std::string &name)
  {
    Json::Value res_JSON;

    res_JSON["request"]="connect";

    if(!publisher_service_->IsConnected())
    {
      res_JSON["status"]=drogon::HttpStatusCode::k409Conflict;
      res_JSON["message"]="Queue service is currently unavailable try using connectQ/ request";
      return res_JSON;
    }


    if (!worker_nodes_.count(host_port)) {
      ComputationalNode cn;

      cn.http_client_ = HttpClient::newHttpClient("http://" + host_port);
      cn.st_ = NodeStatus::INACTIVE;
      worker_nodes_[host_port] = std::move(cn);
    }
    else
    {
      if(worker_nodes_[host_port].st_==NodeStatus::ACTIVE)
      {
        res_JSON["status"]=drogon::HttpStatusCode::k409Conflict;
        res_JSON["message"]=fmt::format("Worker node {} is already connected to cluster!",host_port);
        return res_JSON;
      }
    }


    try {
      auto ls=rest_service_->ListQueues(vhost_);
      if(std::find(ls.begin(), ls.end(),name)==ls.end()) {
        //todo create queue object class

        rest_service_->CreateQueue(vhost_, name, Json::Value());
        rest_service_->BindQueueToExchange(vhost_,
                                           name,
                                           publisher_service_->GetDefaultExchange(), name);
      }
    }
    catch (shared::HttpError&err)
    {
      res_JSON["status"]=drogon::HttpStatusCode::k409Conflict;
      res_JSON["message"]="Error during queue creation";
      res_JSON["exception"]["message"]=err.get<1>();
      res_JSON["exception"]["code"]=err.get<0>();
      return res_JSON;
    }

    Json::Value res=auth_->ToJson();
    res["ip"]=q_host_;
    res["name"]=name;
    auto req1 = HttpRequest::newHttpJsonRequest(res);
    req1->setPath("/v1/Connect");
    req1->setMethod(Post);
    auto [code, resp] = worker_nodes_[host_port].http_client_->sendRequest(req1);

    if(code==drogon::ReqResult::BadServerAddress)
    {
      res_JSON["message"]=fmt::format("Unable to access worker on {}! Is working node running?",host_port);
      res_JSON["status"]=drogon::HttpStatusCode::k504GatewayTimeout;
      return res_JSON;
    }
    if (resp->getStatusCode() >= HttpStatusCode::k400BadRequest) {
      res_JSON["message"]=fmt::format("Error occurred during worker {} connection!",host_port);
      res_JSON["status"]=resp->getStatusCode();
      res_JSON["node_output"]=resp->getJsonObject()->toStyledString();
      return res_JSON;
    }

    auto jsoncpp=resp->getJsonObject();
    worker_nodes_[host_port].power_=print_utils::JsonToContinuous<BenchResVec>((*jsoncpp)["bench"]);
    worker_nodes_[host_port].st_ = NodeStatus::ACTIVE;
    res_JSON=*jsoncpp;

    res_JSON["status"]=drogon::HttpStatusCode::k200OK;

    return res_JSON;
  }

  Json::Value DisconnectNode(const std::string &host_port)
  {

    Json::Value res_JSON;

    res_JSON["request"]="disconnect";
    if(!publisher_service_->IsConnected())
    {
      res_JSON["status"]=drogon::HttpStatusCode::k409Conflict;
      res_JSON["message"]="Queue service is currently unavailable try using connectQ/ request";
      return res_JSON;
    }


    if(!worker_nodes_.count(host_port)||worker_nodes_.at(host_port).st_!=NodeStatus::ACTIVE)
    {
      res_JSON["status"]=drogon::HttpStatusCode::k409Conflict;
      res_JSON["message"]=fmt::format("Worker node {} was not connected to cluster!",host_port);
      return res_JSON;
    }
    auto req1 = HttpRequest::newHttpRequest();

    req1->setPath("/v1/Disconnect");
    req1->setMethod(Post);

    auto ct = HttpClient::newHttpClient(host_port);
    auto [code, resp] = worker_nodes_[host_port].http_client_->sendRequest(req1);

    if(code==drogon::ReqResult::BadServerAddress)
    {
      res_JSON["message"]=fmt::format("Unable to access worker on {}! Is working node running?",host_port);
      res_JSON["status"]=drogon::HttpStatusCode::k504GatewayTimeout;
      return res_JSON;
    }

    if (resp->getStatusCode() >= HttpStatusCode::k400BadRequest) {
      res_JSON["message"]=fmt::format("Unable to disconnect worker {} from cluster!",host_port);
      res_JSON["status"]=resp->getStatusCode();
      res_JSON["node_output"]=resp->getJsonObject()->toStyledString();
      return res_JSON;
    }

    worker_nodes_[host_port].st_ = NodeStatus::INACTIVE;

    res_JSON=*resp->getJsonObject();
    res_JSON["status"]=drogon::HttpStatusCode::k200OK;
    return res_JSON;
  }


  Json::Value Disconnect()
  {

    Json::Value res_JSON;
    res_JSON["request"]="disconnectQ";
    if(!publisher_service_->IsConnected())
    {
      res_JSON["status"]=drogon::HttpStatusCode::k409Conflict;
      res_JSON["message"]="Queue service is currently unavailable try using connectQ/ request";
      return res_JSON;
    }

    if(!publisher_service_->IsConnected())
    {
      res_JSON["status"]=drogon::HttpStatusCode::k409Conflict;
      res_JSON["message"]="Queue service is already shut down";
      return res_JSON;
    }

    publisher_service_->Disconnect();


    res_JSON["status"]=drogon::HttpStatusCode::k200OK;
    return res_JSON;
  }
  Json::Value Publish(network_types::TestSolveParam&ts,std::string node)
  {

    Json::Value res_JSON;

    res_JSON["request"]="message";
    if(!publisher_service_->IsConnected())
    {
      res_JSON["status"]=drogon::HttpStatusCode::k409Conflict;
      res_JSON["message"]="Queue service is already shut down";
      return res_JSON;
    }
    auto str=ts.ToJson().toStyledString();

    auto envelope = std::make_shared<AMQP::Envelope>(str);

    envelope->setPersistent(true);
    AMQP::Table headers;
    headers["messageNum"] = ts.experiment_id;
    headers["time"] = std::chrono::steady_clock::now().time_since_epoch().count();
    envelope->setHeaders(headers);

    publisher_service_->Publish(envelope, node);

    res_JSON["status"]=drogon::HttpStatusCode::k200OK;
    return res_JSON;
  }
 private:
  std::unique_ptr<JsonAuthHandler> auth_;
  std::unique_ptr<amqp_common::RabbitMQRestService> rest_service_;
  std::unique_ptr<amqp_common::AMQPPublisherService> publisher_service_;
  std::unordered_map<std::basic_string<char>, ComputationalNode> worker_nodes_;
  std::string q_host_;

  static inline std::string vhost_ = "%2F";
};




}