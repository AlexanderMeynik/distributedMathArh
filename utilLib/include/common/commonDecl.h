#pragma once
#ifndef DIPLOM_COMMONDECL_H
#define DIPLOM_COMMONDECL_H

#include <eigen3/Eigen/Dense>
#include <vector>
#include <array>
#include <valarray>

/// namespace for some const declarations
namespace const_ {
    //todo some of the decalred types need to be pulled away
    using FloatType = double;


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
    using standartVec = std::vector<FloatType>;
    using standartValarr = std::valarray<FloatType>;
    using meshDrawClass = std::vector<standartVec>;
    using meshStorageType =std::valarray<FloatType>;

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
        using type = standartVec;
    };

    template<>
    struct returnToDataTypeT<returnType::StdValarray> {
        using type = standartValarr ;
    };

    /**
     * @brief Converter to return type
     */
    template<returnType T>
    using returnToDataType = typename returnToDataTypeT<T>::type;
}
#endif //DIPLOM_COMMONDECL_H
