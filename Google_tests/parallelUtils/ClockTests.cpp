
#include <chrono>



#include "../../application/common/lib.h"

#include "../../application/common/printUtils.h"
#include "../../application/parallelUtils/OpenmpParallelClock.h"
#include <iomanip>
#include <omp.h>
#include <chrono>
#include <thread>
#include "../GoogleCommon.h"
#include <gtest/gtest.h>
constexpr double tolerance=0.1;
using namespace timing;
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

    namespace detail // EDIT: make it an implementation detail
    {
        template < typename T > struct deduce_type ;

        template < typename RETURN_TYPE, typename CLASS_TYPE, typename... ARGS >
        struct deduce_type< RETURN_TYPE(CLASS_TYPE::*)(ARGS...) const >
        { using type = std::function< RETURN_TYPE(ARGS...) > ; };
    }

    template < typename CLOSURE > auto wrap( const CLOSURE& fn ) // EDIT: give it a better name
    { return typename detail::deduce_type< decltype( &CLOSURE::operator() ) >::type(fn) ; }


using openmpClock=OpenmpParallelClock<double,&omp_get_wtime,&omp_get_thread_num>;

TEST(OpenmpParallelClock,test_parralel_time_all_threads)
{
    openmpClock clock1;
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
    ASSERT_NEAR(t2-t1,clock1.getTime(),tolerance);
}

TEST(OpenmpParallelClock,test_parralel_time_4_threads)
{
    openmpClock clock1;
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
    ASSERT_NEAR(t2-t1,clock1.getTime(),tolerance);
}

TEST(OpenmpParallelClock,test_parralel_time_single_thread)
{
    openmpClock clock1;
    auto t1=omp_get_wtime();
    {
        for (int i = 0; i < omp_get_num_threads()*iteartions; ++i) {
            clock1.tik();
            Msleep(sleep_sec);
            clock1.tak();
        }
    }
    auto t2=omp_get_wtime();
    ASSERT_NEAR(t2-t1,clock1.getTime(),tolerance);

}




TEST(OpenmpParallelClock,test_parralel_time_one_more_thread)
{

    openmpClock clock1;
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
    auto tt=clock1.aggregate(max<double>.value,max<double>.f);
    ASSERT_NEAR(t2-t1,tt,tolerance);
}

