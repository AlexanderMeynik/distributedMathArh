

#ifndef MAGISTER1_LIB_H
#define MAGISTER1_LIB_H

#include <map>

#include "commonTypes.h"
#include "myConcepts.h"

using myConcepts::isOneDimensionalContinuous;

template<typename Collection>
requires isOneDimensionalContinuous<Collection>
auto toEigenVector(Collection &collection);

template<typename Collection>
requires isOneDimensionalContinuous<Collection>
auto toEigenRowVector(Collection &collection);

template<typename Collection>
requires isOneDimensionalContinuous<Collection>
auto
toEigenMatrix(Collection &collection, int columns);


template<typename Collection>
requires isOneDimensionalContinuous<Collection>
auto
toEigenVector(Collection &collection) {
    using Scalar = std::remove_reference_t<typename Collection::value_type>;
    using DataPtr = decltype(&collection[0]);

    constexpr bool is_const = std::is_const_v<std::remove_pointer_t<DataPtr>>;

    using EigenVector = Eigen::Vector<Scalar, Eigen::Dynamic>;
    using MapType = std::conditional_t<is_const,
            Eigen::Map<const EigenVector>,
            Eigen::Map<EigenVector>
    >;

    if (collection.size() == 0) {
        throw std::length_error("Zero input collection size!");
    }

    return MapType(&collection[0], collection.size());
}

template<typename Collection>
requires isOneDimensionalContinuous<Collection>
auto
toEigenRowVector(Collection &collection) {

    using Scalar = std::remove_reference_t<typename Collection::value_type>;
    using DataPtr = decltype(&collection[0]);

    constexpr bool is_const = std::is_const_v<std::remove_pointer_t<DataPtr>>;

    using EigenVector = Eigen::RowVector<Scalar, Eigen::Dynamic>;
    using MapType = std::conditional_t<is_const,
            Eigen::Map<const EigenVector>,
            Eigen::Map<EigenVector>
    >;

    if (collection.size() == 0) {
        throw std::length_error("Zero input collection size!");
    }

    return MapType(&collection[0], collection.size());
}

template<typename Collection>
requires isOneDimensionalContinuous<Collection>
auto
toEigenMatrix(Collection &collection, int columns) {

    using Scalar = std::remove_reference_t<typename Collection::value_type>;
    using DataPtr = decltype(&collection[0]);

    constexpr bool is_const = std::is_const_v<std::remove_pointer_t<DataPtr>>;

    using EigenMatrix = Eigen::Matrix<std::remove_reference_t<typename Collection::value_type>, Eigen::Dynamic, Eigen::Dynamic>;
    using MapType = std::conditional_t<is_const,
            Eigen::Map<const EigenMatrix>,
            Eigen::Map<EigenMatrix>
    >;

    if (collection.size() == 0) {
        throw std::length_error("Zero input collection size!");
        //todo redo
    }

    if (collection.size() % columns != 0) {
        throw std::length_error("Invalid collection to columns mod: collection.size() % columns = " +
                                std::to_string(collection.size() % columns));
    }
    size_t rows = collection.size() / columns;


    return MapType (&collection[0], rows, columns);
}

#endif //MAGISTER1_LIB_H
