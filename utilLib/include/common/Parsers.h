#pragma once

#include <iostream>

#include <algorithm>//do weneed it?
#include <numeric>
#include <fstream>
#include <iomanip>
#include <optional>

#include <json/json.h>

#include "common/MeshCreator.h"
#include "common/myConcepts.h"

/// printUtils namespace
namespace print_utils {
using namespace shared;
using my_concepts::isOneDimensionalContinuous;
namespace ms = mesh_storage;
namespace ct = common_types;

/**
 * @brief Parse one dimensional array from json
 * @tparam Collection
 * @param val
 * @param sz
 * @param sizeless_format
 */
template<isOneDimensionalContinuous Collection>
Collection JsonToContinuous(const Json::Value &val,
                            std::optional<size_t> sz = std::nullopt,
                            bool sizeless_format=false);

/**
 * @brief Parse MeshCreator from json
 * @param val
 * @param dim_opt
 * @param lim_opt
 * @return
 */
ms::MeshCreator FromJson(Json::Value &val,
                         std::optional<ms::DimType> dim_opt = std::nullopt,
                         std::optional<ms::LimType> lim_opt = std::nullopt);

/**
 * @brief Parse one dimensional array from provided istream
 * @tparam Struct
 * @param in
 * @param size_opt
 * @param ef
 * @return Struct to store the values
 */
template<isOneDimensionalContinuous Struct>
Struct ParseOneDim(std::istream &in,
                   std::optional<size_t> size_opt = std::nullopt,
                   const EFormat &ef = EFormat());

/**
 * @brief Parse matrix
 * @param in
 * @param dim_opt
 * @param ef
 */
ct::MatrixType ParseMatrix(std::istream &in,
                           std::optional<ct::DimType> dim_opt = std::nullopt,
                           const EFormat &ef = EFormat());

/**
 * @brief Parse solution function interface
 * @tparam Struct
 * @param in
 * @param format
 * @param size_opt
 * @param ef
 * @return solution vector
 */
template<isOneDimensionalContinuous Struct>
Struct ParseSolution(std::istream &in,
                     IoFormat format = IoFormat::SERIALIZABLE,
                     std::optional<size_t> size_opt = std::nullopt,
                     const EFormat &ef = EFormat());

/**
 * @brief Parse coordinates function interface
 * @tparam Struct
 * @param in
 * @param format
 * @param size_opt
 * @param ef
 * @return coordinates vector
 */
template<isOneDimensionalContinuous Struct>
Struct ParseCoordinates(std::istream &in,
                        IoFormat format = IoFormat::SERIALIZABLE,
                        std::optional<size_t> size_opt = std::nullopt,
                        const EFormat &ef = EFormat());

/**
 * @brief Parses mesh creator from provided istream
 * @param in
 * @param ef
 * @param format
 * @param dim_opt
 * @param lim_opt
 * @return MeshCreator instance
 */
mesh_storage::MeshCreator ParseMeshFrom(std::istream &in,
                                        IoFormat format = IoFormat::SERIALIZABLE,
                                        std::optional<ms::DimType> dim_opt = std::nullopt,
                                        std::optional<ms::LimType> lim_opt = std::nullopt,
                                        const EFormat &ef = EFormat());

}

namespace print_utils {
template<isOneDimensionalContinuous Struct>
Struct JsonToContinuous(const Json::Value &val,
                        std::optional<size_t> sz,
                        bool sizeless_format) {

  size_t size;
  if(sz.has_value())
  {
    size=sz.value();
  }
  else {
    if (sizeless_format) {
      size = val.size();
    } else {
      size = val["size"].asUInt();
    }
  }

  Struct res(size);
  if(!sizeless_format)
  {
    for (int i = 0; i < size; ++i) {
      res[i] = val["data"][i].as<std::remove_all_extents_t<typename Struct::value_type>>();
    }
  }
  else
  {
    for (int i = 0; i < size; ++i) {
      res[i] = val[i].as<std::remove_all_extents_t<typename Struct::value_type>>();
    }
  }

  return res;
}

template<isOneDimensionalContinuous Struct>
Struct ParseOneDim(std::istream &in,
                   std::optional<size_t> size_opt,
                   const EFormat &ef) {

  size_t size;
  if (size_opt.has_value()) {
    size = size_opt.value();
  } else {
    in >> size;
  }

  if (!in) {
    throw ioError(to_string(in.rdstate()));
  }

  Struct res(size);

  for (size_t i = 0; i < size; ++i) {
    in >> res[i];
  }
  return res;
}

template<isOneDimensionalContinuous Struct>
Struct ParseSolution(std::istream &in,
                     IoFormat format,
                     std::optional<size_t> size_opt,
                     const EFormat &ef) {

  Struct res;
  switch (format) {
    case IoFormat::SERIALIZABLE:return ParseOneDim<Struct>(in, size_opt, ef);
    case IoFormat::HUMAN_READABLE: {
      size_t n;
      in >> n;
      std::string dummy;
      std::getline(in, dummy);
      std::getline(in, dummy);
      std::getline(in, dummy);

      if (!in) {
        throw ioError(to_string(in.rdstate()));
      }

      Struct sol(n * 4);

      for (int i = 0; i < n; i++) {
        FloatType ax, bx, cx;
        FloatType ay, by, cy;
        in >> ax >> bx >> cx >> ay >> by >> cy;

        sol[2 * i] = ax;
        sol[2 * i + 2 * n] = bx;
        sol[2 * i + 1] = ay;
        sol[2 * i + 1 + 2 * n] = by;

      }
      return sol;

    }
  }
  return res;
}

template<isOneDimensionalContinuous Struct>
Struct ParseCoordinates(std::istream &in,
                        IoFormat format,
                        std::optional<size_t> size_opt,
                        const EFormat &ef) {
  Struct res;
  switch (format) {
    case IoFormat::SERIALIZABLE:return ParseOneDim<Struct>(in, size_opt, ef);
    case IoFormat::HUMAN_READABLE: {
      std::string dummy;
      std::getline(in, dummy);
      size_t n;
      in >> n;
      if (!in) {
        throw ioError(to_string(in.rdstate()));
      }

      Struct coord(n * 2);

      for (int i = 0; i < n; i++) {
        FloatType x, y;
        in >> coord[i] >> coord[i + n];
      }
      return coord;

    }
  }
  return res;
}
}
