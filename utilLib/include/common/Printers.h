#pragma once

#include <json/json.h>

#include "common/typeCasts.h"
#include "common/MeshCreator.h"

/// printUtils namespace
namespace print_utils {

using shared::FloatType;
using print_utils::IosStatePreserve, print_utils::IosStateScientific;
namespace sh = shared;

namespace ms = mesh_storage;

Json::Value SerializeException(const shared::MyException &ex);
/**
 * @brief Print mesh to supplied std::ostream
 * @param mesh
 * @param out
 * @param form
 * @param print_dims
 * @param print_lims
 * @param eigen_form
 */
void PrintMesh(std::ostream &out,
               const ms::MeshCreator &mesh,
               const IoFormat &form = IoFormat::SERIALIZABLE,
               bool print_dims = true,
               bool print_lims = true,
               const EFormat &eigen_form = EIGENF(EigenPrintFormats::VECTOR_FORMAT_1));

/**
 * @brief Cast any one dimensional array to Json::value
 * @tparam Collection
 * @param col
 * @param print_size
 * @param sizeless_format
 * @returns Json::Value with serialized array
 */
template<my_concepts::isOneDimensionalContinuous Collection>
Json::Value ContinuousToJson(const Collection &col,
                             bool print_size = true,
                             bool sizeless_format = false);

/**
 * @brief Transforms MeshCreator to json
 * @param mesh
 * @param print_dims
 * @param print_lims
 * @return Json::Value with serialized meshCreator
 */
Json::Value ToJson(const ms::MeshCreator &mesh,
                   bool print_dims = true,
                   bool print_lims = true);

/**
 * @brief Prints one dimensional Collection using Eigen format
 * @tparam Collection
 * @param out
 * @param col
 * @param print_size
 * @param eigen_form
 */
template<typename Collection>
requires my_concepts::isOneDimensionalContinuous<Collection> &&
    std::is_floating_point_v<typename Collection::value_type> ||
    std::is_integral_v<typename Collection::value_type>
void inline OneDimSerialize(std::ostream &out,
                            const Collection &col,
                            bool print_size = true,
                            const EFormat &eigen_form = EFormat()) {
  auto map = ToEigenRowVector(col);
  if (print_size) {
    out << col.size() << '\n';
  }
  out << map.format(eigen_form);
}

/**
 * @brief Prints one dimensional Collection to string  using Eigen format
 * @tparam Collection
 * @param col
 * @param print_size
 * @param eigen_form
 */
template<typename Collection>
requires my_concepts::isOneDimensionalContinuous<Collection> &&
    std::is_floating_point_v<typename Collection::value_type> ||
    std::is_integral_v<typename Collection::value_type>
std::string inline OneDimToString(const Collection &col,
                                  bool print_size = true,
                                  const EFormat &eigen_form = EFormat()) {

  std::ostringstream ss;
  OneDimSerialize(ss, col, print_size, eigen_form);
  return ss.str();
}

/**
 * Flattens and prints matrix as row Vector
 * @param out
 * @param matr
 * @param print_size
 * @param eigen_form
 */
void MatrixPrint1D(std::ostream &out,
                   const common_types::MatrixType &matr,
                   bool print_size = true,
                   const EFormat &eigen_form = EFormat());

/**
 * @brief Serializes matrix with coordinates
 * @param out
 * @param matr
 * @param print_dims
 * @param eigen_form
 */
void MatrixPrint2D(std::ostream &out,
                   const common_types::MatrixType &matr,
                   bool print_dims = true,
                   const EFormat &eigen_form = EIGENF(EigenPrintFormats::MATRIX_FORMAT_1));

/**
 * @brief Human readable way to Print solution vector
 * @tparam Collection
 * @param out
 * @param solution
 * @return
 */
template<typename Collection>
requires my_concepts::isOneDimensionalContinuous<Collection> &&
    std::is_floating_point_v<typename Collection::value_type>
int PrintSolutionFormat1(std::ostream &out,
                         const Collection &solution);

/**
 * @brief Human readable way to Print coordinates vector
 * @tparam Collection
 * @param out
 * @param col
 */
template<typename Collection>
void PrintCoordinates2(std::ostream &out,
                       const Collection &col);

/**
 * @brief Basic interface for printing coordinates
 * @tparam Collection
 * @param out
 * @param coord
 * @param format
 * @param print_size
 * @param eigen_form
 */
template<typename Collection>
requires my_concepts::isOneDimensionalContinuous<Collection> &&
    std::is_floating_point_v<typename Collection::value_type>
void PrintCoordinates(std::ostream &out,
                      const Collection &coord,
                      IoFormat format = IoFormat::SERIALIZABLE,
                      bool print_size = true,
                      const EFormat &eigen_form = EIGENF(EigenPrintFormats::VECTOR_FORMAT_1));

/**
 * @brief Basic interface for printing solution
 * @tparam Collection
 * @param out
 * @param sol
 * @param format
 * @param print_size
 * @param eigen_form
 */
template<typename Collection>
requires my_concepts::isOneDimensionalContinuous<Collection> &&
    std::is_floating_point_v<typename Collection::value_type>
void PrintSolution(std::ostream &out,
                   const Collection &sol,
                   IoFormat format = IoFormat::SERIALIZABLE,
                   bool print_size = true,
                   const EFormat &eigen_form = EIGENF(EigenPrintFormats::VECTOR_FORMAT_1));

}

