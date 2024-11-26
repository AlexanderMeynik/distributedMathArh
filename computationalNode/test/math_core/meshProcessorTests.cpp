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

template<typename TestSuite>
auto firstValueTuplePrinter(const testing::TestParamInfo<typename TestSuite::ParamType> &info) {
    return get<0>(info.param);
}


class MeshCommonFunctionsTests : public ::testing::Test {
protected:
    static inline double abs = std::numeric_limits<double>::epsilon();
    static inline auto double_comparator = [](double a, double b, size_t i) {
        const testing::internal::FloatingPoint<double> lhs(a), rhs(b);

        return lhs.AlmostEquals(rhs);
    };
};

using tType = std::tuple<std::string, double, double, size_t, std::valarray<double>>;
using tType1 = std::tuple<std::string, std::function<double(double)>, double, double, double>;
using tType2 = std::tuple<std::string, dipoles::integrableFunction, double, double, dipoles::directionGraph>;
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


TEST_P(TestsIntegrate, testIntegrate) {
    auto [_, function, l, r, result] = GetParam();

    auto res = integrate(function, l, r);

    ASSERT_DOUBLE_EQ(res, result);

}


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

    compareArrays(g1, g2, double_comparator);

}


TEST_P(TestLinspace, linspaceValueTest) {
    auto [_, start, end, n, result] = GetParam();

    auto r = ::myLinspace(start, end, n);
    compareArrays(r, result, double_comparator);

}

//todo tests for size =0;
//typed tests for different containers and value of end
INSTANTIATE_TEST_SUITE_P(
        linspaceTests,
        TestLinspace,
        ::testing::Values(
                std::make_tuple("simple_range", 0., 1., 2UL, std::valarray{0., 1.}),
                std::make_tuple("negative_range", -2.0, -1., 2UL, std::valarray{-2., -1.}),
                std::make_tuple("large_range", 0., 20., 11UL, std::valarray{0., 2.,
                4.,6.,8.,10.,12.,14.,16.,18.,20.})
        ), firstValueTuplePrinter<TestLinspace>);


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


INSTANTIATE_TEST_SUITE_P(
        integrateLabmdaTests,
        TestsIntegrateOneVar,
        ::testing::Values(
                std::make_tuple("zero", [](double x, double y, double z) { return z * z; }, 0, 2,
                                [](double x, double y) { return 8 / 3.0; }),
                std::make_tuple("parabola", [](double x, double y, double z) { return x + y + z * z; }, 0, 2,
                                [](double x, double y) { return 2 * x + 2 * y + 8 / 3.0; })
                /*todo add more*/), firstValueTuplePrinter<TestsIntegrateOneVar>);


//test mesh processor