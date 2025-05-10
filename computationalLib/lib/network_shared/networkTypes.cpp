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

  if (!iter_count
      ||
          range.first + iter_count > range.second) {

    throw shared::outOfRange(iter_count,
                             1, range.second - range.first + 1);
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

connection::connection(Json::Value &val) :
    channels(val["channels"].asUInt()),
    host(val["host"].asString()),
    name(val["name"].asString()),
    peer_host(val["peer_host"].asString()),
    port(val["port"].asUInt()),
    peer_port(val["peer_port"].asUInt()),
    connected_at(val["connected_at"].asUInt64()),
    user(val["user"].asString()) {}

Json::Value connection::ToJson() const {
  Json::Value val;
  val["channels"] = static_cast<Json::UInt>(channels);
  val["host"] = host;
  val["name"] = name;
  val["peer_host"] = peer_host;
  val["port"] = static_cast<Json::UInt>(port);
  val["peer_port"] = static_cast<Json::UInt>(peer_port);
  val["connected_at"] = static_cast<Json::UInt64>(connected_at);
  val["user"] = user;
  return val;
}

Json::Value channel::ToJson() const {
  Json::Value val;
  val["name"] = name;
  val["number"] = static_cast<Json::UInt>(channel_number);
  val["connection_name"] = connection_name;
  val["user"] = user;
  val["vhost"] = vhost;
  val["state"] = state;
  val["consumer_count"] = consumer_count;
  return val;
}
channel::channel(Json::Value &val) :
    name(val["name"].asString()),
    channel_number(val["number"].asUInt()),
    connection_name(val["connection_details"]["name"].asString()),
    user(val["user"].asString()),
    vhost(val["vhost"].asString()),
    state(val["state"].asString()),
    consumer_count(val["consumer_count"].asUInt()) {}
Json::Value message::ToJson() const {
  Json::Value val;

  val["properties"] = Json::objectValue;

  val["routing_key"] = routing_key;
  val["payload"] = payload;
  val["payload_encoding"] = payload_encoding;
  return val;
}
message::message(Json::Value &val) {
  routing_key = val["routing_key"].asString();
  payload = val["payload"].asString();
  payload_encoding = val["payload_encoding"].asString();
}
message::message(const std::string &key, const std::string &pay, const std::string &payloadType) {
  routing_key = key;
  payload = pay;
  payload_encoding = payloadType;

}
global_param::global_param(Json::Value &val) {
  name = val["name"].asString();
  value = val["value"];
}
Json::Value global_param::ToJson() const {
  Json::Value ret;

  ret["name"] = name;
  ret["value"] = value;
  return ret;
}
global_param::global_param(const std::string pName, const Json::Value &val) {
  name = pName;
  value = val;
}
myConnString::myConnString(std::string_view user,
                           std::string_view password,
                           std::string_view host,
                           std::string_view dbname,
                           unsigned int port)
    : user(user), password(password),
      host(host),
      dbname(dbname),
      port(port) {
  UpdateFormat();
}
myConnString::operator std::string_view() {
  return formatted_string;
}
const char *myConnString::CStr() const {
  return formatted_string.c_str();
}
void myConnString::SetPassword(std::string_view new_password) {
  password = new_password;
  UpdateFormat();
}
void myConnString::SetHost(std::string_view new_host) {
  host = new_host;
  UpdateFormat();
}
void myConnString::SetPort(unsigned int new_port) {
  port = new_port;
  UpdateFormat();
}
void myConnString::SetDbname(std::string_view new_dbname) {
  dbname = new_dbname;
  UpdateFormat();
}
void myConnString::SetUser(std::string_view new_user) {
  user = std::forward<std::string_view>(new_user);
  UpdateFormat();
}
const std::string &myConnString::GetUser() const {
  return user;
}
const std::string &myConnString::GetPassword() const {
  return password;
}
const std::string &myConnString::GetHost() const {
  return host;
}
const std::string &myConnString::GetDbname() const {
  return dbname;
}
unsigned int myConnString::GetPort() const {
  return port;
}
void myConnString::UpdateFormat() {
  formatted_string = fmt::format("postgresql://{}:{}@{}:{}/{}",
                                 user.c_str(), password.c_str(), host.c_str(), port, dbname.c_str());
}
std::string myConnString::GetVerboseName() const {
  return fmt::format("{}:{} db:{}", host, port, dbname);
}
bool myConnString::operator==(const myConnString &rhs) const {
  return user == rhs.user &&
      password == rhs.password &&
      host == rhs.host &&
      dbname == rhs.dbname &&
      port == rhs.port &&
      formatted_string == rhs.formatted_string;
}

}