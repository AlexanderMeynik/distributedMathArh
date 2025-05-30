#pragma once

#include <type_traits>
#include <concepts>
#include <array>

using size_t = decltype(sizeof 1);

//my_concepts namespace
namespace my_concepts {

/**
 * @brief concept to check whether T has value_type
 * @tparam T
 */
template<typename T>
concept valueTyped = requires { typename T::value_type; };

/**
 * @brief Is type a specialization of
 */
template<typename, template<typename...> class>
struct is_specialization_of : std::false_type {};

/**
 * @brief Is type a specialization of
 * @tparam C
 * @tparam Args
 */
template<template<typename...> class C, typename... Args>
struct is_specialization_of<C<Args...>, C> : std::true_type {};

/**
 * @brief Concept to check whether type T has subscript operator
 * @tparam T
 */
template<typename T>
concept HasBracketOperator = valueTyped<T> && requires(T t, size_t i) {
  { t[i] };
};

/**
 * @brief Concept to check whether type T hash size method
 * @tparam T
 */
template<typename T>
concept HasSizeMethod = requires(T a) {
  { a.size() } -> std::convertible_to<std::size_t>;
};

/**
 * @brief Checks whether T is a one dimensional continuous array of elements
 * @tparam T
 */
template<typename T>
concept isOneDimensionalContinuous = HasBracketOperator<T> && HasSizeMethod<T>;

/**
 * @brief Concept to check whether type T element have subscript operation
 * @tparam T
 */
template<typename T>
concept HasBracketsNested = HasBracketOperator<T> && HasBracketOperator<typename T::value_type>;

/**
 * @brief Concept to check whether type T is 2d vector
 * @tparam T
 */
template<typename T>
concept twodVector=
HasSizeMethod<T> &&
    HasBracketOperator<T> &&
    HasBracketsNested<T> &&
    HasSizeMethod<typename T::value_type>;

/**
 * @brief Concept to check whether type T has rows and columns methods
 * @tparam T
 */
template<typename T>
concept RowCol = requires(T a) {
  { a.rows() } -> std::convertible_to<std::size_t>;
  { a.cols() } -> std::convertible_to<std::size_t>;
};

/*
 * @brief Returns dimentsions of structure
 * @tparam T
 * @param collection
 * @return
 */
template<RowCol T>
std::array<long, 2> GetShape(const T &collection) {
  return {collection.rows(), collection.cols()};
}

/**
 * @brief Returns dimentsions of structure
 * @tparam T
 * @param collection
 * @return
 */
template<twodVector T>
std::array<long, 2> GetShape(const T &collection) {
  return {static_cast<long>(collection.size()), static_cast<long>(collection[0].size())};
}

template<typename Container>
concept parenthesisOperator = requires(Container cont, size_t i1, size_t i2)
{
  { cont(i1, i2) }->std::common_with<typename Container::value_type>;
};

template<typename Collection>
requires HasBracketsNested<Collection> || parenthesisOperator<Collection>
auto &GetMatrElement(const Collection &collection, size_t i1, size_t i2) {
  if constexpr (parenthesisOperator<Collection>) {
    return collection(i1, i2);
  } else {
    return collection[i1][i2];
  }
}

/**
 * @brief Specializes return method depending on dimensions of Collection
 * @tparam Collection
 * @param collection
 * @param i1
 * @param i2
 * @param n
 * @details if collection is 2 dimensional array then it's equivalent to collections[i1][i1]
 * @details in other case it's use collection[i1 * N + i2]
 */
template<typename Collection>
requires HasBracketOperator<Collection>
const auto &GetElement(const Collection &collection, size_t i1, size_t i2, size_t n) {
  if constexpr (not HasBracketsNested<Collection>) {
    return collection[i1 * n + i2];
  } else {
    return collection[i1][i2];
  }
}

template<typename T>
requires (HasBracketOperator<T>) auto &
GetValue(const T &collection, int i1) {
  return collection[i1];
}

template<typename T>
requires (!twodVector<T>) auto &
GetValue(const T &collection, int i1, int i2) {
  return collection(i1, i2);
}

template<typename T>
requires (twodVector<T>) auto &
GetValue(const T &collection, int i1, int i2) {

  return collection[i1][i2];
}
}
