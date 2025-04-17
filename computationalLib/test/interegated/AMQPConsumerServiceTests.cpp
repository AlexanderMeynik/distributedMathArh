#include <gtest/gtest.h>

#include "network_shared/AMQPConsumerService.h"
#include "network_shared/AMQPPublisherService.h"

TEST(ss2,ss2)
{
  ASSERT_TRUE(true);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}