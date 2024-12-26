#pragma once
#ifndef DIPLOM_DIPOLESCOMMON_H
#define DIPLOM_DIPOLESCOMMON_H


#include "common/myConcepts.h"
#include "const.h"

using namespace commonDeclarations;
namespace co=const_;
///dipoles namespace
namespace dipoles {

    using std::pair, std::vector, std::array;





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
     * Check that matrix is symmetric
     * @param matr
     */
    bool isSymmetric(const matrixType &matr);
}

#endif //DIPLOM_DIPOLESCOMMON_H
