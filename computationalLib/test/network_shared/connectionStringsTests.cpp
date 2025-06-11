#include <tuple>
#include <gtest/gtest.h>
#include "../GoogleCommon.h"
#include "network_shared/connectionString.h"

using namespace network_types;


class ParsingTests : public ::testing::Test {
 public:

};

TEST_F(ParsingTests,PostgreSQLCStrTest)
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

TEST_F(ParsingTests,AMQPSQLCStrTest)
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