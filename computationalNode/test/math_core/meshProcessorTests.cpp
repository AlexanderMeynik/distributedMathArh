#include <chrono>
#include <tuple>
#include <limits>

#include "common/lib.h"
#include "computationalLib/math_core/MeshProcessor2.h"
#include "../GoogleCommon.h"

#include <algorithm>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace testCommon;




class MeshCommonFunctionsTests : public ::testing::Test {
protected:
    static inline double abs = std::numeric_limits<double>::epsilon();
    static inline FloatType anotherErr=1e-14;
};

using tType = std::tuple<std::string, double, double, size_t, std::valarray<double>>;
using tType1 = std::tuple<std::string, std::function<double(double)>, double, double, double>;
using tType2 = std::tuple<std::string, dipoles::integrableFunction, double, double, dipoles::directionGraph>;
using tType3 = std::tuple<std::string, std::vector<FloatType>, std::vector<FloatType>, std::array<std::valarray<FloatType>,2>>;
using tType4 = std::tuple<std::string, std::array<std::valarray<FloatType>,2>,
std::function<FloatType(FloatType,FloatType)>,std::valarray<FloatType>>;

using namespace meshStorage;


using MyTypes = ::testing::Types<std::valarray<FloatType>, int, unsigned int>;


/*template<template<typename ...> typename container>*/
class TestLinspace :
        public MeshCommonFunctionsTests,
        public ::testing::WithParamInterface<tType> {
};

class TestsIntegrate :
        public MeshCommonFunctionsTests,
        public ::testing::WithParamInterface<tType1> {
};

class TestsIntegrateOneVar :
        public MeshCommonFunctionsTests,
        public ::testing::WithParamInterface<tType2> {
};
class TestMeshGenerate :
        public MeshCommonFunctionsTests,
        public ::testing::WithParamInterface<tType3> {
};

class TestFunctionApply :
        public MeshCommonFunctionsTests,
        public ::testing::WithParamInterface<tType4> {
};

TEST_P(TestsIntegrate, testIntegrate) {
    auto [_, function, l, r, result] = GetParam();

    auto res = integrate(function, l, r);

    ASSERT_DOUBLE_EQ(res, result);

}

INSTANTIATE_TEST_SUITE_P(
        integrateTests,
        TestsIntegrate,
        ::testing::Values(
                std::make_tuple("parabola", [](double x) { return x * x; }, 0, 2, 8 / 3.0),
                std::make_tuple("sin", [](double x) { return sin(x); }, 0, 2, -cos(2) + cos(0)),
                std::make_tuple("dual_limits", [](double x) { return x; }, -2, 2, 0),
                std::make_tuple("GausIntegral", [](double x) { return exp(-x * x); },
                                -std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity(),
                                sqrt(M_PI))
        ), firstValueTuplePrinter<TestsIntegrate>);


TEST_P(TestsIntegrateOneVar, testIntegrateLambdaForOneVariable) {
    auto [_, function, l, r, resultFunction] = GetParam();

    auto res = integrateLambdaForOneVariable(function, 0, 0, l, r);
    std::vector<double> ss = myLinspace(0.0, 10.0, 10);

    std::valarray<FloatType> a = myLinspace<std::valarray>(0.0, 10.0, 2);
    std::valarray<FloatType> b = myLinspace<std::valarray>(-5.0, 5.0, 2);
    auto grid = myMeshGrid(a, b);
    auto g1 = meshStorage::computeFunction(grid[0], grid[1], [&function, &l, r](FloatType x, FloatType y) {
        return meshStorage::integrateLambdaForOneVariable<61>(function, y, x, l, r);
    });
    auto g2 = meshStorage::computeFunction(grid[0], grid[1], resultFunction);

    compareArrays(g1, g2, double_comparator2,anotherErr);//todo variant with another function

}

