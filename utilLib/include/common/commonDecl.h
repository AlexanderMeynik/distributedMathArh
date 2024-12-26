#pragma once
#ifndef DIPLOM_COMMONDECL_H
#define DIPLOM_COMMONDECL_H

#include <eigen3/Eigen/Dense>
#include <vector>
#include <array>
#include <valarray>

namespace const_ {
    //todo move some common things here
    using FloatType = double;


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
}
#endif //DIPLOM_COMMONDECL_H
