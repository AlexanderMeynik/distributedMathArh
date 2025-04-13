#include "network_shared/networkTypes.h"

namespace network_types
{

Json::Value TestSolveParam::ToJson() {
  Json::Value val;

  val["experiment_id"]=experiment_id;
  val["range"][0]=range.first;
  val["range"][1]=range.second;

  for(auto&[key,value]:args)
  {
    val["args"][key]=value;
  }
  return val;
}

TestSolveParam::TestSolveParam(Json::Value &val):
    experiment_id(val["experiment_id"].asUInt()),
    range(val["range"][0].asUInt(), val["range"][1].asUInt())
{
  auto &vv=val["args"];
  for(auto&key:vv.getMemberNames())
  {
    args[key]=vv[key].asDouble();
  }
}

rabbitMQUser::rabbitMQUser(Json::Value &val):
    name(val["name"].asString()),
    password_hash(val["password_hash"].asString()),
    tags() {
  tags.resize(val["tags"].size());
  for (int i = 0; i < tags.size(); ++i) {
    tags[i] = val["tags"][i].asString();
  }
}

queueBinding::queueBinding(const Json::Value &val):
    exchange(val["source"].asString()),
    routing_key(val["routing_key"].asString()) {}

queueBinding::queueBinding(const std::string &exch, const std::string &key):
    exchange(exch),
    routing_key(key) {}

}