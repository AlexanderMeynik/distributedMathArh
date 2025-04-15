#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <valarray>


#include <json/json.h>
#include <amqpcpp/exchangetype.h>
#include <common/commonTypes.h>

/// Namespace for network related types
namespace network_types {

using common_types::JsonVariant;

/**
 * @brief Casts JsonVariant to Json::Value
 * @param v
 */
Json::Value VariantToJson(const JsonVariant &v);

/**
 * @brief Casts Json::Value to JsonVariant
 * @param val
 */
JsonVariant JsonToVariant(const Json::Value &val);

/**
 * @brief Simple task sent structure
 */
struct TestSolveParam {
  TestSolveParam() = default;
  size_t experiment_id;
  size_t N_;
  std::pair<size_t, size_t> range;
  std::unordered_map<std::string, JsonVariant> args;
  bool operator==(const TestSolveParam &oth) const = default;

  TestSolveParam SliceAway(size_t iter_count) {
    TestSolveParam ret = *this;

    ret.range.second = range.first + iter_count - 1;
    this->range.first = range.first + iter_count;

    return ret;
  }

  bool Slice(size_t iter_count) {
    if (range.first + iter_count >= range.second) {
      return false;
    }
    range.first += iter_count;
    return true;
  }

  Json::Value ToJson();
  TestSolveParam(Json::Value &val);
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

  friend bool operator==(const queueBinding &a1, const queueBinding &a2) = default;

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

/**
 * @brief Exchange struct
 */
struct exchange {

  std::string name;
  std::string creator;
  AMQP::ExchangeType type;
  bool autoDelete;
  bool durable;
  bool internal;

  Json::Value ToJson() const;

  exchange(const std::string &name,
           const std::string &creator_a,
           const AMQP::ExchangeType &type_a,
           bool auto_delete_a = false,
           bool durable_a = true,
           bool internal_a = false);

  exchange(Json::Value &val);
};

/**
 * @brief Queue struct
 */
struct queue {

  std::string name;
  std::string creator;
  bool autoDelete;
  bool durable;

  Json::Value ToJson() const;

  queue(const std::string &name,
        const std::string &creator_a,
        bool auto_delete_a = false,
        bool durable_a = true);

  queue(Json::Value &val);
};

}