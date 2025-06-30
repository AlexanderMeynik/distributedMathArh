#include "network_shared/amqpRestService.h"
#include <sstream>

///amqpCommon namespace
namespace amqp_common {

RabbitMQRestService::RabbitMQRestService() {
  curl_global_init(CURL_GLOBAL_DEFAULT);
}
RabbitMQRestService::RabbitMQRestService(const std::string &base_url,
                                         AuthHandler *auth_handler)
    : base_url_(base_url),
      auth_ptr_(auth_handler) {
  curl_global_init(CURL_GLOBAL_DEFAULT);
}

void RabbitMQRestService::SetBaseUrl(const std::string &base_url) {
  base_url_ = base_url;
}

void RabbitMQRestService::SetParams(const std::string &base_url, AuthHandler *auth_handler) {
  base_url_ = base_url;
  auth_ptr_ = auth_handler;
}

RabbitMQRestService::~RabbitMQRestService() {
  curl_global_cleanup();
}

HttpResult RabbitMQRestService::PerformRequest(const std::string &path,
                                                const std::string &method,
                                                const std::string &data) {
  return PerformCurlRequest(path, method, base_url_, auth_ptr_, data);
}

HttpResult RabbitMQRestService::PerformRequest(const std::string &path,
                                               const HttpMethod &method,
                                               const std::string &data) {
  return PerformRequest(path,ToString(method),data);
}


Json::Value RabbitMQRestService::ParseJson(const std::string &json_str) {
  Json::Value root;
  Json::CharReaderBuilder builder;
  std::string errs;
  std::istringstream iss(json_str);
  if (!Json::parseFromStream(builder, iss, &root, &errs)) {
    throw std::runtime_error("JSON parse error: " + errs);
  }
  return root;
}

bool RabbitMQRestService::CreateQueue(const std::string &vhost,
                                      const network_types::queue &queue,
                                      const Json::Value &arguments) {
  std::string path = fmt::format("/api/queues/{}/{}", vhost, queue.name);

  Json::Value body = queue.ToJson();
  if (!arguments.empty()) {
    body["arguments"] = arguments;
  }
  std::string data = Json::writeString(Json::StreamWriterBuilder(), body);
  PerformRequest(path, HttpMethod::PUT, data);
  return true;
}

bool RabbitMQRestService::DeleteQueue(const std::string &vhost,
                                      const std::string &queue_name) {
  std::string path = fmt::format("/api/queues/{}/{}", vhost, queue_name);
  PerformRequest(path, HttpMethod::DELETE);
  return true;
}

bool RabbitMQRestService::CreateExchange(const std::string &vhost, const exchange &exchange,
                                         const Json::Value &arguments) {
  std::string path = fmt::format("/api/exchanges/{}/{}", vhost, exchange.name);
  Json::Value body = exchange.ToJson();
  std::string data = Json::writeString(Json::StreamWriterBuilder(), body);
  PerformRequest(path, HttpMethod::PUT, data);
  return true;
}

bool RabbitMQRestService::DeleteExchange(const std::string &vhost, const std::string &exchange_name) {
  std::string path = fmt::format("/api/exchanges/{}/{}", vhost, exchange_name);
  PerformRequest(path, HttpMethod::DELETE);
  return true;
}

Json::Value RabbitMQRestService::GetQueueStats(const std::string &vhost,
                                               const std::string &queue_name) {

  std::string path = fmt::format("/api/queues/{}/{}", vhost, queue_name);
  std::string response = PerformRequest(path, HttpMethod::GET).second;
  return ParseJson(response);
}

std::vector<std::string> RabbitMQRestService::ListQueues(const std::string &vhost) {
  std::string path = "/api/queues/" + vhost;
  std::string response = PerformRequest(path, HttpMethod::GET).second;
  Json::Value j = ParseJson(response);
  std::vector<std::string> queues;
  for (const auto &item : j) {
    queues.push_back(item["name"].asString());
  }
  return queues;
}

bool RabbitMQRestService::BindQueueToExchange(const std::string &vhost,
                                              const std::string &queue_name,
                                              const std::string &exchange_name,
                                              const std::string &routing_key) {
  std::string path = "/api/bindings/" + vhost + "/e/" + exchange_name + "/q/" + queue_name;
  Json::Value body;
  body["routing_key"] = routing_key;
  std::string data = Json::writeString(Json::StreamWriterBuilder(), body);
  PerformRequest(path, HttpMethod::POST, data);
  return true;
}

bool RabbitMQRestService::UnbindQueueFromExchange(const std::string &vhost,
                                                  const std::string &queue_name,
                                                  const std::string &exchange_name,
                                                  const std::string &routing_key) {
  std::string path = "/api/bindings/" + vhost + "/e/" + exchange_name + "/q/" + queue_name + "/" + routing_key;
  PerformRequest(path, HttpMethod::DELETE);
  return true;
}

bool RabbitMQRestService::CreateUser(const std::string &user,
                                     const std::string &pass) {
  std::string path = "/api/users/" + user;
  Json::Value body;
  body["password"] = pass;
  body["tags"] = "worker";
  std::string data = Json::writeString(Json::StreamWriterBuilder(), body);
  PerformRequest(path, HttpMethod::PUT, data);
  return true;
}

bool RabbitMQRestService::PublishMessage(const std::string &vhost,
                                         const std::string &exhange_name,
                                         const message &message) {
  std::string path = fmt::format("/api/exchanges/{}/{}/publish", vhost, exhange_name);

  Json::Value body = message.ToJson();
  std::string data = Json::writeString(Json::StreamWriterBuilder(), body);
  auto rr = ParseJson(PerformRequest(path, HttpMethod::POST, data).second);

  return rr["routed"].asBool();
}

size_t RabbitMQRestService::GetMessageCount(const std::string &vhost, const std::string &queue_name) {
  std::string path = fmt::format("/api/queues/{}/{}/get", vhost, queue_name);

  Json::Value body;
  body["count"] = 1000;
  body["ackmode"] = "ack_requeue_true";///@todoif we need more modes create or find enum
  body["encoding"] = "auto";
  body["truncate"] = 50000;
  std::string data = Json::writeString(Json::StreamWriterBuilder(), body);
  auto rr = ParseJson(PerformRequest(path, HttpMethod::POST, data).second);

  return rr.size();
}

bool RabbitMQRestService::DeleteUser(const std::string &user) {
  std::string path = "/api/users/" + user;
  PerformRequest(path, HttpMethod::DELETE);
  return true;
}

std::vector<rabbitMQUser> RabbitMQRestService::ListUsers(const std::string &vhost) {
  std::string path = "/api/users/";
  auto res = ParseJson(PerformRequest(path, HttpMethod::GET).second);
  return ParseArray<std::vector,rabbitMQUser>(res);
}

std::vector<connection> RabbitMQRestService::ListConnections() {
  std::string path = "/api/connections";
  auto res = ParseJson(PerformRequest(path, HttpMethod::GET).second);
  return ParseArray<std::vector,connection>(res);
}

std::vector<channel> RabbitMQRestService::ListChannels() {
  std::string path = "/api/channels";
  auto res = ParseJson(PerformRequest(path, HttpMethod::GET).second);
  return ParseArray<std::vector,channel>(res);
}

Json::Value RabbitMQRestService::Whoami() {

  std::string path = "/api/whoami";

  std::string response = PerformRequest(path, HttpMethod::GET).second;

  return ParseJson(response);
}

std::vector<queueBinding>
RabbitMQRestService::GetQueueBindings(const std::string &vhost, const std::string &queue) {
  std::string path = fmt::format("/api/queues/{}/{}/bindings", vhost, queue);
  auto res = ParseJson(PerformRequest(path, HttpMethod::GET).second);
  return ParseArray<std::vector,queueBinding>(res);
}

std::vector<global_param> RabbitMQRestService::ListGlobalParams() {
  std::string path = "/api/global-parameters";
  auto res = ParseJson(PerformRequest(path, HttpMethod::GET).second);
  return ParseArray<std::vector,global_param>(res);
}
void RabbitMQRestService::GlobalParam(const global_param &param, const HttpMethod &type) {

  std::string path = fmt::format("/api/global-parameters/{}", param.name);

  if (type == HttpMethod::PUT) {
    std::string data = Json::writeString(Json::StreamWriterBuilder(), param.ToJson());
    PerformRequest(path, type, data);
  } else {
    PerformRequest(path, type);
  }
}

std::vector<exchange> RabbitMQRestService::GetExchanges(const std::string &vhost) {
  std::string path = fmt::format("/api/exchanges/{}", vhost);
  auto res = ParseJson(PerformRequest(path, HttpMethod::GET).second);
  return ParseArray<std::vector,exchange>(res);
}


}