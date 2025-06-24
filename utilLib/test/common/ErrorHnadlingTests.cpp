#include <gtest/gtest.h>
#include "common/errorHandling.h"

TEST(ExceptionTests, BrokenConnection) {
  auto exception=shared::Broken_Connection("serviceC", "hostD");
  try {
    throw exception;
  } catch (const shared::Broken_Connection& e) {
    std::string_view message=e.what();
    //todo contains macro
    EXPECT_TRUE(message.contains("Service serviceC is unable to connect to hostD!"))<<message;
    EXPECT_EQ(e.getParams(),exception.getParams());
  }
}
//todo use https://google.github.io/googletest/reference/testing.html#TYPED_TEST_SUITE
//todo add string as a prams