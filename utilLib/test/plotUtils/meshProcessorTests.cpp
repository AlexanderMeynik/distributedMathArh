#include <tuple>
#include <limits>

#include "plotUtils/MeshCreator.h"
#include "common/commonTypes.h"
#include "../../../computationalNode/test/GoogleCommon.h"

#include <gtest/gtest.h>


using namespace testCommon;

namespace co = commonTypes;


class MeshCommonFunctionsTests : public ::testing::Test {
protected:
    static inline double abs = std::numeric_limits<double>::epsilon();
    static inline FloatType anotherErr=1e-14;
};

using tType = std::tuple<std::string, double, double, size_t, std::valarray<double>>;
using tType1 = std::tuple<std::string, std::function<double(double)>, double, double, double>;
using tType2 = std::tuple<std::string, co::integrableFunction, double, double, co::directionGraph>;
using tType3 = std::tuple<std::string, std::vector<FloatType>, std::vector<FloatType>, std::array<std::valarray<FloatType>,2>>;
using tType4 = std::tuple<std::string, std::array<std::valarray<FloatType>,2>,
        co::directionGraph,std::valarray<FloatType>>;
co::directionGraph dummy=[](double a,double b){return 0.0;};
using namespace meshStorage;



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

TEST_P(TestsIntegrate, testIntegrateZeroSpan)
{
    auto [_, function, l, r, result] = GetParam();

    auto res = integrate(function, 0.0, 0.0);

    ASSERT_DOUBLE_EQ(res, 0.0);
}



TEST_P(TestsIntegrate, testIntegrate) {
    auto [_, function, l, r, result] = GetParam();

    auto res = integrate(function, l, r);
    auto res2= newintegrate(function,l,r);

    ASSERT_DOUBLE_EQ(res, result);
    ASSERT_DOUBLE_EQ(res2, result);//todo error at lasts test(for infinite range)
    //todo dots to rulenumconversion.
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

    compareArrays(g1, g2, arrayDoubleComparator, anotherErr);

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
               ), firstValueTuplePrinter<TestsIntegrateOneVar>);





TEST_P(TestLinspace, linspaceValueTest) {
    auto [_, start, end, n, result] = GetParam();

    auto r = ::myLinspace(start, end, n);
    compareArrays(r, result, arrayDoubleComparator, anotherErr);

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

    compareArrays(grid[0], res[0], arrayDoubleComparator, anotherErr);
    compareArrays(grid[1], res[1], arrayDoubleComparator, anotherErr);

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


TEST_F(TestFunctionApply, testZeroContainerSize)
{
    auto a=std::valarray<FloatType>{};
    ASSERT_THROW(computeFunction(a,a,dummy),std::length_error);
}

TEST_F(TestFunctionApply, testMismathcedContainerSizez)
{
    auto a=std::valarray<FloatType>{};
    auto b=std::valarray<FloatType>{1,2,3};
    ASSERT_THROW(computeFunction(a,a,dummy),std::length_error);
}

TEST_P(TestFunctionApply, TestFunctionApply) {
    auto [_, mesh, func, res] = GetParam();

    auto r= computeFunction(mesh[0],mesh[1],func);

    compareArrays(r, res, arrayDoubleComparator, anotherErr);

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





//todo test mesh processor
    //use fake function framework to test function invocations
    //compare calculated plot views