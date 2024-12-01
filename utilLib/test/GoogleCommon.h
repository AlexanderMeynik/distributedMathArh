
#ifndef DIPLOM_GOOGLECOMMON_H
#define DIPLOM_GOOGLECOMMON_H

#include <chrono>


#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <eigen3/Eigen/Dense>
#include <type_traits>
#include <concepts>
#include "common/myConcepts.h"

using namespace commonDeclarations;

template<HasSizeMethod T1, HasSizeMethod T2>
void compare_collections(const T1 &solution, const T2 &solution2, int ii, double tool) {
    EXPECT_TRUE(solution.size() == solution2.size());
    auto ss = solution2.size();
    for (int i = 0; i < ss; ++i) {
        SCOPED_TRACE("Outer index" + std::to_string(ii) + '\n');
        SCOPED_TRACE("Checked index " + std::to_string(i) + '\n');

        auto a1 = get_value(solution, i);
        auto a2 = get_value(solution, i);
        SCOPED_TRACE("Values to check:" + std::to_string(a1) + "," + std::to_string(a2));
        if (std::abs(a2) >= tool) {
            EXPECT_NEAR(a1 / a2, 1, tool);
        } else {
            EXPECT_NEAR(a1, 0.0, tool);
        }
    }
}

template<typename T1, typename T2>
void compare_matrices(const T1 &mat1, const T2 &mat2, int ii, double tool) {
    auto shape = get_shape(mat1);
    auto shape2 = get_shape(mat2);
    int rows = shape[0];
    int cols = shape[1];
    EXPECT_EQ(rows, shape2[0]);
    EXPECT_EQ(cols, shape2[1]);

    for (int i = 0; i < rows; ++i) {

        for (int j = 0; j < cols; ++j) {

            SCOPED_TRACE("iteration " + std::to_string(ii) + '\n');
            SCOPED_TRACE("Checked index " + std::to_string(i) + '\n');
            SCOPED_TRACE("Checked index " + std::to_string(j) + '\n');
            auto a1 = get_value(mat1, i, j);
            auto a2 = get_value(mat2, i, j);
            //std::stringstream ss;

            SCOPED_TRACE("Values to check:" + std::to_string(a1) + "," + std::to_string(a2));
            if (std::abs(a2) >= tool) {
                EXPECT_NEAR(a1 / a2, 1.0, tool);
            } else {
                EXPECT_NEAR(a1, 0.0, tool);
            }
        }
    }
}


#endif //DIPLOM_GOOGLECOMMON_H
