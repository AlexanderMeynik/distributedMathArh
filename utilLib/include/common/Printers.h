

#ifndef DIPLOM_PRINTERS_H
#define DIPLOM_PRINTERS_H

#include <vector>


#include <eigen3/Eigen/Dense>


#include "printUtils.h"
#include "common/myConcepts.h"

template<class T>
void printSolutionFormat1(std::ostream &out, std::vector<T> &solution) {
    Eigen::Map<Eigen::Vector<T, Eigen::Dynamic>> solution_(solution.data(), solution.size());
    out << "Решение системы диполей\n Ai(x\\ny)\tBi(x\\ny)\tCi(x\\ny)\n";//todo copied code(that can be temlated)
    int N_ = solution_.size() / 4.0;
    for (int i = 0; i < N_; ++i) {
        auto cx = sqrt(solution_.coeffRef(2 * i) * solution_.coeffRef(2 * i) +
                       solution_.coeffRef(2 * i + 2 * N_) * solution_.coeffRef(2 * i + 2 * N_));
        auto cy = sqrt(solution_.coeffRef(2 * i + 1) * solution_.coeffRef(2 * i + 1) +
                       solution_.coeffRef(2 * i + 1 + 2 * N_) * solution_.coeffRef(2 * i + 1 + 2 * N_));
        IosStatePreserve state(out);
        out << std::scientific;

        out << solution_.coeffRef(2 * i) << "\t"
            << solution_.coeffRef(2 * i + 2 * N_) << "\t"
            << cx << "\n";
        out << solution_.coeffRef(2 * i + 1) << "\t"
            << solution_.coeffRef(2 * i + 1 + 2 * N_) << "\t"
            << cy << "\n";
    }
}


template<class T>
void printSolutionFormat1(std::ostream &out, Eigen::Vector<T, Eigen::Dynamic> &solution_) {
    out << "Решение системы диполей\n Ai(x\\ny)\tBi(x\\ny)\tCi(x\\ny)\n";
    int N_ = solution_.size() / 4.0;
    for (int i = 0; i < N_; ++i) {
        auto cx = sqrt(solution_.coeffRef(2 * i) * solution_.coeffRef(2 * i) +
                       solution_.coeffRef(2 * i + 2 * N_) * solution_.coeffRef(2 * i + 2 * N_));
        auto cy = sqrt(solution_.coeffRef(2 * i + 1) * solution_.coeffRef(2 * i + 1) +
                       solution_.coeffRef(2 * i + 1 + 2 * N_) * solution_.coeffRef(2 * i + 1 + 2 * N_));
        IosStatePreserve state(out);
        out << std::scientific;

        out << solution_.coeffRef(2 * i) << "\t"
            << solution_.coeffRef(2 * i + 2 * N_) << "\t"
            << cx << "\n";
        out << solution_.coeffRef(2 * i + 1) << "\t"
            << solution_.coeffRef(2 * i + 1 + 2 * N_) << "\t"
            << cy << "\n";
    }
}

template<class T>
void plotCoordinates(std::string name, T ar, std::array<std::vector<T>, 2> &xi) {
    auto ax = matplot::gca();
    ax->scatter(xi[0], xi[1]);
    ax->xlim({-8 * ar, 8 * ar});
    ax->ylim({-8 * ar, 8 * ar});
    matplot::save(name);
    ax.reset();
}


template<class T>
void printSolution(std::ostream &out, std::vector<T> &solution_, Eigen::IOFormat format = Eigen::IOFormat()) {
    Eigen::Map<Eigen::Vector<T, Eigen::Dynamic>> map(solution_.data(), solution_.size());//todo copy impl(as template
    out << "Вектор решения\n" << map.format(format) << "\n";
}

template<class T>
void printSolution(std::ostream &out, Eigen::Vector<T, Eigen::Dynamic> &solution_,
                   Eigen::IOFormat format = Eigen::IOFormat()) {
    out << "Вектор решения\n" << solution_.format(format) << "\n";
}


template<typename Collection>
void printCoordinates2(std::ostream &out, const Collection &xi) {
    out << "Координаты диполей\n";

    if constexpr (not myConcepts::HasBracketsNested<Collection>) {
        auto N = xi.size() / 2;
        for (int i = 0; i < N; ++i) {
            out << xi[i] << '\t' << xi[i + N] << "\n";
        }
    } else {
        auto N = xi[0].size();
        for (int i = 0; i < N; ++i) {
            out << xi[0][i] << '\t' << xi[1][i] << "\n";
        }
    }
}


template<class T>
void printCoordinates(std::ostream &out, Eigen::Vector<T, Eigen::Dynamic> &xi) {
    out << "Координаты диполей\n";
    auto N = xi.size() / 2;
    for (int i = 0; i < N; ++i) {
        out << xi[i] << '\t' << xi[i + N] << "\n";
    }
}


#endif //DIPLOM_PRINTERS_H
