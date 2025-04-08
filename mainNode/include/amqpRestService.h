#pragma once

#include <vector>
#include <string>
#include <unordered_map>

#include <json/json.h>
#include <amqpcpp/exchangetype.h>
#include "restUtils.h"

///amqpCommon namespace
namespace amqp_common {

/// std::string to AMQP::ExchangeType
static inline std::unordered_map<std::string, AMQP::ExchangeType> stringToExchangeType
    {
        {"fanout", AMQP::fanout},
        {"direct", AMQP::direct},
        {"topic", AMQP::topic},
        {"headers", AMQP::headers},
        {"consistent_hash", AMQP::consistent_hash},
        {"message_deduplication", AMQP::message_deduplication}
    };
/// AMQP::ExchangeType to c-string look-up
static inline std::array<const char *, 6> eTypeToStr
    {
        "fanout",
        "direct",
        "topic",
        "headers",
        "consistent_hash",
        "message_deduplication"
    };

/**
 * @brief queueBinding struct
 */
struct queueBinding {
  std::string exchange;
  std::string routingKey;

  friend bool operator==(const queueBinding &a1, const queueBinding &a2) {
    return std::tie(a1.exchange, a1.routingKey) == std::tie(a2.exchange, a2.routingKey);
  }

  queueBinding(const std::string &exch, const std::string &key) :
      exchange(exch),
      routingKey(key) {}

  queueBinding(const Json::Value &val) :
      exchange(val["source"].asString()),
      routingKey(val["routing_key"].asString()) {}
};

/**
 * @brief rabbitMQUser class
 */
struct rabbitMQUser {
  std::string name;
  std::string passwordHash;
  std::vector<std::string> tags;

  rabbitMQUser(Json::Value &val) :
      name(val["name"].asString()),
      passwordHash(val["password_hash"].asString()),
      tags() {
    tags.resize(val["tags"].size());
    for (int i = 0; i < tags.size(); ++i) {
      tags[i] = val["tags"][i].asString();
    }
  }

};

/**
 * @brief Exchange struct
 */
struct exchange {

  struct exchangeData {
    std::string creator;
    AMQP::ExchangeType type;
    bool autoDelete;
    bool durable;
    bool internal;

    exchangeData(const std::string &creator_a,
                 const AMQP::ExchangeType &type_a,
                 bool autoDelete_a = false,
                 bool durable_a = true,
                 bool internal_a = false) :
        creator(creator_a),
        type(type_a),
        autoDelete(autoDelete_a),
        durable(durable_a),
        internal(internal_a) {}

    exchangeData(Json::Value &val) :
        creator(val["user_who_performed_action"].asString()),
        type(static_cast<AMQP::ExchangeType>(stringToExchangeType.at(val["type"].asString()))),
        autoDelete(val["auto_delete"].asBool()),
        durable(val["durable"].asBool()),
        internal(val["internal"].asBool()) {
    }

    Json::Value toJson() const {
      Json::Value val;
      val["user_who_performed_action"] = creator;
      val["type"] = eTypeToStr[static_cast<size_t>(type)];
      val["auto_delete"] = autoDelete;
      val["durable"] = durable;
      val["internal"] = internal;
      return val;
    }

  };

  exchange(const std::string &name_a, exchangeData &&data) : name(name_a),
                                                             dat(std::move(data)) {}

  exchange(Json::Value &val) :
      name(val["name"].asString()),
      dat(val) {}

  std::string name;
  exchangeData dat;

};

class RabbitMQRestService {
 public:
  RabbitMQRestService(const std::string &base_url,
                      AuthHandler *auth_handler);

  ~RabbitMQRestService();

  Json::Value Whoami();

  bool CreateQueue(const std::string &vhost,
                   const std::string &queue_name,
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
  std::string baseUrl;
  std::string username;
  std::string password;

  AuthHandler *m_authPtr;

  std::string PerformRequest(const std::string &path,
                             const std::string &method,
                             const std::string &data = "");

  static Json::Value ParseJson(const std::string &json_str);
};
}