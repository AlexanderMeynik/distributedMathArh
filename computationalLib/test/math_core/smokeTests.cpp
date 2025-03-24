
#include <chrono>


#include "common/sharedDeclarations.h"
#include "computationalLib/math_core/Dipoles.h"
#include "common/commonTypes.h"
#include "common/Parsers.h"
#include "common/Printers.h"
#include "../GoogleCommon.h"



using namespace pu;
using namespace testCommon;
std::string res_dir_path = "../../../res/";
std::string filename = res_dir_path.append("config.txt");
std::string subdir = filename.substr(0, filename.rfind('.')) + "data7_25";

using meshStorage::MeshCreator;
using ttype = std::tuple<std::string, std::vector<FloatType>, ct::matrixType, ct::EigenVec, MeshCreator>;



std::vector<ttype> testFixtureGetter() {

    int NN;

    std::vector<ttype> values;
    std::ifstream coords1(subdir + "/coordinates.txt");
    std::ifstream sols1(subdir + "/solutions.txt");
    std::ifstream matrixes1(subdir + "/matrixes.txt");
    std::ifstream meshes1(subdir + "/meshes.txt");
    coords1>>NN;
    sols1>>NN;
    matrixes1>>NN;
    meshes1>>NN;
    values.reserve(NN);

    ioFormat a;
    coords1>>a;
    sols1>>a;
    matrixes1>>a;
    meshes1>>a;

    EFormat ef;
    coords1>>ef;
    sols1>>ef;
    matrixes1>>ef;
    meshes1>>ef;
    for (int i = 0; i < NN; ++i) {
        auto sol=printUtils::parseOneDim<ct::EigenVec>(sols1);
        auto coords=printUtils::parseOneDim<ct::stdVec>(coords1);

        auto matr=printUtils::parseMatrix(matrixes1);
        auto m=printUtils::parseMeshFrom(meshes1);
        values.emplace_back(std::to_string(i), coords, matr, sol, m);
    }



    coords1.close();
    sols1.close();
    matrixes1.close();
    meshes1.close();
    return values;
}

class DipolesVerificationTS : public ::testing::TestWithParam<ttype> {
public:


protected:
};


INSTANTIATE_TEST_SUITE_P(
        ValidationTest, DipolesVerificationTS,
        ::testing::ValuesIn(testFixtureGetter()),
        firstValueTuplePrinter<DipolesVerificationTS>);

TEST_P(DipolesVerificationTS, test_on_10_basik_conf_matrixes) {
    auto [nn, conf, matr, _, pp] = GetParam();

    EXPECT_EQ(matr.rows()/4, conf.size() / 2);
    dipoles::Dipoles dd(conf);
    compare2dArrays(dd.getMatrixx(), matr, twoDArrayDoubleComparator<FloatType>::call, 1e20 / 10000);

}

TEST_P(DipolesVerificationTS,
       test_on_10_basik_conf_solutions) {

    auto [nn, _, matr, sol, pp] = GetParam();

    dipoles::Dipoles dd;
    dd.loadFromMatrix(matr);

    auto solut = dd.solve<ct::EigenVec>();
    compareArrays(sol, solut, arrayDoubleComparator<FloatType>::call);
}


TEST_P(DipolesVerificationTS, test_on_10_basik_conf_meshes) {
    std::ios_base::sync_with_stdio(false);


    auto [nn, conf, mattr, sol, mesh] = GetParam();

    dipoles::Dipoles dd;
    dd.getFullFunction_(conf, sol);

    MeshCreator mm;
    mm.constructMeshes();
    mm.applyFunction(dd.getI2function());
    auto r2 = meshStorage::unflatten(mm.spans[2]);

    auto ress = meshStorage::unflatten(mesh.data[2], mesh.dimensions);

    compare2dArrays<true>(ress, r2, twoDArrayDoubleComparator<FloatType>::call, 1e-3);
}

