#include "parallelUtils/chronoClock.h"
#include <gtest/gtest.h>

using chrono_clock::ChronoClockTemplate;
using namespace timing;
using namespace std::chrono_literals;

#define SLEEP(dur) std::this_thread::sleep_for(dur)
#define ASSERT_NEAR_REL(val1, val2, rel_error) ASSERT_NEAR(val1,val2,rel_error*val1/100)
#define ASSERT_HISTORY_EQ(index, function) ASSERT_EQ(fff.call_history[index],(void *)function);
#define COMMA ,

using Ratio = std::milli;
using DurationType = std::chrono::duration<int64_t, Ratio>;
static constexpr double kAbsErr = 1e-2;
static constexpr double kRelErr = 10;

/**
 * @brief Test various measurement situations
 */
class ClockArrayTest : public ::testing::Test {
 protected:
  ChronoClockTemplate<Ratio> clk_;
};

TEST_F(ClockArrayTest, simple_linear_calculation) {
  DurationType dd{14};
  auto key = clk_.TikLoc();
  SLEEP(dd);
  clk_.Tak();
  ASSERT_NEAR_REL(dd.count(), clk_[key].time, kRelErr);
}

TEST_F(ClockArrayTest, inlined_clocks) {
  DurationType dd{20};
  DurationType dd2{5};
  auto key1 = clk_.TikLoc();

  locationType key2;
  {
    SLEEP(dd);
    key2 = clk_.TikLoc();
    SLEEP(dd2);
    clk_.Tak();
  }
  clk_.Tak();
  ASSERT_NEAR_REL(dd.count() + dd2.count(), clk_[key1].time, kRelErr);
  ASSERT_NEAR_REL(dd2.count(), clk_[key2].time, kRelErr);
}

TEST_F(ClockArrayTest, loop_clock) {
  DurationType dd{20};
  constexpr size_t kLoopCount = 10;
  locationType key;
  for (int i = 0; i < kLoopCount; ++i) {
    key = clk_.TikLoc();
    SLEEP(dd);
    clk_.Tak();
  }
  ASSERT_NEAR_REL(dd.count() * kLoopCount, clk_[key].time, kRelErr * 10);
}

TEST_F(ClockArrayTest, subsequent_section_clock) {
  DurationType dd{20};
  DurationType dd2{60};
  DurationType dd3{12};
  auto pair = clk_.TikPair();
  locationType key = pair.second;
  SLEEP(dd);
  clk_.Tak();

  SLEEP(dd2);

  clk_.Tik(pair.first);
  SLEEP(dd3);
  clk_.Tak();

  ASSERT_NEAR_REL(dd.count() + dd3.count(), clk_[key].time, kRelErr);
}

TEST_F(ClockArrayTest, lambda_function_call) {
  DurationType dd{20};

  locationType key = clk_.TikLoc();

  std::source_location ss;

  [&]() {
    auto pp = clk_.TikPair();
    ss = pp.first;
    key = pp.second;
    SLEEP(dd);
    clk_.Tak();
  }();
  ASSERT_TRUE(std::string(ss.function_name()).contains(key[0]));
}

TEST_F(ClockArrayTest, mismatched_tik_tak) {
  ASSERT_THROW(clk_.Tak(), std::logic_error);
}

TEST_F(ClockArrayTest, reset_empty_tok) {
  DurationType dd{20};
  DurationType dd2{60};
  DurationType dd3{12};
  auto pair = clk_.TikPair();
  locationType key = pair.second;
  SLEEP(dd);
  clk_.Tak();

  SLEEP(dd2);

  clk_.Tik(pair.first);
  SLEEP(dd3);
  clk_.Tak();

  ASSERT_NO_THROW(clk_.Reset());
}