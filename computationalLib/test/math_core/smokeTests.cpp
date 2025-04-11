
#include <chrono>

#include "common/sharedDeclarations.h"
#include "math_core/Dipoles.h"
#include "common/commonTypes.h"
#include "common/Parsers.h"
#include "common/Printers.h"
#include "../GoogleCommon.h"

using namespace pu;
using namespace test_common;
std::string res_dir_path = "../../../res/";
std::string filename = res_dir_path.append("config.txt");
std::string subdir = filename.substr(0, filename.rfind('.')) + "data7_25";

using mesh_storage::MeshCreator;
using Ttype = std::tuple<std::string, std::vector<FloatType>, ct::MatrixType, ct::EigenVec, MeshCreator>;

std::vector<Ttype> TestFixtureGetter() {

  int nn;

  std::vector<Ttype> values;
  std::ifstream coords1(subdir + "/coordinates.txt");
  std::ifstream sols1(subdir + "/solutions.txt");
  std::ifstream matrixes1(subdir + "/matrixes.txt");
  std::ifstream meshes1(subdir + "/meshes.txt");
  coords1 >> nn;
  sols1 >> nn;
  matrixes1 >> nn;
  meshes1 >> nn;
  values.reserve(nn);

  IoFormat a;
  coords1 >> a;
  sols1 >> a;
  matrixes1 >> a;
  meshes1 >> a;

  EFormat ef;
  coords1 >> ef;
  sols1 >> ef;
  matrixes1 >> ef;
  meshes1 >> ef;

  for (int i = 0; i < nn; ++i) {
    my_concepts::isOneDimensionalContinuous auto sol = print_utils::ParseOneDim<ct::EigenVec>(sols1);
    my_concepts::isOneDimensionalContinuous auto coords = print_utils::ParseOneDim<ct::StdVec>(coords1);

    long rows, cols;
    matrixes1 >> rows >> cols;
    common_types::MatrixType res(rows, cols);

    for (int r = 0; r < rows; ++r) {
      for (int c = 0; c < cols; ++c) {
        matrixes1 >> res(r, c);
      }
    }
    //ct::matrixType matr=printUtils::parseMatrix(matrixes1);//todo why this cause segfault

    auto m = print_utils::ParseMeshFrom(meshes1);

    values.emplace_back(std::to_string(i), coords, res, sol, m);
  }

  coords1.close();
  sols1.close();
  matrixes1.close();
  meshes1.close();
  return values;
}

class DipolesVerificationTs : public ::testing::TestWithParam<Ttype> {
 public:

 protected:
};

INSTANTIATE_TEST_SUITE_P(
    ValidationTest, DipolesVerificationTs,
    ::testing::ValuesIn(TestFixtureGetter()),
    FirstValueTuplePrinter<DipolesVerificationTs>);

TEST_P(DipolesVerificationTs, test_on_10_basik_conf_matrixes) {
  auto [nn, conf, matr, _, pp] = GetParam();

  EXPECT_EQ(matr.rows() / 4, conf.size() / 2);
  dipoles::Dipoles dd(conf);
  Compare2DArrays(dd.GetMatrixx(), matr, twoDArrayDoubleComparator<FloatType>::call, 1e20 / 10000);

}

TEST_P(DipolesVerificationTs,
       test_on_10_basik_conf_solutions) {

  auto [nn, _, matr, sol, pp] = GetParam();

  dipoles::Dipoles dd;
  dd.LoadFromMatrix(matr);

  auto solut = dd.Solve<ct::EigenVec>();
  CompareArrays(sol, solut, arrayDoubleComparator<FloatType>::call);
}

TEST_P(DipolesVerificationTs, test_on_10_basik_conf_meshes) {
  std::ios_base::sync_with_stdio(false);

  auto [nn, conf, mattr, sol, mesh] = GetParam();

  dipoles::Dipoles dd;
  dd.GetFullFunction(conf, sol);

  MeshCreator mm;
  mm.ConstructMeshes();
  mm.ApplyFunction(dd.GetI2Function());
  auto r2 = mesh_storage::Unflatten(mm.data_[2], mm.dimensions_);

  auto ress = mesh_storage::Unflatten(mesh.data_[2], mesh.dimensions_);

  Compare2DArrays<true>(ress, r2, twoDArrayDoubleComparator<FloatType>::call, 1e-3);
}

