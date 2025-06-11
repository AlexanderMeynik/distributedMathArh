#include <tuple>
#include <gtest/gtest.h>
#include "../GoogleCommon.h"
#include "network_shared/connectionString.h"

using namespace network_types;

/*
using Ptype=std::tuple<PostgreSQLCStr>;
class PostgreSQLCStrTests : public ::testing::TestWithParam<Ptype> {
 public:

};

INSTANTIATE_TEST_SUITE_P(
    ValidatePrintParse, PostgreSQLCStrTests,
    ::testing::Values({PostgreSQLCStr("user","pass","host",5432)}),
    TupleToString<PostgreSQLCStrTests>));*/

TEST(PostgreSQLCStrTests,TestPrintParse)
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


TEST(AMQPSQLCStrTest,TestPrintParse)
{
  AMQPSQLCStr tt("user","pass","host:12", true);

  auto str=tt.CStr();

  AMQPSQLCStr tt2;
  tt2.FromString(str);

  ASSERT_EQ(tt,tt2)<<fmt::format("AMQPSQLCStr strings are different s1="
                                 "\"{}\", s2= \"{}\"",
                                 tt.CStr(),
                                 tt2.CStr());
}