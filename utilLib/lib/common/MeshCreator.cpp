#include "common/MeshCreator.h"

namespace mesh_storage {

co::MeshStorageType
ComputeFunction(const co::MeshStorageType &a, const co::MeshStorageType &b, const co::DirectionGraph &func) {
  if (!a.size() || !b.size()) {
    throw shared::myException(
        fmt::format("Zero container sizes: a.size() = {},b.size() = {}", a.size(), b.size()));
  }

  if (a.size() != b.size()) {
    throw shared::mismatchedSizes(a.size(), b.size());
  }

  auto sz = a.size();
  co::MeshStorageType result(sz);
  for (size_t i = 0; i < sz; ++i) {
    result[i] = func(a[i], b[i]);
  }
  return result;
}

FloatType GetMeshDiffNorm(const co::MeshStorageType &mesh1, const co::MeshStorageType &mesh2) {
  return sqrt(pow((mesh1 - mesh2), 2).sum());
}

void AddMesh(co::MeshStorageType &a, const co::MeshStorageType &b) {
  a += b;
}

void MeshCreator::ApplyFunction(const co::DirectionGraph &func) {
  data_[2] = mesh_storage::ComputeFunction(data_[0], data_[1], func);
}

MeshArr<3> SphericalTransformation(const MeshCreator &oth) {
  MeshArr<3> res;
  for (size_t i = 0; i < 3; ++i) {
    res[i] = oth.data_[i];
  }

  res[0] = oth.data_[2] * sin(oth.data_[1]) * cos(oth.data_[0]);
  res[1] = oth.data_[2] * sin(oth.data_[1]) * sin(oth.data_[0]);
  res[2] = oth.data_[2] * cos(oth.data_[1]);
  return res;
}

MeshCreator::MeshCreator(bool construct) : dimensions_(kDefaultDims), limits_(kDefaultLims) {

  if (construct) {
    data_ = {{co::MeshStorageType(dimensions_[0] * dimensions_[1]),
              co::MeshStorageType(dimensions_[0] * dimensions_[1]),
              co::MeshStorageType(dimensions_[0] * dimensions_[1])}};
  }
}

void MeshCreator::ConstructMeshes(const DimType &dims, const LimType &lims) {
  this->dimensions_ = dims;
  this->limits_ = lims;

  auto phi = mesh_storage::MyLinspace<std::valarray>(limits_[2], limits_[3], dimensions_[1]);
  auto theta = mesh_storage::MyLinspace<std::valarray>(limits_[0], limits_[1], dimensions_[0]);
  /*std::array<std::valarray<FloatType>, 2> coords;
  for (size_t i = 0; i < 2; ++i) {
    coords[i] = mesh_storage::MyLinspace<std::valarray>(limits_[2 * i], limits_[2 * i + 1], dimensions_[i]);
  }*/

  auto rrr = mesh_storage::MyMeshGrid(phi, theta);
  data_[0] = rrr[0];
  data_[1] = rrr[1];
}

void MeshCreator::ConstructMeshes(std::optional<DimType> dim_opt, std::optional<LimType> lim_opt) {
  ConstructMeshes(dim_opt.value_or(this->dimensions_), lim_opt.value_or(this->limits_));
}

void MeshCreator::ApplyIntegrate(const co::IntegrableFunction &func, FloatType a, FloatType b) {
  this->ApplyFunction([&func, &a, b](FloatType x, FloatType y) {
    return IntegrateLambdaForOneVariable<61>(func, y, x, a, b);
  });
}

void PrintDec(const mesh_storage::MeshCreator &mmesh, std::ostream &out) {
  auto ext0 = mmesh.dimensions_[0];
  auto ext1 = mmesh.dimensions_[1];

  out << ext0 << '\t' << ext1 << '\n';
  out << "Функция I(phi,th)\n";
  out << "phi\\th\t\t";
  std::vector<FloatType> row(ext0);
  for (size_t i = 0; i < ext0 - 1; ++i) {
    row[i] = mmesh.data_[1][i * ext1];
    out << mmesh.data_[1][i * ext1] << '\t';
  }
  row[ext0 - 1] = mmesh.data_[1][(ext0 - 1) * ext1];
  out << mmesh.data_[1][(ext0 - 1) * ext1]
      << '\n';

  std::vector<FloatType> col(ext1);
  for (size_t i = 0; i < ext1; ++i) {
    auto phi = mmesh.data_[0][i];
    col[i] = phi;
    out << phi << "\t";
    for (size_t j = 0; j < ext0 - 1; ++j) {
      out << mmesh.data_[2][j * ext1 + i] << "\t";
    }
    out << mmesh.data_[2][(ext0 - 1) * ext1 + i] << "\n";
  }
}

co::MeshDrawClass Unflatten(const common_types::MeshStorageType &mm, const DimType &dims) {
  auto res = co::MeshDrawClass(dims[0], co::StdVec(dims[1], 0.0));

  for (size_t i = 0; i < dims[0]; ++i) {
    for (size_t j = 0; j < dims[1]; ++j) {
      res[i][j] = mm[i * dims[1] + j];
    }
  }

  return res;
}

}