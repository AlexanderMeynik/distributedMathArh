#pragma once

#include <vector>
#include <string>
#include <unordered_map>

#include <json/json.h>
#include <amqpcpp/exchangetype.h>
#include "restUtils.h"
#include "network_shared/networkTypes.h"
///amqpCommon namespace
namespace amqp_common {

using namespace network_types;

class RabbitMQRestService {
 public:
  RabbitMQRestService();

  void SetBaseUrl(const std::string &base_url);

  void SetParams(const std::string &base_url,
                 AuthHandler *auth_handler);

  RabbitMQRestService(const std::string &base_url,
                      AuthHandler *auth_handler);

  RabbitMQRestService(const RabbitMQRestService &other) = delete;

  RabbitMQRestService &operator=(const RabbitMQRestService &other) = delete;
  ~RabbitMQRestService();

  Json::Value Whoami();

  bool CreateQueue(const std::string &vhost,
                   const network_types::queue &queue,
                   const Json::Value &arguments);

  bool DeleteQueue(const std::string &vhost,
                   const std::string &queue_name);

  bool CreateExchange(const std::string &vhost,
                      const exchange &exchange,
                      const Json::Value &arguments);

  bool DeleteExchange(const std::string &vhost,
                      const std::string &exchange_name);

  Json::Value GetQueueStats(const std::string &vhost,
                            const std::string &queue_name);

  std::vector<std::string> ListQueues(const std::string &vhost);

  bool BindQueueToExchange(const std::string &vhost,
                           const std::string &queue_name,
                           const std::string &exchange_name,
                           const std::string &routing_key);

  bool UnbindQueueFromExchange(const std::string &vhost,
                               const std::string &queue_name,
                               const std::string &exchange_name,
                               const std::string &routing_key);

  bool CreateUser(const std::string &user,
                  const std::string &pass);

  bool DeleteUser(const std::string &user);

  std::vector<rabbitMQUser> ListUsers(const std::string &vhost);

  std::vector<queueBinding> GetQueueBindings(const std::string &vhost, const std::string &queue);

  std::vector<exchange> GetExchanges(const std::string &vhost);

 private:
  std::string base_url_;
  std::string username;
  std::string password;

  AuthHandler *auth_ptr_;

  std::string PerformRequest(const std::string &path,
                             const std::string &method,
                             const std::string &data = "");

  static Json::Value ParseJson(const std::string &json_str);
};
}