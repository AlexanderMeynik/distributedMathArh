#pragma once


#include <vector>
#include <array>
#include <valarray>

#include <eigen3/Eigen/Dense>

#include "sharedDeclarations.h"
/// namespace for some common types used elsewhere
namespace commonTypes {
    using shared::FloatType;

    /**
     * @brief Enum for return types
     */
    enum class returnType : size_t {
        ArrayEigenVectors = 0,
        EigenVector,
        StdVector,
        StdValarray
    };

    using Arr2EigenVec = std::array<Eigen::Vector<FloatType, Eigen::Dynamic>, 2>;
    using EigenVec = Eigen::Vector<FloatType, Eigen::Dynamic>;
    using stdVec = std::vector<FloatType>;
    using stdValarr = std::valarray<FloatType>;
    using meshDrawClass = std::vector<stdVec>;
    using meshStorageType = std::valarray<FloatType>;

    ///stores information about dimensions
    using dimType=std::array<size_t, 2>;

    /**
     * @brief Type for function thta will be integrated to get directional graph
     */
    using integrableFunction = std::function<FloatType(FloatType, FloatType, FloatType)>;

    /**
     * @brief type for a direction graph
     */
    using directionGraph = std::function<FloatType(FloatType, FloatType)>;
    using matrixType = Eigen::Matrix<FloatType, Eigen::Dynamic, Eigen::Dynamic>;

    /**
     * @brief Enum to return type conversion struct
     */
    template<returnType>
    struct returnToDataTypeT;
    template<>
    struct returnToDataTypeT<returnType::ArrayEigenVectors> {
        using type = Arr2EigenVec;
    };
    template<>
    struct returnToDataTypeT<returnType::EigenVector> {
        using type = EigenVec;
    };
    template<>
    struct returnToDataTypeT<returnType::StdVector> {
        using type = stdVec;
    };

    template<>
    struct returnToDataTypeT<returnType::StdValarray> {
        using type = stdValarr;
    };

    /**
     * @brief Converter to return type
     */
    template<returnType T>
    using returnToDataType = typename returnToDataTypeT<T>::type;
}
