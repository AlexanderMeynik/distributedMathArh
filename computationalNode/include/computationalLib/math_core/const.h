//
// Created by Lenovo on 05.06.2024.
//

#ifndef DIPLOM_CONST_H
#define DIPLOM_CONST_H

#include <iostream>
#include <array>
#include <eigen3/Eigen/Dense>
#include <vector>
#include <matplot/matplot.h>


template<typename T>
struct params {
    static constexpr T c = 3.0 * 1e8;
    static constexpr T yo = 1e7;
    static constexpr T omega = 1e15;
    static constexpr T omega0 = omega;
    static constexpr T a = 1;
    static constexpr T eps = 1;
};


#endif //DIPLOM_CONST_H
