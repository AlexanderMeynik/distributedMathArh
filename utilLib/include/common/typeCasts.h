

#ifndef MAGISTER1_LIB_H
#define MAGISTER1_LIB_H

#include <string>
#include <map>

#include "commonTypes.h"
#include "myConcepts.h"


using commonDeclarations::isOneDimensionalContinuous;


//todo array<coll,2>->vector or matrix(2 columns)

template<typename Container>
requires isOneDimensionalContinuous<Container>
Eigen::Map<Eigen::Vector<std::remove_reference_t<typename Container::value_type>
        , Eigen::Dynamic>>
toEigenVector(Container&container);

template<typename Container>
requires isOneDimensionalContinuous<Container>
Eigen::Map<Eigen::RowVector<std::remove_reference_t<typename Container::value_type>
        , Eigen::Dynamic>>
toEigenRowVector(Container&container);

template<typename Container>
requires isOneDimensionalContinuous<Container>
Eigen::Map<Eigen::Matrix<std::remove_reference_t<typename Container::value_type>
        , Eigen::Dynamic,Eigen::Dynamic>>
toEigenMatrix(Container&container, int columns);


template<class T>
std::array<std::vector<T>, 2> reinterpretVector(Eigen::Vector<T, Eigen::Dynamic> &xi) {
    auto N = xi.size() / 2;
    if (!N) {
        return std::array<std::vector<T>, 2>();
    }
    std::array<std::vector<T>, 2> res;
    res[0] = std::vector<T>(N, 0);
    res[1] = std::vector<T>(N, 0);

    for (int i = 0; i < N; ++i) {
        res[0][i] = xi[i];
        res[1][i] = xi[i + N];
    }
    return res;
}

enum class state_t {
    new_,
    old,
    openmp_new,
    openmp_old,
    print_
};

const static std::map<state_t, std::string> stateToString = {
        {state_t::openmp_new, "openmp_new"},
        {state_t::new_,       "new"},
        {state_t::openmp_old, "openmp_old"},
        {state_t::old,        "old"},
        {state_t::print_,     "printImpl"},
};
const static std::map<std::string, state_t> stringToState = {
        {"openmp_new", state_t::openmp_new},
        {"new",        state_t::new_},
        {"openmp_old", state_t::openmp_old},
        {"old",        state_t::old},
        {"printImpl",      state_t::print_},
};
std::ostream& operator<<  (std::ostream &out,const state_t&st);





template<typename Container>
requires isOneDimensionalContinuous<Container>
Eigen::Map<Eigen::Vector<std::remove_reference_t<typename Container::value_type>
        , Eigen::Dynamic>>
toEigenVector(Container&container)
{
    if(container.size() == 0) {
        throw std::length_error("Zero input container size!");
    }


    Eigen::Map<Eigen::Vector<std::remove_reference_t<typename Container::value_type>
            , Eigen::Dynamic>> map(&container[0], container.size());


    return map;
}

template<typename Container>
requires isOneDimensionalContinuous<Container>
Eigen::Map<Eigen::RowVector<std::remove_reference_t<typename Container::value_type>
        , Eigen::Dynamic>>
toEigenRowVector(Container&container)
{
    if(container.size() == 0) {
        throw std::length_error("Zero input container size!");
    }


    Eigen::Map<Eigen::RowVector<std::remove_reference_t<typename Container::value_type>
            , Eigen::Dynamic>> map(&container[0], container.size());


    return map;
}

template<typename Container>
requires isOneDimensionalContinuous<Container>
Eigen::Map<Eigen::Matrix<std::remove_reference_t<typename Container::value_type>
        , Eigen::Dynamic,Eigen::Dynamic>>
toEigenMatrix(Container&container, int columns)
{
    if(container.size() == 0) {
        throw std::length_error("Zero input container size!");
    }

    if(container.size() % columns != 0) {
        throw std::length_error("Invalid solutions size to columns ratio: container.size() % columns = "+std::to_string(container.size() % columns));
    }
    size_t rows= container.size() / columns;


    Eigen::Map<Eigen::Matrix<std::remove_reference_t<typename Container::value_type>
            , Eigen::Dynamic,Eigen::Dynamic>> map(&container[0], rows, columns);


    return map;
}

#endif //MAGISTER1_LIB_H