INSTANTIATE_TEST_SUITE_P(
        integrateLabmdaTests,
        TestsIntegrateOneVar,
        ::testing::Values(
                std::make_tuple("zero", [](double x, double y, double z) { return z * z; }, 0, 2,
                                [](double x, double y) { return 8 / 3.0; }),
                std::make_tuple("parabola", [](double x, double y, double z) { return x + y + z * z; }, 0, 2,
                                [](double x, double y) { return 2 * x + 2 * y + 8 / 3.0; }),
                std::make_tuple("non_divisible_z_function", [](double x, double y, double z) { return x*z+y*z; }, 0, 2,
                                [](double x, double y) { return x*2+y*2; })
                /*std::make_tuple("non_divisible_z_function", [](double x, double y, double z) { return x*cos(z)+y*sin(z); }, 0, 2,
                                [](double x, double y) { return x*(sin(2)-sin(0))+y*(-cos(2)+cos(0)); })*/
                /*todo add more*/), firstValueTuplePrinter<TestsIntegrateOneVar>);





TEST_P(TestLinspace, linspaceValueTest) {
    auto [_, start, end, n, result] = GetParam();

    auto r = ::myLinspace(start, end, n);
    compareArrays(r, result, double_comparator2,anotherErr);

}

INSTANTIATE_TEST_SUITE_P(
        linspaceTests,
        TestLinspace,
        ::testing::Values(
                std::make_tuple("simple_range", 0., 1., 2UL, std::valarray{0., 1.}),
                std::make_tuple("negative_range", -2.0, -1., 2UL, std::valarray{-2., -1.}),
                std::make_tuple("large_range", 0., 20., 11UL, std::valarray{0., 2.,
                                                                            4.,6.,8.,10.,12.,14.,16.,18.,20.})
        ), firstValueTuplePrinter<TestLinspace>);


TEST_P(TestMeshGenerate, TestMeshGenerate) {
    auto [_, a, b, res] = GetParam();

    auto grid= myMeshGrid(a,b);

    compareArrays(grid[0],res[0],double_comparator2,anotherErr);
    compareArrays(grid[1],res[1],double_comparator2,anotherErr);

}

INSTANTIATE_TEST_SUITE_P(
        MeshGenerate,
        TestMeshGenerate,
        ::testing::Values(
                std::make_tuple("simple_range", std::vector<double>{0,1},std::vector<double>{1,2},
                                std::array{std::valarray<double>{0,1,0,1},std::valarray<double>{1,1,2,2}}),
                std::make_tuple("different_ranges", std::vector<double>{0,1},std::vector<double>{1,2,3},
                                std::array{std::valarray<double>{0,1,0,1,0,1},std::valarray<double>{1,1,2,2,3,3}}),
                std::make_tuple("larger_first_range", std::vector<double>{-1,0,1,2},std::vector<double>{1,2,3},
                                std::array{std::valarray<double>{-1,0,1,2,-1,0,1,2,-1,0,1,2},std::valarray<double>{1,1,1,1,2,2,2,2,3,3,3,3}})
                ), firstValueTuplePrinter<TestMeshGenerate>);



TEST_P(TestFunctionApply, TestFunctionApply) {
    auto [_, mesh, func, res] = GetParam();

    auto r= computeFunction(mesh[0],mesh[1],func);

    compareArrays(r,res,double_comparator2,anotherErr);

}

INSTANTIATE_TEST_SUITE_P(
        TestFunctionApplyTest,
        TestFunctionApply,
        ::testing::Values(
                std::make_tuple("simple_range",
                                std::array{std::valarray<FloatType>{0,1,0,1},std::valarray<FloatType>{1,1,2,2}},
                                [](FloatType x,FloatType y){return x+y;},
                                std::valarray<FloatType >{1,2,2,3}),
                std::make_tuple("nonLinearFunction",
                                std::array{std::valarray<FloatType>{-1,1,-1,1},std::valarray<FloatType>{1,1,2,2}},
                                [](FloatType x,FloatType y){return x*x+y*y*y;},
                                std::valarray<FloatType >{2,2,9,9})
        ), firstValueTuplePrinter<TestFunctionApply>);

//todo tests for size =0;
//typed tests for different containers and value of end









//test mesh processor
//todo test that integral is zero for (0,0)- same for integrate with one