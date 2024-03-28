#include "../application/lib.h"
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN  // in only one cpp file
#include <boost/test/unit_test.hpp>
//https://stackoverflow.com/questions/6759560/boosttest-and-mocking-framework

BOOST_AUTO_TEST_CASE( free_test_function )
/* Compare with void free_test_function() */
{
    auto res= integrate<double>([](double x)->double{return 2*x;},0,2);
    BOOST_TEST(res==4);
}