#pragma once
#include <string>
#include <unordered_map>

#include <json/json.h>
#include <amqpcpp/exchangetype.h>

/// Namespace for network related types
namespace network_types
{

/**
 * @brief Simple task sent structure
 */
struct TestSolveParam
{
  TestSolveParam()=default;
  size_t experiment_id;
  std::pair<size_t, size_t > range;
  std::unordered_map<std::string,double> args;
  bool operator==(const TestSolveParam&oth)const=default;

  Json::Value ToJson();
  TestSolveParam(Json::Value&val);
};

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
  std::string routing_key;

  friend bool operator==(const queueBinding &a1, const queueBinding &a2)=default;

  queueBinding(const std::string &exch, const std::string &key);

  queueBinding(const Json::Value &val);
};

/**
 * @brief rabbitMQUser class
 */
struct rabbitMQUser {
  std::string name;
  std::string password_hash;
  std::vector<std::string> tags;

  rabbitMQUser(Json::Value &val);

};

}