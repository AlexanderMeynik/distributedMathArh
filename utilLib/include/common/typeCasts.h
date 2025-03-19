

#ifndef MAGISTER1_LIB_H
#define MAGISTER1_LIB_H

#include <map>

#include "commonTypes.h"
#include "myConcepts.h"

using myConcepts::isOneDimensionalContinuous;


//todo test implementations
template<typename Container>
requires isOneDimensionalContinuous<Container>
        auto
/*Eigen::Map<Eigen::Vector<std::remove_reference_t<typename Container::value_type>, Eigen::Dynamic>>*/
toEigenVector(Container &container);



template<typename Container>
requires isOneDimensionalContinuous<Container>
auto
toEigenRowVector(Container &container);

template<typename Container>
requires isOneDimensionalContinuous<Container>
Eigen::Map<Eigen::Matrix<std::remove_reference_t<typename Container::value_type>, Eigen::Dynamic, Eigen::Dynamic>>
toEigenMatrix(Container &container, int columns);


//todo remove(testing type casts)
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

template<typename Container>
requires isOneDimensionalContinuous<Container>
auto
toEigenVector(Container &container) {
    using Scalar = std::remove_reference_t<typename Container::value_type>;
    using DataPtr = decltype(&container[0]);

    constexpr bool is_const = std::is_const_v<std::remove_pointer_t<DataPtr>>;

    using EigenVector = Eigen::Vector<Scalar, Eigen::Dynamic>;
    using MapType = std::conditional_t<is_const,
            Eigen::Map<const EigenVector>,
            Eigen::Map<EigenVector>
    >;

    if (container.size() == 0) {
        throw std::length_error("Zero input container size!");
    }

    return MapType(&container[0], container.size());
}




template<typename Container>
requires isOneDimensionalContinuous<Container>
auto
toEigenRowVector(Container &container) {

    using Scalar = std::remove_reference_t<typename Container::value_type>;
    using DataPtr = decltype(&container[0]);

    constexpr bool is_const = std::is_const_v<std::remove_pointer_t<DataPtr>>;

    using EigenVector = Eigen::RowVector<Scalar, Eigen::Dynamic>;
    using MapType = std::conditional_t<is_const,
            Eigen::Map<const EigenVector>,
            Eigen::Map<EigenVector>
    >;

    if (container.size() == 0) {
        throw std::length_error("Zero input container size!");
    }

    return MapType(&container[0], container.size());
}

template<typename Container>
requires isOneDimensionalContinuous<Container>
Eigen::Map<Eigen::Matrix<std::remove_reference_t<typename Container::value_type>, Eigen::Dynamic, Eigen::Dynamic>>
toEigenMatrix(Container &container, int columns) {
    if (container.size() == 0) {
        throw std::length_error("Zero input container size!");
    }

    if (container.size() % columns != 0) {
        throw std::length_error("Invalid solutions size to columns ratio: container.size() % columns = " +
                                std::to_string(container.size() % columns));
    }
    size_t rows = container.size() / columns;


    Eigen::Map<Eigen::Matrix<std::remove_reference_t<typename Container::value_type>, Eigen::Dynamic, Eigen::Dynamic>> map(
            &container[0], rows, columns);


    return map;
}

#endif //MAGISTER1_LIB_H
