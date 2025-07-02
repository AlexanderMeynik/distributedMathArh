#include "common/Parsers.h"

namespace print_utils {

ms::MeshCreator FromJson(Json::Value &val,
                         std::optional<ms::DimType> dim_opt,
                         std::optional<ms::LimType> lim_opt) {
  mesh_storage::MeshCreator mm;

  ms::DimType dims;

  if (dim_opt.has_value()) {
    dims = dim_opt.value();
  } else {
    dims[0] = val["dimensions"][0].asUInt();
    dims[1] = val["dimensions"][1].asUInt();
  }
  ms::LimType lims{};

  if (lim_opt.has_value()) {
    lims = lim_opt.value();
  } else {
    for (int i = 0; i < lims.size(); ++i) {
      lims[i] = val["limits"][i].asDouble();
    }
  }

  mm.ConstructMeshes(dims, lims);
  mm.data_[2] = JsonToContinuous<common_types::MeshStorageType>(val, dims[0] * dims[1]);

  return mm;
}

mesh_storage::MeshCreator ParseMeshFrom(std::istream &in,
                                        IoFormat format,
                                        std::optional<ms::DimType> dim_opt,
                                        std::optional<ms::LimType> lim_opt,
                                        const EFormat &ef) {

  mesh_storage::MeshCreator mm;
  ms::DimType dims{};
  ms::LimType lims{};

  switch (format) {
    case IoFormat::SERIALIZABLE:

      if (dim_opt.has_value()) {
        dims = dim_opt.value();
      } else {
        in >> dims[0] >> dims[1];

        if (!in) {
          throw ioError(to_string(in.rdstate()));
        }
      }

      if (lim_opt.has_value()) {
        lims = lim_opt.value();
      } else {
        in >> lims[0] >> lims[1] >> lims[2] >> lims[3];
        if (!in) {
          throw ioError(to_string(in.rdstate()));
        }
      }

      mm.data_[2] = ParseOneDim<common_types::MeshStorageType>(in, dims[0] * dims[1]);

      break;

    case IoFormat::HUMAN_READABLE:

      in >> dims[0] >> dims[1];

      if (!in) {
        throw ioError(to_string(in.rdstate()));
      }

      std::string dummy;
      std::getline(in, dummy);
      std::getline(in, dummy);
      in >> dummy;

      in >> lims[0];
      FloatType a;
      for (int i = 0; i < dims[0] - 2; ++i) {
        in >> a;
      }
      in >> lims[1];

      ct::MeshStorageType m(dims[0] * dims[1]);

      for (int i = 0; i < dims[1]; ++i) {

        FloatType temp = 0;
        in >> temp;

        if (i == 0) {
          lims[2] = temp;
        }
        if (i == dims[1] - 1) {
          lims[3] = temp;
        }
        for (int j = 0; j < dims[0]; ++j) {
          FloatType val;
          in >> val;
          m[j * dims[1] + i] = val;

        }
      }

      mm.ConstructMeshes(dims, lims);

      mm.data_[2] = m;

      break;

  }
  mm.ConstructMeshes(dims, lims);

  return mm;

}

ct::MatrixType ParseMatrix(std::istream &in,
                           std::optional<ct::DimType> dim_opt,
                           const EFormat &ef) {
  size_t rows, cols;
  if (dim_opt.has_value()) {
    rows = dim_opt.value()[0];
    cols = dim_opt.value()[1];
  } else {
    in >> rows >> cols;
  }

  if (!in) {
    throw ioError(to_string(in.rdstate()));
  }

  common_types::MatrixType res(rows, cols);

  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      in >> res(i, j);
    }
  }
  return res;
}

}



