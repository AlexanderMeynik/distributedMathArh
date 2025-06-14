#include "testingUtils/GoogleCommon.h"

#include "math_core/Dipoles.h"
#include "common/Generator.h"
#include "common/typeCasts.h"

using namespace test_common;
using common_types::EigenVec;
template<typename T>
using DynEigenVec = Eigen::Vector<T, -1>;
static inline FloatType aRange = 1e-6;
static inline auto normal_gen = generators::get_normal_generator(0.0, aRange * sqrt(2));

class IsSymmetricTestSuite : public testing::TestWithParam<int> {
};

TEST_P(IsSymmetricTestSuite, CheckGenratedMatrixes) {
  const int kSize = 2;

  Eigen::MatrixXd mat = Eigen::MatrixXd::Random(kSize, kSize);

  Eigen::MatrixXd sym_mat = (mat + mat.transpose()) / 2;

  ASSERT_TRUE(dipoles::IsSymmetric(sym_mat));
}

TEST_F(IsSymmetricTestSuite, CheckNonSymmetricMatrix) {
  const int kSize = 4;

  Eigen::MatrixXd mat = Eigen::MatrixXd::Random(kSize, kSize);
  mat(0, kSize - 1) = -2;
  mat(kSize - 1, 0) = 2;

  ASSERT_FALSE(dipoles::IsSymmetric(mat));
}

INSTANTIATE_TEST_SUITE_P(Matrixes, IsSymmetricTestSuite, testing::Values(2, 4, 10, 100, 200, 400, 800),
                         testing::PrintToStringParamName());

TEST(Dipoles, test_solve_result_in_zero_nev) {
  const int kN = 2;

  DynEigenVec<FloatType> coord(kN * 2);
  std::generate(std::begin(coord), std::end(coord), normal_gen);

  dipoles::Dipoles dipolearr(coord);
  auto solution = dipolearr.Solve<EigenVec>();

  EXPECT_TRUE(solution.size() == 4 * kN);

  Eigen::Vector<FloatType, Eigen::Dynamic> nev = dipolearr.GetMatrixx() * solution - dipolearr.GetRightPart();
  auto nev_norm = nev.norm();
  {
    EXPECT_NEAR(nev_norm, 0, 10e-4);
  }

}

class DipoleSolveMethodNevTests : public testing::TestWithParam<std::tuple<int, int>> {
};

INSTANTIATE_TEST_SUITE_P(DipoleNevs, DipoleSolveMethodNevTests,
                         testing::Combine(
                             testing::Values(1, 2, 3, 4, 5, 10, 20, 40, 80, 160, 200),
                             testing::Range(0, 10)
                         ),
                         TupleToString<DipoleSolveMethodNevTests>);

TEST_P(DipoleSolveMethodNevTests, test_right_part_nev_solve_impl) {

  auto [n, i] = GetParam();
  SCOPED_TRACE("Perform comparison test for N = " + std::to_string(n) + " attempt №" + std::to_string(i));

  DynEigenVec<FloatType> coord(n * 2);
  std::generate(std::begin(coord), std::end(coord), normal_gen);

  dipoles::Dipoles dipolearr(coord);
  auto rsol = dipolearr.Solve<EigenVec>();
  Eigen::Vector<FloatType, Eigen::Dynamic> nev = dipolearr.GetMatrixx() * rsol - dipolearr.GetRightPart();
  FloatType nev_norm = nev.norm();
  {
    EXPECT_NEAR(nev_norm, 0, 10e-4);
  }

}


