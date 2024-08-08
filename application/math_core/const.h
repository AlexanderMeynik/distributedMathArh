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
void printCoordinates(std::ostream &out,Eigen::Vector<T , Eigen::Dynamic> &xi) {
    out << "Координаты диполей\n";
    auto N=xi.size()/2;
    for (int i = 0; i <N; ++i) {
        out << xi[i] << '\t' << xi[i+N] << "\n";
    }
}

/*
template<class T>
void printSolution(std::ostream &out, Eigen::IOFormat &format,std::array<Eigen::Vector<T, Eigen::Dynamic>, 2>& solution_);
*/

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
void printSolution(std::ostream &out,std::vector<T>& solution_,Eigen::IOFormat format=Eigen::IOFormat()) {
    Eigen::Map<Eigen::Vector<T, Eigen::Dynamic>> map(solution_.data(),solution_.size());//todo copy impl(as template
    out << "Вектор решения\n" << map.format(format) << "\n";
}
template<class T>
void printSolution(std::ostream &out,Eigen::Vector<T, Eigen::Dynamic>& solution_,Eigen::IOFormat format=Eigen::IOFormat()) {
    out << "Вектор решения\n" << solution_.format(format) << "\n";
}


template<typename Container>//todo вот этому всему тут не место
class Parser
{
public:
    Parser() : vals_() {}

    Container vals_;
};

template<typename T>
class Parser<Eigen::Vector<T,-1>>
{
public:
    Parser() : size_(0), vals_() {}

    Parser(int size) : size_(size), vals_(size) {}

    friend std::istream& operator>>(std::istream& in, Parser& pp)
    {
        in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::string dummy;
        std::getline(in, dummy);

        pp.vals_.resize(4*pp.size_);
        for (int i = 0; i < 4*pp.size_; ++i) {
            T val;
            in>>val;
            pp.vals_(i)=val;
        }
        return in;
    }

    Eigen::Vector<T,-1> vals_;
    int size_;
};


template<typename T>
class Parser<Eigen::Matrix<T,Eigen::Dynamic,Eigen::Dynamic>>
{
public:
    Parser() : size_(0), vals_() {}

    Parser(int size) : size_(size), vals_() {}

    friend std::istream& operator>>(std::istream& in, Parser& pp)
    {
        in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        pp.vals_.resize(4*pp.size_,4*pp.size_);

       // pp.vals_.resize(4,4);
        //in>>pp.vals_;

        for(int i = 0; i <4*pp.size_; ++i) {
            for(int j = 0; j < 4*pp.size_; ++j) {
                in >> pp.vals_(i, j);
            }
        }
        return in;
    }

    Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> vals_;
    int size_;
};













template<class T>
void printSolutionFormat1(std::ostream &out,std::vector<T>& solution) {
    Eigen::Map<Eigen::Vector<T, Eigen::Dynamic>> solution_(solution.data(),solution.size());
    out << "Решение системы диполей\n Ai(x\\ny)\tBi(x\\ny)\tCi(x\\ny)\n";//todo copied code(that can be temlated)
    int N_=solution_.size()/4.0;
    for (int i = 0; i < N_; ++i) {
        auto cx = sqrt(solution_.coeffRef(2 * i) * solution_.coeffRef(2 * i) +
                       solution_.coeffRef(2 * i+2*N_) * solution_.coeffRef(2 * i+2*N_));
        auto cy = sqrt(solution_.coeffRef(2 * i + 1) * solution_.coeffRef(2 * i + 1) +
                       solution_.coeffRef(2 * i + 1+2*N_) * solution_.coeffRef(2 * i + 1+2*N_));
        IosStatePreserve state(out);
        out << std::scientific;

        out << solution_.coeffRef(2 * i) << "\t"
            << solution_.coeffRef(2 * i+2*N_) << "\t"
            << cx << "\n";
        out << solution_.coeffRef(2 * i + 1) << "\t"
            << solution_.coeffRef(2 * i + 1+2*N_) << "\t"
            << cy << "\n";
    }
}


template<class T>
void printSolutionFormat1(std::ostream &out,Eigen::Vector<T, Eigen::Dynamic>& solution_) {
    out << "Решение системы диполей\n Ai(x\\ny)\tBi(x\\ny)\tCi(x\\ny)\n";
    int N_=solution_.size()/4.0;
    for (int i = 0; i < N_; ++i) {
        auto cx = sqrt(solution_.coeffRef(2 * i) * solution_.coeffRef(2 * i) +
                       solution_.coeffRef(2 * i+2*N_) * solution_.coeffRef(2 * i+2*N_));
        auto cy = sqrt(solution_.coeffRef(2 * i + 1) * solution_.coeffRef(2 * i + 1) +
                       solution_.coeffRef(2 * i + 1+2*N_) * solution_.coeffRef(2 * i + 1+2*N_));
        IosStatePreserve state(out);
        out << std::scientific;

        out << solution_.coeffRef(2 * i) << "\t"
            << solution_.coeffRef(2 * i+2*N_) << "\t"
            << cx << "\n";
        out << solution_.coeffRef(2 * i + 1) << "\t"
            << solution_.coeffRef(2 * i + 1+2*N_) << "\t"
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
