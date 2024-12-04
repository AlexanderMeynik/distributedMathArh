#pragma once
#ifndef DIPLOM_DIPOLESCOMMON_H
#define DIPLOM_DIPOLESCOMMON_H

#include <vector>
#include <array>
#include <valarray>

#include <eigen3/Eigen/Dense>

#include "common/myConcepts.h"
#include "const.h"

using namespace commonDeclarations;

///dipoles namespace
namespace dipoles {

    using std::pair, std::vector, std::array;

    using const_::FloatType;

    /**
     * @brief Enum for return types
     */
    enum class ReturnType : size_t {
        ArrayEigenVectors = 0,
        EigenVector,
        StdVector,
        StdValarray
    };

    using Arr2EigenVec = std::array<Eigen::Vector<FloatType, Eigen::Dynamic>, 2>;
    using EigenVec = Eigen::Vector<FloatType, Eigen::Dynamic>;
    using standartVec = std::vector<FloatType>;
    using standartValarr = std::valarray<FloatType>;

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
    template<ReturnType>
    struct ReturnToDataType_t;
    template<>
    struct ReturnToDataType_t<ReturnType::ArrayEigenVectors> {
        using type = Arr2EigenVec;
    };
    template<>
    struct ReturnToDataType_t<ReturnType::EigenVector> {
        using type = EigenVec;
    };
    template<>
    struct ReturnToDataType_t<ReturnType::StdVector> {
        using type = standartVec;
    };

    template<>
    struct ReturnToDataType_t<ReturnType::StdValarray> {
        using type = standartValarr ;
    };

    /**
     * @brief Converter to return type
     */
    template<ReturnType T>
    using ReturnToDataType = typename ReturnToDataType_t<T>::type;

    /**
     * Check that matrix is symmetric
     * @param matr
     */
    bool isSymmetric(const matrixType &matr);
}

#endif //DIPLOM_DIPOLESCOMMON_H
