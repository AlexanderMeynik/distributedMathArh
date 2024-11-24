#ifndef DIPLOM_DIPOLESCOMMON_H
#define DIPLOM_DIPOLESCOMMON_H

#include <vector>
#include <array>

#include <eigen3/Eigen/Dense>

#include "common/myConcepts.h"
#include "const.h"

using namespace Eigen;
using namespace myConcepts;

///dipoles namespace
namespace dipoles {

    using matplot::gca;
    using std::function, std::pair, std::vector, std::array;

    using const_::FloatType;

    /**
     * @brief Enum for return types
     */
    enum class ReturnType : size_t {
        ArrayEigenVectors = 0,
        EigenVector,
        StdVector
    };

    using Arr2EigenVec = std::array<Eigen::Vector<FloatType, Eigen::Dynamic>, 2>;
    using EigenVec = Eigen::Vector<FloatType, Eigen::Dynamic>;
    using standartVec = std::vector<FloatType>;

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
