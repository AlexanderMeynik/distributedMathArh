//
// Created by Lenovo on 23.07.2024.
//

#ifndef DIPLOM_GOOGLECOMMON_H
#define DIPLOM_GOOGLECOMMON_H

#include <chrono>



#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <eigen3/Eigen/Dense>
#include <type_traits>
#include <concepts>


namespace myconceps {
    template<typename T>
    concept HasBracketOperatorAndSize = requires(T t, size_t i) {
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
        HasBracketOperatorAndSize<T>;
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
        return {collection.size(), collection[0].size()};
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
using namespace myconceps;

template<typename... Args, template<typename...> typename Container>
requires HasBracketOperatorAndSize<Container<Args...>>
void
compare_collections(const Container<Args...> &solution, const Container<Args...> &solution2,int ii,double tool) {
    EXPECT_TRUE(solution.size() == solution2.size());
    auto ss = solution2.size();
    for (int i = 0; i < ss; ++i) {
        SCOPED_TRACE("Outer index" + std::to_string(ii) + '\n');
        SCOPED_TRACE("Checked index " + std::to_string(ss) + '\n');
        EXPECT_NEAR(solution[i], solution2[i], tool);
    }
}

template <typename T1, typename T2>
void compare_matrices(const T1& mat1, const T2& mat2,int ii,double  tool) {
    auto shape= get_shape(mat1);
    auto shape2= get_shape(mat2);
    int rows = shape[0];
    int cols = shape[1];
    EXPECT_EQ(rows,shape2[0]);
    EXPECT_EQ(cols,shape2[1]);

    for (int i = 0; i < rows; ++i) {

        for (int j = 0; j < cols; ++j) {

            SCOPED_TRACE("iteration "+std::to_string(ii)+'\n');
            SCOPED_TRACE("Checked index "+std::to_string(i)+'\n');
            SCOPED_TRACE("Checked index "+std::to_string(j)+'\n');
            auto a1=get_value(mat1, i, j);
            auto a2=get_value(mat2, i, j);
            if(a2!=0.0)
            {
                EXPECT_NEAR(a1/a2,1,tool);
            }
            else
            {
                EXPECT_NEAR(a1,0.0,tool);
            }
            //EXPECT_NEAR(get_value(mat1, i, j),get_value(mat2, i, j),tool);
        }
    }
}



#endif //DIPLOM_GOOGLECOMMON_H
