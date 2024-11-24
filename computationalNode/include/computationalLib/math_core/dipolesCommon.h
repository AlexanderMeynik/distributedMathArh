#ifndef DIPLOM_DIPOLESCOMMON_H
#define DIPLOM_DIPOLESCOMMON_H

#include <vector>
#include <memory>

#include <iostream>
#include <iomanip>
#include <variant>

#include <eigen3/Eigen/Dense>
#include "common/my_consepts.h"
#include "const.h"

using namespace Eigen;
using namespace myConcepts;


namespace dipoles {
    using matplot::gca;
    using std::function, std::pair, std::vector, std::array;
    using const_::FloatType;

    bool isSymmetric(const Eigen::Matrix<FloatType , -1, -1> &matr);
    enum ReturnType:size_t
    {
        ArrayEigenVectors=0,
        EigenVector,
        StdVector
    };
    //todo https://godbolt.org/z/qaohWfvfe
    //https://stackoverflow.com/questions/68059855/map-enum-values-to-corresponding-types-with-templates-at-compile-time
    using Arr2EigenVec=std::array<Eigen::Vector<FloatType, Eigen::Dynamic>, 2>;
    using EigenVec=Eigen::Vector<FloatType, Eigen::Dynamic>;
    using standartVec=std::vector<FloatType>;

    using integrableFunction = std::function<FloatType(FloatType, FloatType, FloatType)> ;
    using directionGraph= std::function<FloatType(FloatType, FloatType)> ;
    using matrixType=Eigen::Matrix<FloatType, Eigen::Dynamic, Eigen::Dynamic>;



    using retTypes =std::variant< std::type_identity<Arr2EigenVec>,
            std::type_identity<EigenVec>,
            std::type_identity<standartVec>>;


    /**
    *
    */
    static const std::unordered_map <size_t , retTypes> enumToType = {
            {ArrayEigenVectors,   std::type_identity<Arr2EigenVec>{}},
            {EigenVector, std::type_identity<EigenVec>{} },
            {StdVector,   std::type_identity<standartVec>{}}
    };//todo reverse
}

#endif //DIPLOM_DIPOLESCOMMON_H
