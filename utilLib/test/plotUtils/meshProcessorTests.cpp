#include <tuple>
#include <limits>

#include "common/MeshCreator.h"
#include "common/commonTypes.h"
#include "testingUtils/GoogleCommon.h"

#include <gtest/gtest.h>

using namespace test_common;

namespace co = common_types;

class MeshCommonFunctionsTests : public ::testing::Test {
 protected:
  static inline double abs_ = std::numeric_limits<double>::epsilon();
  static inline FloatType another_err_ = 1e-14;
};

using TType = std::tuple<std::string, double, double, size_t, std::valarray<double>>;
using TType1 = std::tuple<std::string, std::function<double(double)>, double, double, double>;
using TType2 = std::tuple<std::string, co::IntegrableFunction, double, double, co::DirectionGraph>;
using TType3 = std::tuple<std::string,
                          std::vector<FloatType>,
                          std::vector<FloatType>,
                          std::array<std::valarray<FloatType>, 2>>;
using TType4 = std::tuple<std::string, std::array<std::valarray<FloatType>, 2>,
                          co::DirectionGraph, std::valarray<FloatType>>;
co::DirectionGraph dummy = [](double a, double b) { return 0.0; };
using namespace mesh_storage;

class TestLinspace :
    public MeshCommonFunctionsTests,
    public ::testing::WithParamInterface<TType> {
};

class TestsIntegrate :
    public MeshCommonFunctionsTests,
    public ::testing::WithParamInterface<TType1> {
};

class TestsIntegrateOneVar :
    public MeshCommonFunctionsTests,
    public ::testing::WithParamInterface<TType2> {
};

class TestMeshGenerate :
    public MeshCommonFunctionsTests,
    public ::testing::WithParamInterface<TType3> {
};

class TestFunctionApply :
    public MeshCommonFunctionsTests,
    public ::testing::WithParamInterface<TType4> {
};

TEST_P(TestsIntegrate, testIntegrateZeroSpan) {
  auto [_, function, l, r, result] = GetParam();

  auto res = Integrate(function, 0.0, 0.0);

  ASSERT_DOUBLE_EQ(res, 0.0);
}

TEST_P(TestsIntegrate, testIntegrate) {
  auto [_, function, l, r, result] = GetParam();

  auto res = Integrate(function, l, r);

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
                        std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max(),
                        sqrt(M_PI))
    ), FirstValueTuplePrinter<TestsIntegrate>);

TEST_P(TestsIntegrateOneVar, testIntegrateLambdaForOneVariable) {
  auto [_, function, l, r, result_function] = GetParam();

  auto res = IntegrateLambdaForOneVariable(function, 0, 0, l, r);
  std::vector<double> ss = MyLinspace(0.0, 10.0, 10);

  std::valarray<FloatType> a = MyLinspace<std::valarray>(0.0, 10.0, 2);
  std::valarray<FloatType> b = MyLinspace<std::valarray>(-5.0, 5.0, 2);
  auto grid = MyMeshGrid(a, b);
  auto g1 = mesh_storage::ComputeFunction(grid[0], grid[1], [&function, &l, r](FloatType x, FloatType y) {
    return mesh_storage::IntegrateLambdaForOneVariable<61>(function, y, x, l, r);
  });
  auto g2 = mesh_storage::ComputeFunction(grid[0], grid[1], result_function);

  CompareArrays(g1, g2, arrayDoubleComparator<FloatType>::call, another_err_);

}

INSTANTIATE_TEST_SUITE_P(
    integrateLabmdaTests,
    TestsIntegrateOneVar,
    ::testing::Values(
        std::make_tuple("zero", [](double x, double y, double z) { return z * z; }, 0, 2,
                        [](double x, double y) { return 8 / 3.0; }),
        std::make_tuple("parabola", [](double x, double y, double z) { return x + y + z * z; }, 0, 2,
                        [](double x, double y) { return 2 * x + 2 * y + 8 / 3.0; }),
        std::make_tuple("non_divisible_z_function", [](double x, double y, double z) { return x * z + y * z; },
                        0, 2,
                        [](double x, double y) { return x * 2 + y * 2; })
    ), FirstValueTuplePrinter<TestsIntegrateOneVar>);

