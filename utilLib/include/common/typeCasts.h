#pragma once

#include <map>

#include "commonTypes.h"
#include "myConcepts.h"
#include "errorHandling.h"

using myConcepts::isOneDimensionalContinuous;
using shared::zeroSize;
using shared::RowDivisionError;

/**
 * @brief Casts one dimensional collection to eigen Vector
 * @tparam Collection
 * @param collection
 * @return Eigen::Map<Eigen::Vector<Scalar, Eigen::Dynamic>>
 * @throws shared::zeroSize
 */
template<typename Collection>
requires isOneDimensionalContinuous<Collection>
auto toEigenVector(Collection &collection);

/**
 * @brief Casts one dimensional collection to eigen RowVector
 * @tparam Collection
 * @param collection
 * @return Eigen::Map<Eigen::RowVector<Scalar, Eigen::Dynamic>>
 * @throws shared::zeroSize
 */
template<typename Collection>
requires isOneDimensionalContinuous<Collection>
auto toEigenRowVector(Collection &collection);

/**
 * @brief Casts one dimensional collection to eigen Matrix
 * @tparam Collection
 * @param collection
 * @param columns
 * @return Eigen::Map<Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic>>
 */
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
        throw zeroSize(VARIABLE_NAME(collection));
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
        throw zeroSize(VARIABLE_NAME(collection));
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

    using EigenMatrix = Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic>;
    using MapType = std::conditional_t<is_const,
            Eigen::Map<const EigenMatrix>,
            Eigen::Map<EigenMatrix>
    >;

    if (collection.size() == 0) {
        throw zeroSize(VARIABLE_NAME(collection));
    }

    if (collection.size() % columns != 0) {

        throw RowDivisionError(collection.size(), columns);
    }
    size_t rows = collection.size() / columns;

    return MapType(&collection[0], rows, columns);
}
