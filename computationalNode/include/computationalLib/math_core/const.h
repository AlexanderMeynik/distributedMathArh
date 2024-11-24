

#ifndef DIPLOM_CONST_H
#define DIPLOM_CONST_H

#include <iostream>
#include <array>
#include <vector>

#include <eigen3/Eigen/Dense>
#include <matplot/matplot.h>

#include "common/constants.h"

using const_::FloatType;


struct params {
    static constexpr FloatType c = 3.0 * 1e8;
    static constexpr FloatType yo = 1e7;
    static constexpr FloatType omega = 1e15;
    static constexpr FloatType omega0 = omega;
    static constexpr FloatType a = 1;
    static constexpr FloatType eps = 1;
};


#endif //DIPLOM_CONST_H
