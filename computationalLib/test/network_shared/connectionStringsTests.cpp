#include <tuple>
#include <gtest/gtest.h>
#include "network_shared/connectionString.h"

using namespace network_types;


class PostgreSQLCStrTest : public ::testing::Test {
 protected:
  PostgreSQLCStr conn_;
};

class AMQPSQLCStrTest : public ::testing::Test {
 protected:
  AMQPSQLCStr conn_;
};

TEST_F(PostgreSQLCStrTest,PrintParseLogic)
{
  PostgreSQLCStr tt("user","pass","host","dbname",5433);

  auto str=tt.CStr();

  PostgreSQLCStr tt2;
  tt2.FromString(str);


  ASSERT_EQ(tt,tt2)<<fmt::format("PostgrSQL strings are different s1="
                                 "\"{}\", s2= \"{}\"",
                                 tt.CStr(),
                                 tt2.CStr());
}
TEST_F(PostgreSQLCStrTest,PrintParseLogicThrow)
{
  auto kStr="someInvalidURL";

  PostgreSQLCStr tt2;

  ASSERT_THROW(tt2.FromString(kStr), shared::ScanningError);
}


TEST_F(PostgreSQLCStrTest,PrintParseVerboseFormat)
{
  conn_.SetHost("localhost");
  conn_.SetPort(5321);
  conn_.SetUser("guest");
  conn_.SetPassword("guest");
  conn_.SetDbname("dbname");

  ASSERT_EQ(conn_.GetVerboseName(), "localhost:5321 db:dbname");
}

TEST_F(PostgreSQLCStrTest, DefaultConstructor) {
  EXPECT_EQ(conn_.GetUser(), "");
  EXPECT_EQ(conn_.GetPassword(), "");
  EXPECT_EQ(conn_.GetHost(), "");
  EXPECT_EQ(conn_.GetDbname(), "");
  EXPECT_EQ(conn_.GetPort(), 5432U);
  EXPECT_EQ(conn_.to_string(), "");
}


TEST_F(PostgreSQLCStrTest, SettersAndGetters) {
  conn_.SetUser("myuser");
  EXPECT_EQ(conn_.GetUser(), "myuser");

  conn_.SetPassword("mypassword");
  EXPECT_EQ(conn_.GetPassword(), "mypassword");

  conn_.SetHost("localhost");
  EXPECT_EQ(conn_.GetHost(), "localhost");

  conn_.SetPort(5432);
  EXPECT_EQ(conn_.GetPort(), 5432);

  conn_.SetDbname("mydb");
  EXPECT_EQ(conn_.GetDbname(), "mydb");

  ASSERT_EQ(conn_.to_string(),
            "postgresql://myuser:mypassword@localhost:5432/mydb");
}


TEST_F(AMQPSQLCStrTest,PrintParseLogic)
{
  AMQPSQLCStr tt("host:12","user","pass", true);

  auto str=tt.CStr();

  AMQPSQLCStr tt2;
  tt2.FromString(str);

  ASSERT_EQ(tt,tt2)<<fmt::format("AMQPSQLCStr strings are different s1="
                                 "\"{}\", s2= \"{}\"",
                                 tt.CStr(),
                                 tt2.CStr());
}


TEST_F(AMQPSQLCStrTest,PrintParseLogicThrow)
{
  auto kStr="someInvalidURL";

  AMQPSQLCStr tt2;

  ASSERT_THROW(tt2.FromString(kStr), shared::ScanningError);
}


TEST_F(AMQPSQLCStrTest,PrintParseVerboseFormat)
{
  conn_.SetHostPort("localhost:5672");
  conn_.SetUser("guest");
  conn_.SetPassword("guest");

  conn_.SetSecure(true);
  ASSERT_EQ(conn_.GetVerboseName(), "localhost:5672 secure:1");
}


TEST_F(AMQPSQLCStrTest, DefaultConstructor) {
  EXPECT_EQ(conn_.GetHostPort(), "");
  EXPECT_EQ(conn_.GetUser(), "");
  EXPECT_EQ(conn_.GetPassword(), "");
  EXPECT_FALSE(conn_.IsSecure());
  EXPECT_EQ(conn_.to_string(), "");
}

TEST_F(AMQPSQLCStrTest, SettersAndGetters) {
  conn_.SetHostPort("localhost:5672");
  EXPECT_EQ(conn_.GetHostPort(), "localhost:5672");

  conn_.SetUser("guest");
  EXPECT_EQ(conn_.GetUser(), "guest");

  conn_.SetPassword("guest");
  EXPECT_EQ(conn_.GetPassword(), "guest");

  conn_.SetSecure(true);
  EXPECT_TRUE(conn_.IsSecure());

  conn_.SetSecure(false);
  EXPECT_FALSE(conn_.IsSecure());

  ASSERT_EQ(conn_.to_string(), "amqp://guest:guest@localhost:5672/");
}



TEST_F(AMQPSQLCStrTest, FromStringNonSecure) {
  std::string conn_str = "amqp://guest:guest@localhost:5672/";
  conn_.FromString(conn_str);

  EXPECT_EQ(conn_.GetHostPort(), "localhost:5672");
  EXPECT_EQ(conn_.GetUser(), "guest");
  EXPECT_EQ(conn_.GetPassword(), "guest");
  EXPECT_FALSE(conn_.IsSecure());
}

TEST_F(AMQPSQLCStrTest, FromStringSecure) {
  std::string conn_str = "amqps://guest:guest@localhost:5672/";
  conn_.FromString(conn_str);

  EXPECT_EQ(conn_.GetHostPort(), "localhost:5672");
  EXPECT_EQ(conn_.GetUser(), "guest");
  EXPECT_EQ(conn_.GetPassword(), "guest");
  EXPECT_TRUE(conn_.IsSecure());
}