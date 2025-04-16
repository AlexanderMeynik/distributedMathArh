

#include <gtest/gtest.h>

#include "../GoogleCommon.h"
#include "network_shared/networkTypes.h"

using namespace network_types;

void ExpectTestSolveParamEqual(const TestSolveParam &lhs, const TestSolveParam &rhs) {
  EXPECT_EQ(lhs.experiment_id, rhs.experiment_id);
  EXPECT_EQ(lhs.N_, rhs.N_);
  EXPECT_EQ(lhs.range.first, rhs.range.first);
  EXPECT_EQ(lhs.range.second, rhs.range.second);
  EXPECT_EQ(lhs.args, rhs.args);
}

void ExpectQueueBindingEqual(const queueBinding &a, const queueBinding &b) {
  EXPECT_EQ(a.exchange, b.exchange);
  EXPECT_EQ(a.routing_key, b.routing_key);
}



class TestSolveParam_TS:public testing::Test
{
 public:
  TestSolveParam GetValidSample
      ()
  {
    TestSolveParam ts;
    ts.N_=10;
    ts.range={0,9999};
    ts.args["val"]=2.0;
    ts.args["msg"]="some string";
    return ts;
  }
};

class NetworkTypesConversionTests:public testing::Test
{

};

TEST_F(TestSolveParam_TS,TestValidSlice)
{
  size_t iter=1000;
  auto ts=GetValidSample();

  auto ts2=ts.SliceAway(iter);

  EXPECT_EQ(ts.range.first,ts2.range.second+1);

  EXPECT_EQ(ts2.range.second-ts2.range.first+1,iter);
}


TEST_F(TestSolveParam_TS,TestZeroSizeSlice)
{
  size_t iter=0;

  auto ts=GetValidSample();
  auto ts_p=ts;

  EXPECT_EXCEPTION_WITH_ARGS(ts.SliceAway(iter),
                             shared::outOfRange,
                             std::make_tuple(iter,1l,ts_p.RangeSize()));

  EXPECT_EQ(ts.range,ts_p.range);

}


TEST_F(TestSolveParam_TS,TestTooLargeSlice)
{
  size_t iter=100000;

  auto ts=GetValidSample();
  auto ts_p=ts;

  EXPECT_EXCEPTION_WITH_ARGS(ts.SliceAway(iter),
                             shared::outOfRange,
                             std::make_tuple(iter,1l,ts_p.RangeSize()));

  EXPECT_EQ(ts.range,ts_p.range);
}


TEST_F(TestSolveParam_TS, SerializeToJson) {

  TestSolveParam ts;
  ts.experiment_id = 1;
  ts.N_ = 10;
  ts.range = {0, 9};
  ts.args["int"] = int64_t(42);
  ts.args["bool"] = true;
  ts.args["double"] = 3.14159;
  ts.args["string"] = std::string("hello");

  Json::Value json = ts.ToJson();

  EXPECT_EQ(json["experiment_id"].asUInt(), 1u);
  EXPECT_EQ(json["N"].asUInt(), 10u);
  EXPECT_EQ(json["range"][0].asUInt(), 0u);
  EXPECT_EQ(json["range"][1].asUInt(), 9u);

  EXPECT_EQ(json["args"]["int"].asInt64(), 42);
  EXPECT_EQ(json["args"]["bool"].asBool(), true);
  EXPECT_DOUBLE_EQ(json["args"]["double"].asDouble(), 3.14159);
  EXPECT_EQ(json["args"]["string"].asString(), "hello");
}

TEST_F(TestSolveParam_TS, DeserializeFromJson) {
  Json::Value json;
  json["experiment_id"] = 2;
  json["N"] = 20;
  json["range"][0] = 5;
  json["range"][1] = 14;
  json["args"]["value"] = 99;
  json["args"]["flag"] = false;
  json["args"]["pi"] = 3.1415;
  json["args"]["text"] = "hello world";

  TestSolveParam ts(json);

  EXPECT_EQ(ts.experiment_id, 2u);
  EXPECT_EQ(ts.N_, 20u);
  EXPECT_EQ(ts.range.first, 5u);
  EXPECT_EQ(ts.range.second, 14u);

  EXPECT_TRUE(std::holds_alternative<int64_t>(ts.args["value"]));
  EXPECT_EQ(std::get<int64_t>(ts.args["value"]), 99);

  EXPECT_TRUE(std::holds_alternative<bool>(ts.args["flag"]));
  EXPECT_EQ(std::get<bool>(ts.args["flag"]), false);

  EXPECT_TRUE(std::holds_alternative<double>(ts.args["pi"]));
  EXPECT_DOUBLE_EQ(std::get<double>(ts.args["pi"]), 3.1415);

  EXPECT_TRUE(std::holds_alternative<std::string>(ts.args["text"]));
  EXPECT_EQ(std::get<std::string>(ts.args["text"]), "hello world");
}