namespace print_utils {

template<my_concepts::isOneDimensionalContinuous Struct>
Json::Value ContinuousToJson(const Struct &col,
                             bool print_size,
                             bool sizeless_format) {
  Json::Value res;
  if (!sizeless_format) {
    if (print_size) {
      res["size"] = col.size();
    }
    for (size_t i = 0; i < col.size(); i++) {
      res["data"][(Json::ArrayIndex) i] = col[i];
    }
  } else {
    for (size_t i = 0; i < col.size(); i++) {
      res[(Json::ArrayIndex) i] = col[i];
    }
  }

  return res;
}

template<typename Collection>
requires my_concepts::isOneDimensionalContinuous<Collection> &&
    std::is_floating_point_v<typename Collection::value_type>
int PrintSolutionFormat1(std::ostream &out,
                         const Collection &solution) {
  int n = solution.size() / 4.0;
  out << n << '\n';
  out << "Решение системы диполей\n";
  out << " Ai(x\\ny)\tBi(x\\ny)\tCi(x\\ny)\n";

  for (int i = 0; i < n; i++) {
    auto cx = sqrt(solution[2 * i] * solution[2 * i] +
        solution[2 * i + 2 * n] * solution[2 * i + 2 * n]);
    auto cy = sqrt(solution[2 * i + 1] * solution[2 * i + 1] +
        solution[2 * i + 1 + 2 * n] * solution[2 * i + 1 + 2 * n]);

    out << solution[2 * i] << "\t"
        << solution[2 * i + 2 * n] << "\t"
        << cx << "\n";
    out << solution[2 * i + 1] << "\t"
        << solution[2 * i + 1 + 2 * n] << "\t"
        << cy << "\n";

  }
  return 0;
}

template<typename Collection>
void PrintCoordinates2(std::ostream &out,
                       const Collection &col) {
  out << "Координаты диполей\n";

  if constexpr (not my_concepts::HasBracketsNested<Collection>) {
    auto n = col.size() / 2;
    out << n << '\n';
    for (int i = 0; i < n; ++i) {
      out << col[i] << '\t' << col[i + n] << "\n";
    }
  } else {
    auto n = col[0].size();
    out << n << '\n';
    for (int i = 0; i < n; ++i) {
      out << col[0][i] << '\t' << col[1][i] << "\n";
    }
  }
}

template<typename Collection>
requires my_concepts::isOneDimensionalContinuous<Collection> &&
    std::is_floating_point_v<typename Collection::value_type>
void PrintSolution(std::ostream &out,
                   const Collection &sol,
                   IoFormat format,
                   bool print_size,
                   const EFormat &eigen_form) {
  switch (format) {
    case IoFormat::SERIALIZABLE:OneDimSerialize(out, sol, print_size, eigen_form);
      break;
    case IoFormat::HUMAN_READABLE: {
      IosStateScientific ios_state_scientific(out, out.precision());
      PrintSolutionFormat1(out, sol);
      break;
    }
  }
}

template<typename Collection>
requires (my_concepts::isOneDimensionalContinuous<Collection> &&
    std::is_floating_point_v<typename Collection::value_type>)
void PrintCoordinates(std::ostream &out,
                      const Collection &coord,
                      IoFormat format, bool print_size,
                      const EFormat &eigen_form) {
  switch (format) {
    case IoFormat::SERIALIZABLE:OneDimSerialize(out, coord, print_size, eigen_form);
      break;
    case IoFormat::HUMAN_READABLE: {
      IosStateScientific ios_state_scientific(out, out.precision());
      PrintCoordinates2(out, coord);
      break;
    }
  }
}
}
