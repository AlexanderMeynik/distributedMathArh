#include "../application/commonFunctions/lib.h"
#include <gtest/gtest.h>
//https://stackoverflow.com/questions/6759560/boosttest-and-mocking-framework

TEST(integarion_test,test_integr)
{
    auto res= integrate<double>([](double x)->double{return 2*x;},0,2);
    ASSERT_EQ(res,4);
}
TEST(integarion_test,test_integr2)
{
    auto res= integrate<double>([](double x)->double{return 2*x;},0,1);
    ASSERT_EQ(res,1);
}

TEST(integarion_test,test_integr3)
{
    auto res= integrate<double>([](double x)->double{return 2*x;},0,3);
    ASSERT_EQ(res,9);
}
