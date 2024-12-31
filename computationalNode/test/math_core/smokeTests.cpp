
#include <chrono>


#include "common/typeCasts.h"
#include "computationalLib/math_core/Dipoles.h"
#include "common/commonTypes.h"
#include "iolib/Parsers.h"

#include "../GoogleCommon.h"


using namespace commonTypes;
using namespace commonDeclarations;
using namespace testCommon;


std::string res_dir_path = "../../../res/";
std::string filename = res_dir_path.append("config.txt");
string subdir = filename.substr(0, filename.rfind('.')) + "data7_25";


using coordType=std::vector<std::vector<FloatType >>;
using meshStorage::MeshCreator;
using ttype = std::tuple<std::string, std::vector<FloatType>, Parser<matrixType>, Parser<EigenVec>, Parser<MeshCreator>>;

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

        auto matr = Parser<matrixType >(NN);
        matrixes >> matr;

        auto solvv = Parser<EigenVec>(NN);
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
    compare2dArrays(dd.getMatrixx(), matr.vals_, twoDArrayDoubleComparator, 1e20 / 10000);

}

TEST_P(DipolesVerificationTS,
       test_on_10_basik_conf_solutions) {

    auto [nn, _, matr, sol, pp] = GetParam();

    dipoles::Dipoles dd;
    dd.loadFromMatrix(matr.vals_);

    auto solut = dd.solve<EigenVec>();
    compareArrays(sol.vals_, solut, arrayDoubleComparator);
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

    compare2dArrays<true>(ress, r2, twoDArrayDoubleComparator, 1e-3);
}

