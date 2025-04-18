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
using namespace rest_utils;
using namespace network_types;

/**
 * @class RabbitMQRestService
 * @brief This service is used to perform http requests to a RabbiMQ
 * management interface
 * @details Since this class initializes curl only one may exist per programm.
 * This class uses libcurl to perform requests.
 * Most of the methods use RabbitMQRestService::PerformRequest() to get the results
 */
class RabbitMQRestService {
 public:

  /**
   *
   */
  RabbitMQRestService();

  /**
   * @brief Sets url's for reuqests
   * @param base_url
   */
  void SetBaseUrl(const std::string &base_url);

  /**
   * @brief Set's url and auth provider
   * @param base_url
   * @param auth_handler
   */
  void SetParams(const std::string &base_url,
                 AuthHandler *auth_handler);

  /**
   * @brief Same as the default constructor but set's the provided parameters.
   * @param base_url
   * @param auth_handler
   * @see RabbitMQRestService::RabbitMQRestService()
   */
  RabbitMQRestService(const std::string &base_url,
                      AuthHandler *auth_handler);


  RabbitMQRestService(const RabbitMQRestService &other) = delete;

  RabbitMQRestService &operator=(const RabbitMQRestService &other) = delete;

  /**
   * @brief Clean up curl
   */
  ~RabbitMQRestService();

  /**
   * @brief Requests user information from a RabbitMQ.
   * @return Json::Value from RabbitMQRestService::PerformRequest
   */
  Json::Value Whoami();

  /**
   * @brief Creates queue with specified params
   * @param vhost
   * @param queue
   * @param arguments - additional arguments to be added in body["arguments"]
   * @return true - if succeeds
   */
  bool CreateQueue(const std::string &vhost,
                   const network_types::queue &queue,
                   const Json::Value &arguments);

  /**
   * @brief Remove queue
   * @param vhost
   * @param queue_name
   * @return true - if succeeds
   */
  bool DeleteQueue(const std::string &vhost,
                   const std::string &queue_name);

  /**
   * @brief Creates exchange with specified params
   * @param vhost
   * @param exchange
   * @param arguments - additional arguments to be added in body["arguments"]
   * @return true - if succeeds
   */
  bool CreateExchange(const std::string &vhost,
                      const exchange &exchange,
                      const Json::Value &arguments);
  /**
   * @brief Remove exchange
   * @param vhost
   * @param exchange_name
   * @return true - if succeeds
   */
  bool DeleteExchange(const std::string &vhost,
                      const std::string &exchange_name);

  /**
   * @brief Retrieves Json::Value from queue/stats
   * @param vhost
   * @param queue_name
   */
  Json::Value GetQueueStats(const std::string &vhost,
                            const std::string &queue_name);

  /**
   * @brief Lists all queues
   * @param vhost
   * @return std::vector of queue names
   */
  std::vector<std::string> ListQueues(const std::string &vhost);

  /**
   * @brief Binds queue to exchange
   * @param vhost
   * @param queue_name
   * @param exchange_name
   * @param routing_key
   * @return true - if succeeds
   */
  bool BindQueueToExchange(const std::string &vhost,
                           const std::string &queue_name,
                           const std::string &exchange_name,
                           const std::string &routing_key);

  /**
   * Unbinds queue from  exchange
   * @param vhost
   * @param queue_name
   * @param exchange_name
   * @param routing_key
   * @return true - if succeeds
   */
  bool UnbindQueueFromExchange(const std::string &vhost,
                               const std::string &queue_name,
                               const std::string &exchange_name,
                               const std::string &routing_key);

  /**
   * @brief Creates new auth user data
   * @param user
   * @param pass
   * @return true - if succeeds
   */
  bool CreateUser(const std::string &user,
                  const std::string &pass);

  /**
   * @brief Deletes user data
   * @param user
   * @return true - if succeeds
   */
  bool DeleteUser(const std::string &user);

  /**
   * @brief List all users with their roles and tags
   * @param vhost
   * @return  std::vector<rabbitMQUser>
   */
  std::vector<rabbitMQUser> ListUsers(const std::string &vhost);

  /**
   * @brief Lists all queue bindings for the specified queue
   * @param vhost
   * @param queue
   * @return std::vector<queueBinding>
   */
  std::vector<queueBinding> GetQueueBindings(const std::string &vhost, const std::string &queue);

  /**
   * @brief Retrieves list of exhnages
   * @param vhost
   * @return std::vector<exchange>
   */
  std::vector<exchange> GetExchanges(const std::string &vhost);

 private:
  std::string base_url_; ///< url to perform request

  AuthHandler *auth_ptr_;

  /**
   * @brief Wrapper to perform curl request
   * @brief Wraps around rest_utils::PerformCurlRequest()
   * Thrown anything this function throws.
   * @param path
   * @param method
   * @param data
   * @return resulting body
   */
  std::string PerformRequest(const std::string &path,
                             const std::string &method,
                             const std::string &data = "");

  static Json::Value ParseJson(const std::string &json_str);
};
}