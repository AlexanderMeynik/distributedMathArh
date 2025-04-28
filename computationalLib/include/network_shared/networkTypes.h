#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <valarray>

#include "common/commonTypes.h"
#include "common/errorHandling.h"

#include <json/json.h>
#include <amqpcpp/exchangetype.h>

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
 * @struct TestSolveParam
 * @brief Simple task sent structure
 * @details This structure contains information about iteration count
 * as well as input parameters.
 * This one is designed to make process of iteration block separation easier
 * @see common_types::JsonVariant - for more information about argument types
 */
struct TestSolveParam {
  TestSolveParam() = default;
  size_t experiment_id;///< current experiment id
  size_t N_;///< size of the system
  std::pair<size_t, size_t> range;///< iteration range: [range.first,range,second]
  std::unordered_map<std::string, JsonVariant> args;///< map can contain any primitive JSON types
  bool operator==(const TestSolveParam &oth) const = default;

  size_t RangeSize() {
    return range.second - range.first + 1;
  }

  /**
   * @brief Slices iter_count iterations from current range and creates
   * @param iter_count
   * @return TestSolveParam with iter_count iterations
   * @throws shared::outOfRange - if block with specified size can't sliced away
   */
  TestSolveParam SliceAway(size_t iter_count);

  /**
   * @brief Serialize as json
   * @return Json::Value
   */
  Json::Value ToJson();

  /**
   * @brief Deserialize from Json::Value
   * @param val
   */
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

  bool operator==(const queueBinding &oth) const = default;

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

/**
 * @brief connection struct
 */
struct connection {

  size_t channels;
  std::string host;
  std::string name;
  std::string peer_host;
  size_t port;
  size_t peer_port;
  uint64_t connected_at;
  std::string user;

  Json::Value ToJson() const;

  connection(Json::Value &val);

};

/**
 * @brief channel struct
 */
struct channel {

  std::string name;
  size_t channel_number;
  std::string connection_name;
  std::string user;
  std::string vhost;
  std::string state;
  size_t consumer_count;

  Json::Value ToJson() const;

  channel(Json::Value &val);

};

/**
 * @brief message struct
 */
struct message {

  message(const std::string &key, const std::string &pay, const std::string &payloadType) {
    routing_key=key;
    payload=pay;
    payload_encoding=payloadType;

  }

  std::string routing_key;
  std::string payload;
  std::string payload_encoding;


  Json::Value ToJson() const
  {
    Json::Value val;

    val["properties"]="{}";
    val["routing_key"]=routing_key;
    val["payload"]=payload;
    val["payload_encoding"]=payload_encoding;
    return val;
  }

  message(Json::Value &val)
  {
    routing_key=val["routing_key"].asString();
    payload=val["payload"].asString();
    payload_encoding=val["payload_encoding"].asString();
  }

};

}