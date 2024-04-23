#include "../application/lib.h"

#include <boost/test/unit_test.hpp>
#include "../application/printUtils.h"
#include "../application/OpenmpParallelClock.h"
#include <iomanip>
#include <omp.h>
#define BOOST_TEST_MODULE printUtils
BOOST_AUTO_TEST_SUITE( clockTest )

BOOST_AUTO_TEST_CASE( test_parralel_time_all )
{
    double sleep_sec=2;
    size_t iteartions=2;
    OpenmpParallelClock clock1;
    {
        #pragma omp parallel for shared(sleep_sec) num_threads(omp_get_max_threads())
        for (int i = 0; i < omp_get_num_threads()*iteartions; ++i) {
                clock1.tik();
                sleep(sleep_sec);
                clock1.tak();
            }
    }
    BOOST_TEST(sleep_sec*iteartions,clock1.getTime());
}


BOOST_AUTO_TEST_CASE( test_parralel_time_not_all )
    {
        double sleep_sec=2;
        size_t iteartions=2;
        OpenmpParallelClock clock1;
        {
#pragma omp parallel for shared(sleep_sec) num_threads(8)//todo data test
            for (int i = 0; i < omp_get_num_threads()*iteartions; ++i) {
                clock1.tik();
                sleep(sleep_sec);
                clock1.tak();
            }
        }
        BOOST_TEST(sleep_sec*iteartions,clock1.getTime());
    }

    BOOST_AUTO_TEST_CASE( test_parralel_time_single )
    {
        double sleep_sec=2;
        size_t iteartions=2;
        OpenmpParallelClock clock1;
        {
            for (int i = 0; i < omp_get_num_threads()*iteartions; ++i) {
                clock1.tik();
                sleep(sleep_sec);
                clock1.tak();
            }
        }
        BOOST_TEST(sleep_sec*iteartions,clock1.getTime());
    }


BOOST_AUTO_TEST_SUITE_END()