TEST_P(TestLinspace, linspaceValueTest) {
  auto [_, start, end, n, result] = GetParam();

  auto r = ::MyLinspace(start, end, n);
  CompareArrays(r, result, arrayDoubleComparator<FloatType>::call, another_err_);

}

INSTANTIATE_TEST_SUITE_P(
    linspaceTests,
    TestLinspace,
    ::testing::Values(
        std::make_tuple("simple_range", 0., 1., 2UL, std::valarray{0., 1.}),
        std::make_tuple("negative_range", -2.0, -1., 2UL, std::valarray{-2., -1.}),
        std::make_tuple("large_range", 0., 20., 11UL, std::valarray{0., 2.,
                                                                    4., 6., 8., 10., 12., 14., 16., 18., 20.})
    ), FirstValueTuplePrinter<TestLinspace>);

TEST_P(TestMeshGenerate, TestMeshGenerate) {
  auto [_, a, b, res] = GetParam();

  auto grid = MyMeshGrid(a, b);

  CompareArrays(grid[0], res[0], arrayDoubleComparator<FloatType>::call, another_err_);
  CompareArrays(grid[1], res[1], arrayDoubleComparator<FloatType>::call, another_err_);

}

INSTANTIATE_TEST_SUITE_P(
    MeshGenerate,
    TestMeshGenerate,
    ::testing::Values(
        std::make_tuple("simple_range", std::vector<double>{0, 1}, std::vector<double>{1, 2},
                        std::array{std::valarray<double>{0, 1, 0, 1}, std::valarray<double>{1, 1, 2, 2}}),
        std::make_tuple("different_ranges", std::vector<double>{0, 1}, std::vector<double>{1, 2, 3},
                        std::array{std::valarray<double>{0, 1, 0, 1, 0, 1},
                                   std::valarray<double>{1, 1, 2, 2, 3, 3}}),
        std::make_tuple("larger_first_range", std::vector<double>{-1, 0, 1, 2}, std::vector<double>{1, 2, 3},
                        std::array{std::valarray<double>{-1, 0, 1, 2, -1, 0, 1, 2, -1, 0, 1, 2},
                                   std::valarray<double>{1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3}})
    ), FirstValueTuplePrinter<TestMeshGenerate>);

TEST_F(TestFunctionApply, testZeroContainerSize) {
  auto a = std::valarray<FloatType>{};
  ASSERT_THROW(ComputeFunction(a, a, dummy), shared::MyException);
}

TEST_F(TestFunctionApply, testMismathcedContainerSizez) {
  auto a = std::valarray<FloatType>{};
  auto b = std::valarray<FloatType>{1, 2, 3};
  ASSERT_THROW(ComputeFunction(a, a, dummy), shared::MyException);
}

TEST_P(TestFunctionApply, TestFunctionApply) {
  auto [_, mesh, func, res] = GetParam();

  auto r = ComputeFunction(mesh[0], mesh[1], func);

  CompareArrays(r, res, arrayDoubleComparator<FloatType>::call, another_err_);

}

INSTANTIATE_TEST_SUITE_P(
    TestFunctionApplyTest,
    TestFunctionApply,
    ::testing::Values(
        std::make_tuple("simple_range",
                        std::array{std::valarray<FloatType>{0, 1, 0, 1}, std::valarray<FloatType>{1, 1, 2, 2}},
                        [](FloatType x, FloatType y) { return x + y; },
                        std::valarray<FloatType>{1, 2, 2, 3}),
        std::make_tuple("nonLinearFunction",
                        std::array{std::valarray<FloatType>{-1, 1, -1, 1},
                                   std::valarray<FloatType>{1, 1, 2, 2}},
                        [](FloatType x, FloatType y) { return x * x + y * y * y; },
                        std::valarray<FloatType>{2, 2, 9, 9})
    ), FirstValueTuplePrinter<TestFunctionApply>);
