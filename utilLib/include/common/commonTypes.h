#pragma once

#include <variant>

#include <eigen3/Eigen/Dense>

#include "sharedDeclarations.h"

/// namespace for some common types used elsewhere
namespace common_types {
using shared::FloatType;

/**
 * @brief Supertype for simple json values
 * This is not recursive type, so nested json structures are not supported.
 * Returns largest int types for any integral variables.
 */
using JsonVariant = std::variant<
    std::nullptr_t,
    bool,
    int64_t,
    uint64_t,
    double,
    std::string
>;

/**
 * @brief Enum for return types
 */
enum class ReturnType : size_t {
  ARRAY_EIGEN_VECTORS = 0,
  EIGEN_VECTOR,
  STD_VECTOR,
  STD_VALARRAY
};

using Arr2EigenVec = std::array<Eigen::Vector<FloatType, Eigen::Dynamic>, 2>;
using EigenVec = Eigen::Vector<FloatType, Eigen::Dynamic>;
using StdVec = std::vector<FloatType>;
using StdValarr = std::valarray<FloatType>;
using MeshDrawClass = std::vector<StdVec>;
using MeshStorageType = std::valarray<FloatType>;

///stores information about dimensions
using DimType = std::array<size_t, 2>;

/**
 * @brief Type for function thta will be integrated to get directional graph
 */
using IntegrableFunction = std::function<FloatType(FloatType, FloatType, FloatType)>;

/**
 * @brief type for a direction graph
 */
using DirectionGraph = std::function<FloatType(FloatType, FloatType)>;
using MatrixType = Eigen::Matrix<FloatType, Eigen::Dynamic, Eigen::Dynamic>;

/**
 * @brief Enum to return type conversion struct
 */
template<ReturnType>
struct returnToDataTypeT;
template<>
struct returnToDataTypeT<ReturnType::ARRAY_EIGEN_VECTORS> {
  using Type = Arr2EigenVec;
};

template<>
struct returnToDataTypeT<ReturnType::EIGEN_VECTOR> {
  using Type = EigenVec;
};

template<>
struct returnToDataTypeT<ReturnType::STD_VECTOR> {
  using Type = StdVec;
};

template<>
struct returnToDataTypeT<ReturnType::STD_VALARRAY> {
  using Type = StdValarr;
};

/**
 * @brief Converter to return type
 */
template<ReturnType T>
using ReturnToDataType = typename returnToDataTypeT<T>::Type;
}