TEST_F(TestSolveParam_TS, SerializeDeserializeEquality) {
  TestSolveParam original;
  original.experiment_id = 3;
  original.N_ = 30;
  original.range = {10, 39};
  original.args["val"] = 7.5;
  original.args["msg"] = std::string("roundtrip test");
  original.args["active"] = true;

  Json::Value json = original.ToJson();

  TestSolveParam copy(json);

  ExpectTestSolveParamEqual(original, copy);
}




TEST_F(NetworkTypesConversionTests, VariantToJson_Nullptr) {
  JsonVariant var = std::nullptr_t{};
  Json::Value json_val = VariantToJson(var);
  EXPECT_TRUE(json_val.isNull());
}

TEST_F(NetworkTypesConversionTests, VariantToJson_Bool) {
  JsonVariant var = true;
  Json::Value json_val = VariantToJson(var);
  EXPECT_TRUE(json_val.isBool());
  EXPECT_TRUE(json_val.asBool());
}

TEST_F(NetworkTypesConversionTests, VariantToJson_Int64) {
  int64_t input = -12345;
  JsonVariant var = input;
  Json::Value json_val = VariantToJson(var);
  EXPECT_TRUE(json_val.isInt());
  EXPECT_EQ(json_val.asInt64(), input);
}

TEST_F(NetworkTypesConversionTests, VariantToJson_UInt64) {
  uint64_t input = 12345;
  JsonVariant var = input;
  Json::Value json_val = VariantToJson(var);
  EXPECT_TRUE(json_val.isUInt());
  EXPECT_EQ(json_val.asUInt64(), input);
}

TEST_F(NetworkTypesConversionTests, VariantToJson_Double) {
  double input = 3.14159;
  JsonVariant var = input;
  Json::Value json_val = VariantToJson(var);
  EXPECT_TRUE(json_val.isDouble());
  EXPECT_DOUBLE_EQ(json_val.asDouble(), input);
}

TEST_F(NetworkTypesConversionTests, VariantToJson_String) {
  std::string input = "hello";
  JsonVariant var = input;
  Json::Value json_val = VariantToJson(var);
  EXPECT_TRUE(json_val.isString());
  EXPECT_EQ(json_val.asString(), input);
}


TEST_F(NetworkTypesConversionTests, JsonToVariant_Null) {
  Json::Value json_val;
  json_val = Json::nullValue;
  JsonVariant var = JsonToVariant(json_val);
  EXPECT_TRUE(std::holds_alternative<std::nullptr_t>(var));
}


TEST_F(NetworkTypesConversionTests, JsonToVariant_Boolean) {
  Json::Value json_val(true);
  JsonVariant var = JsonToVariant(json_val);
  EXPECT_TRUE(std::holds_alternative<bool>(var));
  EXPECT_EQ(std::get<bool>(var), true);
}

TEST_F(NetworkTypesConversionTests, JsonToVariant_Int) {
  int64_t num = -9876;
  Json::Value json_val(num);
  JsonVariant var = JsonToVariant(json_val);
  EXPECT_TRUE(std::holds_alternative<int64_t>(var));
  EXPECT_EQ(std::get<int64_t>(var), num);
}

TEST_F(NetworkTypesConversionTests, JsonToVariant_UInt) {
  uint64_t num = 9876;
  Json::Value json_val(num);
  JsonVariant var = JsonToVariant(json_val);
  EXPECT_TRUE(std::holds_alternative<uint64_t>(var));
  EXPECT_EQ(std::get<uint64_t>(var), num);
}

TEST_F(NetworkTypesConversionTests, JsonToVariant_Double) {
  double num = 2.71828;
  Json::Value json_val(num);
  JsonVariant var = JsonToVariant(json_val);
  EXPECT_TRUE(std::holds_alternative<double>(var));
  EXPECT_DOUBLE_EQ(std::get<double>(var), num);
}

TEST_F(NetworkTypesConversionTests, JsonToVariant_String) {
  std::string str = "world";
  Json::Value json_val(str);
  JsonVariant var = JsonToVariant(json_val);
  EXPECT_TRUE(std::holds_alternative<std::string>(var));
  EXPECT_EQ(std::get<std::string>(var), str);
}


TEST_F(NetworkTypesConversionTests, JsonToVariant_UnsuprotedType) {

  Json::Value json_val;
  json_val["data"]="ss";
  json_val["size"]=2;
  JsonVariant var;

  EXPECT_EXCEPTION_WITH_ARGS(var=JsonToVariant(json_val),
                             shared::InvalidOption,
                             std::make_tuple(std::to_string(Json::objectValue)));

  EXPECT_TRUE(std::holds_alternative<std::nullptr_t>(var));
  
}



TEST(NetworkTypesRabbitMQUserTests, JsonConstructor) {

  Json::Value json_val;
  json_val["name"] = "user1";
  json_val["password_hash"] = "hash123";
  Json::Value tags;
  tags.append("tag1");
  tags.append("tag2");
  json_val["tags"] = tags;

  rabbitMQUser user(json_val);
  
  EXPECT_EQ(user.name, "user1");
  EXPECT_EQ(user.password_hash, "hash123");
  ASSERT_EQ(user.tags.size(), 2u);
  EXPECT_EQ(user.tags[0], "tag1");
  EXPECT_EQ(user.tags[1], "tag2");
}


