#include "network_shared/networkTypes.h"

namespace network_types {

Json::Value VariantToJson(const JsonVariant &v) {
  return std::visit([](const auto &arg) -> Json::Value {
    using T = std::decay_t<decltype(arg)>;
    if constexpr (std::is_same_v<T, std::nullptr_t>) {
      return Json::Value(Json::nullValue);
    } else if constexpr (std::is_same_v<T, bool>) {
      return Json::Value(arg);
    } else if constexpr (std::is_same_v<T, int64_t>) {
      return Json::Value(arg);
    } else if constexpr (std::is_same_v<T, uint64_t>) {
      return Json::Value(arg);
    } else if constexpr (std::is_same_v<T, double>) {
      return Json::Value(arg);
    } else if constexpr (std::is_same_v<T, std::string>) {
      return Json::Value(arg);
    }
  }, v);
}

JsonVariant JsonToVariant(const Json::Value &val) {
  switch (val.type()) {
    case Json::nullValue:return JsonVariant(std::nullptr_t{});
    case Json::booleanValue:return JsonVariant(val.asBool());
    case Json::intValue:return JsonVariant(val.asInt64());
    case Json::uintValue:return JsonVariant(val.asUInt64());
    case Json::realValue:return JsonVariant(val.asDouble());
    case Json::stringValue:return JsonVariant(val.asString());

    default:throw shared::InvalidOption(std::to_string(val.type()));
  }
}

Json::Value TestSolveParam::ToJson() {
  Json::Value val;

  val["experiment_id"] = experiment_id;
  val["N"] = N_;
  val["range"][0] = range.first;
  val["range"][1] = range.second;

  for (auto &[key, value] : args) {
    val["args"][key] = VariantToJson(value);
  }
  return val;
}

TestSolveParam::TestSolveParam(Json::Value &val) :
    experiment_id(val["experiment_id"].asUInt()),
    N_(val["N"].asUInt()),
    range(val["range"][0].asUInt(), val["range"][1].asUInt()) {
  auto &vv = val["args"];
  for (auto &key : vv.getMemberNames()) {
    args[key] = JsonToVariant(vv[key]);
  }
}
TestSolveParam TestSolveParam::SliceAway(size_t iter_count) {

  if(!iter_count
  ||
  range.first + iter_count>range.second)
  {

    throw shared::outOfRange(iter_count,
                             1,range.second - range.first + 1);
  }

  TestSolveParam ret = *this;

  ret.range.second = range.first + iter_count - 1;
  this->range.first = range.first + iter_count;

  return ret;
}

rabbitMQUser::rabbitMQUser(Json::Value &val) :
    name(val["name"].asString()),
    password_hash(val["password_hash"].asString()),
    tags() {
  tags.resize(val["tags"].size());
  for (int i = 0; i < tags.size(); ++i) {
    tags[i] = val["tags"][i].asString();
  }
}

queueBinding::queueBinding(const Json::Value &val) :
    exchange(val["source"].asString()),
    routing_key(val["routing_key"].asString()) {}

queueBinding::queueBinding(const std::string &exch,
                           const std::string &key) :
    exchange(exch),
    routing_key(key) {}

exchange::exchange(Json::Value &val) :
    name(val["name"].asString()),
    creator(val["user_who_performed_action"].asString()),
    type(static_cast<AMQP::ExchangeType>(stringToExchangeType.at(val["type"].asString()))),
    autoDelete(val["auto_delete"].asBool()),
    durable(val["durable"].asBool()),
    internal(val["internal"].asBool()) {}

exchange::exchange(const std::string &name,
                   const std::string &creator_a,
                   const AMQP::ExchangeType &type_a,
                   bool auto_delete_a,
                   bool durable_a,
                   bool internal_a) :
    name(name),
    creator(creator_a),
    type(type_a),
    autoDelete(auto_delete_a),
    durable(durable_a),
    internal(internal_a) {}

Json::Value exchange::ToJson() const {
  Json::Value val;
  val["name"] = name;
  val["user_who_performed_action"] = creator;
  val["type"] = eTypeToStr[static_cast<size_t>(type)];
  val["auto_delete"] = autoDelete;
  val["durable"] = durable;
  val["internal"] = internal;
  return val;
}

queue::queue(Json::Value &val) :
    name(val["name"].asString()),
    creator(val["user_who_performed_action"].asString()),
    autoDelete(val["auto_delete"].asBool()),
    durable(val["durable"].asBool()) {}
queue::queue(const std::string &name,
             const std::string &creator_a,
             bool auto_delete_a,
             bool durable_a) :
    name(name),
    creator(creator_a),
    autoDelete(auto_delete_a),
    durable(durable_a) {}
Json::Value queue::ToJson() const {
  Json::Value val;
  val["name"] = name;
  val["user_who_performed_action"] = creator;
  val["auto_delete"] = autoDelete;
  val["durable"] = durable;
  return val;
}

}