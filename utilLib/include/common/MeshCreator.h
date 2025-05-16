#pragma once

#include <iosfwd>
#include <optional>

#include <boost/math/quadrature/gauss_kronrod.hpp>

#include "common/commonTypes.h"
#include "common/printUtils.h"

namespace mesh_storage {
namespace co = common_types;
using shared::FloatType, shared::params;
using print_utils::IosStateScientific;
using common_types::DimType;

///stores upper and lower limit pairs for meshes
using LimType = std::array<FloatType, 4>;

template<size_t N>
using MeshArr = std::array<co::MeshStorageType, N>;

/**
 *
 * @tparam Ndots
 * @param function
 * @param left
 * @param right
 * @param max_depth
 * @param tol
 */
template<unsigned Ndots = 61>
FloatType Integrate(const std::function<FloatType(FloatType)> &function,
                    FloatType left,
                    FloatType right,
                    unsigned int max_depth = 5,
                    FloatType tol = Eigen::NumTraits<FloatType>::epsilon());

template<unsigned Ndots = 61>
FloatType IntegrateLambdaForOneVariable(const co::IntegrableFunction &function,
                                        FloatType theta,
                                        FloatType phi,
                                        FloatType left,
                                        FloatType right,
                                        unsigned int max_depth = 5,
                                        FloatType tol = 1e-20);

/**
 * @brief Computes functional mesh for 2 coordinates
 * @param a
 * @param b
 * @param func
 */
co::MeshStorageType ComputeFunction(const co::MeshStorageType &a,
                                    const co::MeshStorageType &b,
                                    const co::DirectionGraph &func);

/**
 * @brief Computes square Diff norm for two meshes
 * @param mesh1
 * @param mesh2
 */
FloatType GetMeshDiffNorm(const co::MeshStorageType &mesh1,
                          const co::MeshStorageType &mesh2);

void AddMesh(co::MeshStorageType &a,
             const co::MeshStorageType &b);

/**
 * @brief Computes meshes using 2 double arrays
 * @param a
 * @param b
 * @return
 */
template<template<typename ...> typename container =std::vector>
std::array<co::MeshStorageType, 2> MyMeshGrid(const container<co::FloatType> &a,
                                              const container<co::FloatType> &b);

/**
 * @brief Generates linearly spaced vectors
 * @tparam container
 * @tparam T
 * @tparam end
 * @param lower_bound
 * @param upper_bound
 * @param n
 * @return
 */
template<template<typename ...> typename container =std::vector, typename T=FloatType, bool end = true>
container<T> MyLinspace(T lower_bound,
                        T upper_bound,
                        size_t n);

co::MeshDrawClass Unflatten(const common_types::MeshStorageType &mm,
                            const DimType &dims);

static constexpr inline const FloatType kRr = 2 * M_PI / params::omega;

static constexpr inline DimType kDefaultDims = {7, 25};
static constexpr inline LimType kDefaultLims = {0, M_PI_2, 0, M_PI * 2};

/**
 * @brief Class that handles 3d mesh creation and parameters management
 */
class MeshCreator {
 public:
  /**
   * @brief Default MeshCreator constructor
   * @param construct - will allocate memory for data arrays if is true
   */
  MeshCreator(bool construct = true);

  /**
   * @brief Computes 2d meshgrid using dims,lims
   * @param dims
   * @param lims
   */
  void ConstructMeshes(const DimType &dims,
                       const LimType &lims);

  /**
   * @brief Will substitute any null-opt with default values
   * @param dim_opt
   * @param lim_opt
   */
  void ConstructMeshes(std::optional<DimType> dim_opt = std::nullopt,
                       std::optional<LimType> lim_opt = std::nullopt);

  /**
   * @brief Cartesian to spherical coordinate transformation
   * @param oth
   * @return
   */
  friend MeshArr<3> SphericalTransformation(const MeshCreator &oth);

  /**
   * @brief Computes data[2] using provided Func function
   * @param func
   */
  void ApplyFunction(const co::DirectionGraph &func);

  /**
   * @brief Uses numerical integration with range [a,b] compute data[2]
   * @param func
   * @param a
   * @param b
   */
  void ApplyIntegrate(const co::IntegrableFunction &func,
                      FloatType a = 0,
                      FloatType b = kRr);

  /**
   * @brief Calls provided callback to Plot this mesh
   * @param filename
   * @param plot_callback
   * @todo remove deprecated callback
   */
  void PlotAndSave[[deprecated("remove mathplot support")]](const std::string &filename,
                                                            const std::function<void(const std::string &filename,
                                                                                     const MeshCreator &)> &plot_callback) {
    plot_callback(filename, *this);
  }

  DimType dimensions_;
  LimType limits_;
  MeshArr<3> data_;
};

/**
 * @brief Prints mesh in human readable way
 * @param mmesh
 * @param out
 */
void PrintDec(const mesh_storage::MeshCreator &mmesh,
              std::ostream &out);

}

/// meshStorage namespace
namespace mesh_storage {

template<unsigned int Ndots>
FloatType Integrate(const std::function<FloatType(FloatType)> &function,
                    FloatType left, FloatType right,
                    unsigned int max_depth,
                    FloatType tol) {
  FloatType error;
  double q = boost::math::quadrature::gauss_kronrod<FloatType, Ndots>::integrate(
      function,
      left,
      right,
      max_depth,
      tol,
      &error);
  return q;
}

template<unsigned int Ndots>
FloatType
IntegrateLambdaForOneVariable(const co::IntegrableFunction &function,
                              FloatType theta,
                              FloatType phi,
                              FloatType left,
                              FloatType right,
                              unsigned int max_depth,
                              FloatType tol) {
  std::function<FloatType(FloatType)> tt = [&theta, &phi, &function](FloatType t) {
    return function(theta, phi, t);
  };
  return Integrate<Ndots>(tt, left, right, max_depth, tol);
}

template<template<typename ...> typename container, typename T, bool end>
container<T> MyLinspace(T lower_bound, T upper_bound, size_t n) {

  if (n == 0 || lower_bound - upper_bound == 0) {
    throw std::invalid_argument("Zero linspace size");
  }

  container<T> result(n);

  size_t div;
  if constexpr (end) {
    div = n - 1;
  } else {
    div = n;
  }
  T step = (upper_bound - lower_bound) / (T) div;

  for (int i = 0; i < n; ++i) {
    result[i] = i * step + lower_bound;
  }
  return result;
}

template<template<typename ...> typename container>
std::array<co::MeshStorageType, 2>
MyMeshGrid(const container<co::FloatType> &a,
           const container<co::FloatType> &b) {
  std::array<co::MeshStorageType, 2> ret = {co::MeshStorageType(b.size() * a.size()),
                                            co::MeshStorageType(b.size() * a.size())};

  for (size_t i = 0; i < b.size(); ++i) {
    for (size_t j = 0; j < a.size(); ++j) {
      ret[0][i * a.size() + j] = a[j];
      ret[1][i * a.size() + j] = b[i];
    }
  }
  return ret;
}

}

