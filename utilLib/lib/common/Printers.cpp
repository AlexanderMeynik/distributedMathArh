#include "common/Printers.h"

namespace print_utils {

Json::Value ToJson(const ms::MeshCreator &mesh,
                   bool print_dims,
                   bool print_lims) {
  Json::Value res = ContinuousToJson(mesh.data_[2], false);

  if (print_dims) {
    res["dimensions"][0] = mesh.dimensions_[0];
    res["dimensions"][1] = mesh.dimensions_[1];
  }
  if (print_lims) {
    for (int i = 0; i < mesh.limits_.size(); i++) {
      res["limits"][i] = mesh.limits_[i];
    }
  }
  return res;
}

void PrintMesh(std::ostream &out,
               const ms::MeshCreator &mesh,
               const IoFormat &form,
               bool print_dims,
               bool print_lims,
               const EFormat &eigen_form) {

  switch (form) {
    case IoFormat::SERIALIZABLE:goto ser;
    case IoFormat::HUMAN_READABLE: {
      goto human;
    }
  }

  ser:
  {

    IosStateScientific ios_state_scientific(out, out.precision());
    if (print_dims) {
      out << mesh.dimensions_[0] << '\t' << mesh.dimensions_[1] << '\n';
    }
    if (print_lims) {
      out << mesh.limits_[0] << '\t' << mesh.limits_[1] << '\n';
      out << mesh.limits_[2] << '\t' << mesh.limits_[3] << '\n';
    }
    auto mm = ToEigenRowVector(mesh.data_.back());

    out << mm.format(eigen_form);
    return;
  }
  human:
  {
    IosStateScientific ios_state_scientific(out, out.precision());
    mesh_storage::PrintDec(mesh, out);
  }

}

void
MatrixPrint1D(std::ostream &out,
              const common_types::MatrixType &matr,
              bool print_size,
              const EFormat &eigen_form) {
  auto map = Eigen::Map<const Eigen::RowVector<FloatType, -1>>(matr.data(), matr.size());
  if (print_size) {
    out << matr.size() << '\n';
  }
  out << map.format(eigen_form);
}

void
MatrixPrint2D(std::ostream &out,
              const common_types::MatrixType &matr,
              bool print_dims,
              const EFormat &eigen_form) {
  if (print_dims) {
    out << matr.rows() << '\t' << matr.cols() << '\n';
  }
  out << matr.format(eigen_form);
}
Json::Value SerializeException(const shared::MyException &ex) {
  Json::Value res;
  res["message"]=ex.what();
  res["severity"]=shared::kSevToStr[static_cast<unsigned long>(ex.getSev())];
  return res;
}
}
