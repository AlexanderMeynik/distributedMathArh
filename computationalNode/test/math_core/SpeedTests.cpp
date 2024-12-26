
#include <chrono>


#include "common/lib.h"
#include "computationalLib/math_core/Dipoles.h"
#include "computationalLib/math_core/MeshCreator.h"
#include "iolib/Parsers.h"
#include "common/Generator.h"
#include "../GoogleCommon.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>


using namespace commonDeclarations;
using namespace testCommon;
using namespace Eigen;


static inline double C1 = 0;
static inline double C2 = 0;


template<typename T>
using dynEigenVec=Eigen::Vector<T,-1>;
static inline FloatType aRange=1e-6;
TEST(transformations, reinterpret_vector_test) {
    auto N = 20;


    auto EigenVec = generators::normal<dynEigenVec>(N,0.0,aRange* sqrt(2));
    auto arr2vec = reinterpretVector(EigenVec);
    EXPECT_TRUE(EigenVec.size() == 2 * arr2vec[0].size() && EigenVec.size() == 2 * N);
    for (int i = 0; i < N; ++i) {
        SCOPED_TRACE("Checked index " + std::to_string(i) + '\n');
        EXPECT_NEAR(EigenVec[i], arr2vec[0][i], tool);
        EXPECT_NEAR(EigenVec[i + N], arr2vec[1][i], tool);
    }

}

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

    auto coord = generators::normal<dynEigenVec>(N,0.0,aRange* sqrt(2));
    dipoles::Dipoles dipolearr(coord);
    auto solution = dipolearr.solve<dipoles::EigenVec>();

    EXPECT_TRUE(solution.size() == 4 * N);

    Eigen::Vector<FloatType,Eigen::Dynamic> nev = dipolearr.getMatrixx() * solution - dipolearr.getRightPart();
    auto nev_norm=nev.norm();
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


    auto coord = generators::normal<dynEigenVec>(N,0.0,aRange* sqrt(2));
    dipoles::Dipoles dipolearr(coord);
    auto rsol = dipolearr.solve<dipoles::EigenVec>();
    Eigen::Vector<FloatType,Eigen::Dynamic> nev = dipolearr.getMatrixx() * rsol - dipolearr.getRightPart();
    FloatType nev_norm=nev.norm();
    {
        EXPECT_NEAR(nev_norm, 0, 10e-4);
    }

}


template<typename Type>
using DynVector = Eigen::Matrix<Type, Eigen::Dynamic, 1>;
std::string res_dir_path = "../../../res/";
std::string filename = res_dir_path.append("config.txt");
string subdir = filename.substr(0, filename.rfind('.')) + "data7_25";









using coordType=std::vector<std::vector<FloatType >>;
using meshStorage::MeshCreator;
using ttype = std::tuple<std::string, std::vector<FloatType>, Parser<MatrixXd>, Parser<DynVector<FloatType>>, Parser<MeshCreator>>;

std::vector<ttype> testFixtureGetter(const std::string & file) {

    std::vector<ttype> values;
    auto avec= parseDipoleCoordinates<coordType>(file);

    std::ifstream sols(subdir + "/solutions.txt");
    std::ifstream matrixes(subdir + "/matrixes.txt");
    std::ifstream meshes(subdir + "/meshes.txt");
    values.reserve(avec.size());


    FloatType steps[2];
    meshes >> steps[0] >> steps[1];
    size_t NN;
    for (size_t i = 0; i < avec.size(); ++i) {
        matrixes >> NN;
        sols >> NN;
        meshes >> NN;

        ttype value;

        auto matr = Parser<MatrixXd>(NN);
        matrixes >> matr;

        auto solvv = Parser<DynVector<FloatType>>(NN);
        sols >> solvv;

        Parser<MeshCreator> meshh;
        meshes >> meshh;

        values.emplace_back(std::to_string(i), avec[i], matr, solvv, meshh);
    }

    sols.close();
    matrixes.close();
    meshes.close();
    return values;
}

class DipolesVerificationTS : public ::testing::TestWithParam<ttype> {
public:


protected:
};


INSTANTIATE_TEST_SUITE_P(
        ValidationTest, DipolesVerificationTS,
        ::testing::ValuesIn(testFixtureGetter(filename)),
        firstValueTuplePrinter<DipolesVerificationTS>);

TEST_P(DipolesVerificationTS, test_on_10_basik_conf_matrixes) {
    auto [nn, conf, matr, _, pp] = GetParam();


    EXPECT_EQ(matr.size_, conf.size() / 2);
    dipoles::Dipoles dd(conf);
    compare2dArrays(dd.getMatrixx(), matr.vals_, double_comparator3, 1e20 / 10000);

}

TEST_P(DipolesVerificationTS,
       test_on_10_basik_conf_solutions) {

    auto [nn, _, matr, sol, pp] = GetParam();

    dipoles::Dipoles dd;
    dd.loadFromMatrix(matr.vals_);

    auto solut = dd.solve<dipoles::EigenVec>();
    compareArrays(sol.vals_, solut, double_comparator2);
}


TEST_P(DipolesVerificationTS, test_on_10_basik_conf_meshes) {
    std::ios_base::sync_with_stdio(false);


    auto [nn, conf, mattr, sol, mesh] = GetParam();

    dipoles::Dipoles dd;
    dd.getFullFunction_(conf, sol.vals_);

    MeshCreator mm;
    mm.constructMeshes();
    mm.applyFunction(dd.getI2function());
    auto r2 = meshStorage::unflatten(mm.spans[2]/*,mm.dimensions.data()*/);

    auto ress = meshStorage::unflatten(mesh.vals_.data[2], mesh.vals_.dimensions);

    auto ll = mesh.vals_.spans[2][std::array{0, 0}];
    compare2dArrays<true>(ress, r2, double_comparator3, 1e-3);
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);


    return RUN_ALL_TESTS();
}