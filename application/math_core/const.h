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
#include "../common/printUtils.h"

template<typename T>
struct params {
    static constexpr T c = 3.0 * 1e8;
    static constexpr T yo = 1e7;
    static constexpr T omega = 1e15;
    static constexpr T omega0 = omega;
    static constexpr T a = 1;
    static constexpr T eps = 1;
};

template<class T>
void printCoordinates(std::ostream &out,std::array<std::vector<T>, 2> &xi) {
    out << "Координаты диполей\n";
    for (int i = 0; i < xi[0].size(); ++i) {
        out << xi[0][i] << '\t' << xi[1][i] << "\n";
    }
}


template<class T>
void printSolution(std::ostream &out, Eigen::IOFormat &format,std::array<Eigen::Vector<T, Eigen::Dynamic>, 2>& solution_);


template <class T>
void printSolutionFormat1(std::ostream &out,std::array<Eigen::Vector<T, Eigen::Dynamic>, 2>& solution_);

template <class T>
void plotCoordinates(std::string name, T ar,std::array<std::vector<T>, 2> &xi);

template <typename T>
std::istream& operator>>(std::istream& in, std::array<std::vector<T>, 2>& xi) {
    size_t size;
    in>>size;
    for (int i = 0; i < 2; ++i) {
        xi[i].resize(size);
        for (int j = 0; j < size; ++j) {
            in >> xi[i][j];
        }
    }
    return in;
}

template <typename T>
std::istream& operator>>(std::istream& in, std::array<Eigen::Vector<T, Eigen::Dynamic>, 2>& sol) {
    size_t size;
    in>>size;
    for (int i = 0; i < 2; ++i) {
        sol[i].resize(size);
        for (int j = 0; j < size; ++j) {
            in >> sol[i][j];
        }
    }
    return in;
}


template<class T>
void printSolution(std::ostream &out, Eigen::IOFormat &format,std::array<Eigen::Vector<T, Eigen::Dynamic>, 2>& solution_) {
    out << "Вектор решения\n" << solution_[0].format(format) << '\n' << solution_[1].format(format) << "\n\n";
}


template<class T>
void printSolutionFormat1(std::ostream &out,std::array<Eigen::Vector<T, Eigen::Dynamic>, 2>& solution_) {
    out << "Решение системы диполей\n Ai(x\\ny)\tBi(x\\ny)\tCi(x\\ny)\n";
    int N_=solution_[0].size()/2.0;
    for (int i = 0; i < N_; ++i) {
        auto cx = sqrt(solution_[0].coeffRef(2 * i) * solution_[0].coeffRef(2 * i) +
                       solution_[1].coeffRef(2 * i) * solution_[1].coeffRef(2 * i));
        auto cy = sqrt(solution_[0].coeffRef(2 * i + 1) * solution_[0].coeffRef(2 * i + 1) +
                       solution_[1].coeffRef(2 * i + 1) * solution_[1].coeffRef(2 * i + 1));
        IosStatePreserve state(out);
        out << std::scientific;

        out << solution_[0].coeffRef(2 * i) << "\t"
            << solution_[1].coeffRef(2 * i) << "\t"
            << cx << "\n";
        out << solution_[0].coeffRef(2 * i + 1) << "\t"
            << solution_[1].coeffRef(2 * i + 1) << "\t"
            << cy << "\n";
    }
}

template<class T>
void plotCoordinates(std::string name, T ar,std::array<std::vector<T>, 2> &xi) {
    auto ax = matplot::gca();
    ax->scatter(xi[0], xi[1]);
    ax->xlim({-8 * ar, 8 * ar});
    ax->ylim({-8 * ar, 8 * ar});
    matplot::save(name);
    ax.reset();
}

#endif //DIPLOM_CONST_H
