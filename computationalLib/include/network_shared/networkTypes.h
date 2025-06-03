#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <valarray>

#include "common/commonTypes.h"
#include "common/errorHandling.h"
#include "common/enumUtils.h"

#include <json/json.h>
#include <amqpcpp/exchangetype.h>

/// Namespace for network related types
namespace network_types {

using common_types::JsonVariant;
using  namespace enum_utils;

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


static const std::vector<EnumMapping<AMQP::ExchangeType>> kExchangeTypeMappings = {
    {AMQP::fanout,"fanout"},
    {AMQP::direct,"direct"},
    {AMQP::topic,"topic"},
    {AMQP::headers,"headers"},
    {AMQP::consistent_hash,"consistent_hash"},
    {AMQP::message_deduplication,"message_deduplication"}

};

static const auto kStrToExchangeType =
    CreateStrToEnumMap(kExchangeTypeMappings);
const auto kExchangeTypeToStr=
    CreateEnumToStrMap(kExchangeTypeMappings);


enum class HttpMethod {
  GET,
  POST,
  PUT,
  DELETE,
  HEAD,
  OPTIONS,
  PATCH,
  CONNECT,
  TRACE
};

static const std::vector<EnumMapping<HttpMethod>> kHttpMethodMappings = {
    {HttpMethod::GET, "GET"},
    {HttpMethod::POST, "POST"},
    {HttpMethod::PUT, "PUT"},
    {HttpMethod::DELETE, "DELETE"},
    {HttpMethod::HEAD, "HEAD"},
    {HttpMethod::OPTIONS, "OPTIONS"},
    {HttpMethod::PATCH, "PATCH"},
    {HttpMethod::CONNECT, "CONNECT"},
    {HttpMethod::TRACE, "TRACE"}
};

static const auto kStrToHttpMethod =
    CreateStrToEnumMap(kHttpMethodMappings);
static const auto kHttpMethodToStr =
    CreateEnumToStrMap(kHttpMethodMappings);


inline std::string ToString(HttpMethod method) {
  return EnumToStr(method, kHttpMethodToStr);
}

inline HttpMethod FromString(const std::string& str) {
  return StrToEnum(str, kStrToHttpMethod);
}

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

  message(const std::string &key, const std::string &pay, const std::string &payloadType);

  std::string routing_key;
  std::string payload;
  std::string payload_encoding;

  Json::Value ToJson() const;

  message(Json::Value &val);

};

/**
 * @brief global_param struct
 */
struct global_param {
  std::string name;
  Json::Value value;

  global_param(const std::string pName, const Json::Value &val);

  Json::Value ToJson() const;

  global_param(Json::Value &val);
};



}