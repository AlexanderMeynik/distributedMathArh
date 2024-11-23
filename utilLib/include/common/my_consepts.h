
#ifndef DIPLOM_MY_CONSEPTS_H
#define DIPLOM_MY_CONSEPTS_H

#include <eigen3/Eigen/Dense>
#include <type_traits>
#include <concepts>


namespace myconceps {
    template<typename T>
    concept HasBracketOperator = requires(T t, size_t i) {
        { t[i] } -> std::same_as<typename T::value_type &>;  // Ensures operator[] exists and returns a reference to value_type
// { t.size() } -> std::same_as<size_t>;               // Ensures size() exists and returns a size_t
    };


    template<typename T>
    concept HasSizeMethod = requires(T a) {
        { a.size() } -> std::convertible_to<std::size_t>;
    };

    template<typename T>
    concept twodVector=requires(T a, int i, int j) {
        HasSizeMethod<T>;
        HasBracketOperator<T>;
        { a[i][j] } -> std::convertible_to<double>;
    };

    template<typename T>
    concept RowCol = requires(T a) {
        { a.rows() } -> std::convertible_to<std::size_t>;
        { a.cols() } -> std::convertible_to<std::size_t>;
    };

    template<RowCol T>
    std::array<long, 2> get_shape(const T &collection) {
        return {collection.rows(), collection.cols()};
    }

    template<twodVector T>
    std::array<long, 2> get_shape(const T &collection) {
        return {static_cast<long>(collection.size()), static_cast<long>(collection[0].size())};
    }


    template<typename Collection>
    //todo requerements
    const auto& getElement(const Collection &collection, size_t i1,size_t i2,size_t N)
    {
        if constexpr (not std::is_compound_v<typename Collection::value_type>)
        {
            return collection[i1*N+i2];
        }
        else
        {
            return collection[i1][i2];
        }
    }



    template<typename T>
    requires (HasBracketOperator<T>) auto &
    get_value(const T &collection, int i1) {
        return collection[i1];
    }


    template<typename T>
    requires (!twodVector<T>) auto &
    get_value(const T &collection, int i1, int i2) {
        return collection(i1, i2);
    }


    template<typename T>
    requires (twodVector<T>) auto &
    get_value(const T &collection, int i1, int i2) {

        return collection[i1][i2];
    }


}
#endif //DIPLOM_MY_CONSEPTS_H
