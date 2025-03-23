#include "parallelUtils/chronoClock.h"
#include <gtest/gtest.h>

using chronoClock::chronoClockTemplate;
using namespace timing;
using namespace std::chrono_literals;

#define SLEEP(dur) std::this_thread::sleep_for(dur)
#define ASSERT_NEAR_REL(val1, val2, rel_error) ASSERT_NEAR(val1,val2,rel_error*val1/100)
#define ASSERT_HISTORY_EQ(index, function) ASSERT_EQ(fff.call_history[index],(void *)function);
#define COMMA ,

using ratio = std::milli;
using durationType = std::chrono::duration<int64_t, ratio>;
static constexpr double absErr = 1e-2;
static constexpr double relErr = 1;

/**
 * @brief Test various measurement situations
 */
class ClockArrayTest : public ::testing::Test {
protected:
    chronoClockTemplate<ratio> clk;
};

TEST_F(ClockArrayTest, simple_linear_calculation) {
    durationType dd{14};
    auto key = clk.tikLoc();
    SLEEP(dd);
    clk.tak();
    ASSERT_NEAR_REL(dd.count(), clk[key].time, relErr);
}

TEST_F(ClockArrayTest, inlined_clocks) {
    durationType dd{20};
    durationType dd2{5};
    auto key1 = clk.tikLoc();

    locationType key2;
    {
        SLEEP(dd);
        key2 = clk.tikLoc();
        SLEEP(dd2);
        clk.tak();
    }
    clk.tak();
    ASSERT_NEAR_REL(dd.count() + dd2.count(), clk[key1].time, relErr);
    ASSERT_NEAR_REL(dd2.count(), clk[key2].time, relErr);
}


TEST_F(ClockArrayTest, loop_clock) {
    durationType dd{20};
    constexpr size_t loop_count = 10;
    locationType key;
    for (int i = 0; i < loop_count; ++i) {
        key = clk.tikLoc();
        SLEEP(dd);
        clk.tak();
    }
    ASSERT_NEAR_REL(dd.count() * loop_count, clk[key].time, relErr*10);
}

TEST_F(ClockArrayTest, subsequent_section_clock) {
    durationType dd{20};
    durationType dd2{60};
    durationType dd3{12};
    auto pair = clk.tikPair();
    locationType key = pair.second;
    SLEEP(dd);
    clk.tak();

    SLEEP(dd2);

    clk.tik(pair.first);
    SLEEP(dd3);
    clk.tak();

    ASSERT_NEAR_REL(dd.count() + dd3.count(), clk[key].time, relErr);
}


TEST_F(ClockArrayTest, lambda_function_call) {
    durationType dd{20};

    locationType key = clk.tikLoc();

    std::source_location ss;

    [&]() {
        auto pp = clk.tikPair();
        ss = pp.first;
        key = pp.second;
        SLEEP(dd);
        clk.tak();
    }();
    ASSERT_TRUE(std::string(ss.function_name()).contains(key[0]));
}

TEST_F(ClockArrayTest, mismatched_tik_tak) {
    ASSERT_THROW(clk.tak(), std::logic_error);
}


TEST_F(ClockArrayTest, reset_empty_tok) {
    durationType dd{20};
    durationType dd2{60};
    durationType dd3{12};
    auto pair = clk.tikPair();
    locationType key = pair.second;
    SLEEP(dd);
    clk.tak();

    SLEEP(dd2);

    clk.tik(pair.first);
    SLEEP(dd3);
    clk.tak();

    ASSERT_NO_THROW(clk.reset());
}