TEST(NetworkTypesExchangeTests, JsonConstructorAndToJson) {

  Json::Value json_val;
  json_val["name"] = "exch1";
  json_val["user_who_performed_action"] = "creator1";
  json_val["type"] = "direct";
  json_val["auto_delete"] = false;
  json_val["durable"] = true;
  json_val["internal"] = false;

  exchange exch(json_val);
  EXPECT_EQ(exch.name, "exch1");
  EXPECT_EQ(exch.creator, "creator1");
  EXPECT_EQ(exch.type, stringToExchangeType.at("direct"));
  EXPECT_EQ(exch.autoDelete, false);
  EXPECT_EQ(exch.durable, true);
  EXPECT_EQ(exch.internal, false);

  Json::Value out_json = exch.ToJson();
  EXPECT_EQ(out_json["name"].asString(), "exch1");
  EXPECT_EQ(out_json["user_who_performed_action"].asString(), "creator1");
  
  EXPECT_EQ(out_json["type"].asString(), "direct");
  EXPECT_EQ(out_json["auto_delete"].asBool(), false);
  EXPECT_EQ(out_json["durable"].asBool(), true);
  EXPECT_EQ(out_json["internal"].asBool(), false);
}

TEST(NetworkTypesExchangeTests, ParameterizedConstructor) {

  exchange exch("exch2", "creator2", stringToExchangeType.at("topic"), true, false, true);
  EXPECT_EQ(exch.name, "exch2");
  EXPECT_EQ(exch.creator, "creator2");
  EXPECT_EQ(exch.type, stringToExchangeType.at("topic"));
  EXPECT_EQ(exch.autoDelete, true);
  EXPECT_EQ(exch.durable, false);
  EXPECT_EQ(exch.internal, true);

  Json::Value out_json = exch.ToJson();
  EXPECT_EQ(out_json["name"].asString(), "exch2");
  EXPECT_EQ(out_json["user_who_performed_action"].asString(), "creator2");
  EXPECT_EQ(out_json["type"].asString(), "topic");
  EXPECT_EQ(out_json["auto_delete"].asBool(), true);
  EXPECT_EQ(out_json["durable"].asBool(), false);
  EXPECT_EQ(out_json["internal"].asBool(), true);
}

TEST(NetworkTypesQueueTests, JsonConstructorAndToJson) {
  Json::Value json_val;
  json_val["name"] = "queue1";
  json_val["user_who_performed_action"] = "creator1";
  json_val["auto_delete"] = false;
  json_val["durable"] = true;

  queue q(json_val);
  EXPECT_EQ(q.name, "queue1");
  EXPECT_EQ(q.creator, "creator1");
  EXPECT_EQ(q.autoDelete, false);
  EXPECT_EQ(q.durable, true);

  Json::Value out_json = q.ToJson();
  EXPECT_EQ(out_json["name"].asString(), "queue1");
  EXPECT_EQ(out_json["user_who_performed_action"].asString(), "creator1");
  EXPECT_EQ(out_json["auto_delete"].asBool(), false);
  EXPECT_EQ(out_json["durable"].asBool(), true);
}

TEST(NetworkTypesQueueTests, ParameterizedConstructor) {
  queue q("queue2", "creator2", true, false);
  EXPECT_EQ(q.name, "queue2");
  EXPECT_EQ(q.creator, "creator2");
  EXPECT_EQ(q.autoDelete, true);
  EXPECT_EQ(q.durable, false);

  Json::Value out_json = q.ToJson();
  EXPECT_EQ(out_json["name"].asString(), "queue2");
  EXPECT_EQ(out_json["user_who_performed_action"].asString(), "creator2");
  EXPECT_EQ(out_json["auto_delete"].asBool(), true);
  EXPECT_EQ(out_json["durable"].asBool(), false);
}

TEST(NetworkTypesQueueBindingTests, ConstructFromJson) {
  Json::Value val;
  val["source"] = "test_exchange";
  val["routing_key"] = "my.routing.key";

  queueBinding binding(val);

  EXPECT_EQ(binding.exchange, "test_exchange");
  EXPECT_EQ(binding.routing_key, "my.routing.key");
}

TEST(NetworkTypesQueueBindingTests, ConstructFromStrings) {
  std::string exch = "logs";
  std::string key = "error.warning";

  queueBinding binding(exch, key);

  EXPECT_EQ(binding.exchange, "logs");
  EXPECT_EQ(binding.routing_key, "error.warning");
}

TEST(NetworkTypesQueueBindingTests, ConstructJsonEquivalence) {
  std::string exch = "exch.name";
  std::string key = "routing.path";

  Json::Value val;
  val["source"] = exch;
  val["routing_key"] = key;

  queueBinding from_json(val);
  queueBinding from_strings(exch, key);

  ExpectQueueBindingEqual(from_json, from_strings);
}

