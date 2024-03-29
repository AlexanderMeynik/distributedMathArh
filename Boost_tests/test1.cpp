#include "../application/lib.h"
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN  // in only one cpp file
#include <boost/test/unit_test.hpp>
#include "../application/MeshProcessor.h"
#include <algorithm>
//https://stackoverflow.com/questions/6759560/boosttest-and-mocking-framework

BOOST_AUTO_TEST_CASE( free_test_function )
/* Compare with void free_test_function() */
{
    auto res= integrate<double>([](double x)->double{return 2*x;},0,2);
    BOOST_TEST(res==4);
}


BOOST_AUTO_TEST_CASE( test_linpace )
/* Compare with void free_test_function() */
{
    auto res1=matplot::linspace(0,10,20);
    auto my_res1= myLinspace<double>(0.0,10.0,20);

    BOOST_TEST(res1==my_res1);
    //BOOST_CHECK_EQUAL_COLLECTIONS(res1.begin(),res1.end(),res2.b)
}

BOOST_AUTO_TEST_CASE( test_meshfunc )
/* Compare with void free_test_function() */
{
    auto res1=matplot::meshgrid(matplot::linspace(1.0,10.0,2),matplot::linspace(0.0,10.0,3));
    auto my_res1= mymeshGrid(matplot::linspace(1.0,10.0,2),matplot::linspace(0.0,10.0,3));
    for (int i = 0; i < res1.first.size(); ++i) {
        BOOST_CHECK_EQUAL_COLLECTIONS(res1.first[i].begin(),res1.first[i].end(),my_res1.first[i].begin(),my_res1.first[i].end());
    }
    for (int i = 0; i < res1.second.size(); ++i) {
        BOOST_CHECK_EQUAL_COLLECTIONS(res1.second[i].begin(),res1.second[i].end(),my_res1.second[i].begin(),my_res1.second[i].end());

    }

    //BOOST_CHECK_EQUAL_COLLECTIONS(res1.begin(),res1.end(),res2.b)
}