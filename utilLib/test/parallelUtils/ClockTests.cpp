
#include <chrono>


#include "common/lib.h"

#include "common/printUtils.h"
#include "parallelUtils/OpenmpParallelClock.h"
#include <iomanip>
#include <omp.h>
#include <chrono>
#include <thread>
#include "../GoogleCommon.h"//todo вот этому здесь не место
#include <gtest/gtest.h>

constexpr double tolerance = 0.1;
using namespace timing;

void nanoSleep(int num) {
    std::this_thread::sleep_for(std::chrono::nanoseconds(num));
}

void Msleep(double seconds) {
    nanoSleep((int) (1e9 * seconds));
}

constexpr double sleep_sec = 0.1;
constexpr size_t iteartions = 12;

namespace detail // EDIT: make it an implementation detail
{
    template<typename T>
    struct deduce_type;

    template<typename RETURN_TYPE, typename CLASS_TYPE, typename... ARGS>
    struct deduce_type<RETURN_TYPE(CLASS_TYPE::*)(ARGS...) const> {
        using type = std::function<RETURN_TYPE(ARGS...)>;
    };
}

template<typename CLOSURE>
auto wrap(const CLOSURE &fn) // EDIT: give it a better name
{ return typename detail::deduce_type<decltype(&CLOSURE::operator())>::type(fn); }


using openmpClock = OpenmpParallelClock<double, &omp_get_wtime, &omp_get_thread_num>;


class ClockArrayTest : public ::testing::Test {
protected:
    openmpClock clk;
};

TEST_F(ClockArrayTest, test_parralel_time_all_threads) {
    
    auto t1 = omp_get_wtime();
    {
#pragma omp parallel for shared(sleep_sec) num_threads(omp_get_max_threads())
        for (int i = 0; i < omp_get_num_threads() * iteartions; ++i) {
            clk.tik();
            Msleep(sleep_sec);
            clk.tak();
        }
    }
    auto t2 = omp_get_wtime();
    ASSERT_NEAR(t2 - t1, clk.getTime(), tolerance);
}

TEST_F(ClockArrayTest, test_parralel_time_4_threads) {
    auto t1 = omp_get_wtime();
    {
#pragma omp parallel for shared(sleep_sec) num_threads(8)
        for (int i = 0; i < omp_get_num_threads() * iteartions; ++i) {
            clk.tik();
            Msleep(sleep_sec);
            clk.tak();
        }
    }
    auto t2 = omp_get_wtime();
    ASSERT_NEAR(t2 - t1, clk.getTime(), tolerance);
}

TEST_F(ClockArrayTest, test_parralel_time_single_thread) {
    auto t1 = omp_get_wtime();
    {
        for (int i = 0; i < omp_get_num_threads() * iteartions; ++i) {
            clk.tik();
            Msleep(sleep_sec);
            clk.tak();
        }
    }
    auto t2 = omp_get_wtime();
    ASSERT_NEAR(t2 - t1, clk.getTime(), tolerance);

}


TEST_F(ClockArrayTest, test_parralel_time_one_more_thread) {

    auto t1 = omp_get_wtime();
    {
#pragma omp parallel for shared(sleep_sec) num_threads(omp_get_max_threads())
        for (int i = 0; i < omp_get_num_threads() * iteartions; ++i) {
            clk.tik();
            Msleep(sleep_sec);
            if (omp_get_thread_num() == 0) {
                Msleep(sleep_sec);
            }
            clk.tak();
        }
    }
    auto t2 = omp_get_wtime();
    auto tt = clk.aggregate(max<double>.value, max<double>.f);
    ASSERT_NEAR(t2 - t1, tt, tolerance);
}

