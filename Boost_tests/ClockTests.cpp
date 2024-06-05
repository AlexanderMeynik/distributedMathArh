#include "../application/common/lib.h"

#include <boost/test/unit_test.hpp>
#include "../application/common/printUtils.h"
#include "../application/parallelUtils/OpenmpParallelClock.h"
#include <iomanip>
#include <omp.h>
#define BOOST_TEST_MODULE printUtils
#include <chrono>
#include <thread>
BOOST_AUTO_TEST_SUITE( clockTest )

void nanoSleep(int num)
{
    std::this_thread::sleep_for(std::chrono::nanoseconds(num));
}
void Msleep(double seconds)
{
    nanoSleep((int)(1e9*seconds));
}

constexpr double sleep_sec=0.1;
constexpr size_t iteartions=12;
constexpr double tolerance=1;

    namespace detail // EDIT: make it an implementation detail
    {
        template < typename T > struct deduce_type ;

        template < typename RETURN_TYPE, typename CLASS_TYPE, typename... ARGS >
        struct deduce_type< RETURN_TYPE(CLASS_TYPE::*)(ARGS...) const >
        { using type = std::function< RETURN_TYPE(ARGS...) > ; };
    }

    template < typename CLOSURE > auto wrap( const CLOSURE& fn ) // EDIT: give it a better name
    { return typename detail::deduce_type< decltype( &CLOSURE::operator() ) >::type(fn) ; }



BOOST_AUTO_TEST_CASE( test_parralel_time_all )
{

    OpenmpParallelClock clock1;
    auto t1=omp_get_wtime();
    {
        #pragma omp parallel for shared(sleep_sec) num_threads(omp_get_max_threads())
        for (int i = 0; i < omp_get_num_threads()*iteartions; ++i) {
                clock1.tik();
                Msleep(sleep_sec);
                clock1.tak();
            }
    }
    auto t2=omp_get_wtime();
    BOOST_CHECK_CLOSE(t2-t1,clock1.getTime(),tolerance);
}


BOOST_AUTO_TEST_CASE( test_parralel_time_not_all )
    {
        OpenmpParallelClock clock1;
        auto t1=omp_get_wtime();
        {
#pragma omp parallel for shared(sleep_sec) num_threads(8)
            for (int i = 0; i < omp_get_num_threads()*iteartions; ++i) {
                clock1.tik();
                Msleep(sleep_sec);
                clock1.tak();
            }
        }
        auto t2=omp_get_wtime();
        BOOST_CHECK_CLOSE(t2-t1,clock1.getTime(),tolerance);
    }

    BOOST_AUTO_TEST_CASE( test_parralel_time_single )
    {
        OpenmpParallelClock clock1;
        auto t1=omp_get_wtime();
        {
            for (int i = 0; i < omp_get_num_threads()*iteartions; ++i) {
                clock1.tik();
                Msleep(sleep_sec);
                clock1.tak();
            }
        }
        auto t2=omp_get_wtime();
        BOOST_CHECK_CLOSE(t2-t1,clock1.getTime(),tolerance);
    }


    BOOST_AUTO_TEST_CASE( test_parralel_one_thread_more )
    {

        OpenmpParallelClock clock1;
        auto t1=omp_get_wtime();
        {
#pragma omp parallel for shared(sleep_sec) num_threads(omp_get_max_threads())
            for (int i = 0; i < omp_get_num_threads()*iteartions; ++i) {
                clock1.tik();
                Msleep(sleep_sec);
                if(omp_get_thread_num()==0)
                {
                    Msleep(sleep_sec);
                }
                clock1.tak();
            }
        }
        auto t2=omp_get_wtime();
        auto tt=clock1.aggregate<double,-1.0>(wrap([](double a, double b){ return std::max(a,b);}));
        BOOST_CHECK_CLOSE(t2-t1,tt,tolerance);
    }

BOOST_AUTO_TEST_SUITE_END()