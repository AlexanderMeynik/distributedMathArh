

#ifndef DIPLOM_PRINTERS_H
#define DIPLOM_PRINTERS_H

#include <vector>


#include <eigen3/Eigen/Dense>

#include <matplot/matplot.h>

#include "common/printUtils.h"
#include "common/myConcepts.h"
#include "computationalLib/math_core/MeshCreator.h"

template<typename PrintType>
int floatPrinter(std::ostream&out,const PrintType& printee,int N=std::numeric_limits<FloatType>::digits10-1);







void  printDec(meshStorage::MeshCreator&mmesh,std::ostream &out,int N=std::numeric_limits<FloatType>::digits10-1);


template<typename Container>
requires commonDeclarations::isOneDimensionalContinuous<Container> && std::is_floating_point_v<typename Container::value_type>
int printSolutionFormat1(std::ostream &out, const Container &solution)
{
    out << "Решение системы диполей\n Ai(x\\ny)\tBi(x\\ny)\tCi(x\\ny)\n";
    int N_ = solution.size() / 4.0;

    for (int i = 0; i < N_; i++) {
        auto cx = sqrt(solution[2 * i] * solution[2 * i] +
                       solution[2 * i + 2 * N_] * solution[2 * i + 2 * N_]);
        auto cy = sqrt(solution[2 * i + 1] * solution[2 * i + 1] +
                       solution[2 * i + 1 + 2 * N_] * solution[2 * i + 1 + 2 * N_]);
        IosStatePreserve state(out);
        out << std::scientific;

        out << solution[2 * i] << "\t"
            << solution[2 * i + 2 * N_] << "\t"
            << cx << "\n";
        out << solution[2 * i + 1] << "\t"
            << solution[2 * i + 1 + 2 * N_] << "\t"
            << cy << "\n";

    }
    return 0;
}

template<typename T>
concept has_data=requires(T& t)
{
    {t.data()}->std::common_with<typename T::value_type*>;
};

/*template< typename T>
requires std::is_floating_point_v<T>
int printEigenVectorMap(Eigen::Map<Eigen::Vector<T
        , Eigen::Dynamic>>&map, const Eigen::IOFormat& format = Eigen::IOFormat())
{

}*/
//todo test
//todo play with formats https://eigen.tuxfamily.org/dox/structEigen_1_1IOFormat.html


//todo printer for eigen things

template<typename Container>
requires has_data<Container>&&commonDeclarations::HasSizeMethod<Container>
int printVectorEigen(std::ostream &out,const Container &solution_, const Eigen::IOFormat& format = Eigen::IOFormat())
{
    Eigen::Map<Eigen::Vector<std::remove_reference<typename Container::value_type>
            , Eigen::Dynamic>> map(solution_.data(), solution_.size());
    out<<map.format(format);
    return 0;
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

    if constexpr (not commonDeclarations::HasBracketsNested<Collection>) {
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
