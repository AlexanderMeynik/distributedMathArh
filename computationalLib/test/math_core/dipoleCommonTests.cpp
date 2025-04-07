#include "../GoogleCommon.h"

#include "computationalLib/math_core/Dipoles.h"
#include "common/Generator.h"
#include "common/typeCasts.h"

using namespace testCommon;
using commonTypes::EigenVec;
template<typename T>
using dynEigenVec = Eigen::Vector<T, -1>;
static inline FloatType aRange = 1e-6;


class IsSymmetricTestSuite : public testing::TestWithParam<int> {
};

TEST_P(IsSymmetricTestSuite, CheckGenratedMatrixes) {
    const int size = 2;

    Eigen::MatrixXd mat = Eigen::MatrixXd::Random(size, size);

    Eigen::MatrixXd symMat = (mat + mat.transpose()) / 2;

    ASSERT_TRUE(dipoles::isSymmetric(symMat));
}

INSTANTIATE_TEST_SUITE_P(Matrixes, IsSymmetricTestSuite, testing::Values(2, 4, 10, 100, 200, 400, 800),
                         testing::PrintToStringParamName());


TEST(Dipoles, test_solve_result_in_zero_nev) {
    const int N = 2;

    auto coord = generators::normal<dynEigenVec>(N, 0.0, aRange * sqrt(2));
    dipoles::Dipoles dipolearr(coord);
    auto solution = dipolearr.solve<EigenVec>();

    EXPECT_TRUE(solution.size() == 4 * N);

    Eigen::Vector<FloatType, Eigen::Dynamic> nev = dipolearr.getMatrixx() * solution - dipolearr.getRightPart();
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
                         tupleToString<DipoleSolveMethodNevTests>);

TEST_P(DipoleSolveMethodNevTests, test_right_part_nev_solve_impl) {

    auto [N, i] = GetParam();
    SCOPED_TRACE("Perform comparison test for N = " + std::to_string(N) + " attempt №" + std::to_string(i));


    auto coord = generators::normal<dynEigenVec>(N, 0.0, aRange * sqrt(2));
    dipoles::Dipoles dipolearr(coord);
    auto rsol = dipolearr.solve<EigenVec>();
    Eigen::Vector<FloatType, Eigen::Dynamic> nev = dipolearr.getMatrixx() * rsol - dipolearr.getRightPart();
    FloatType nev_norm = nev.norm();
    {
        EXPECT_NEAR(nev_norm, 0, 10e-4);
    }

}
