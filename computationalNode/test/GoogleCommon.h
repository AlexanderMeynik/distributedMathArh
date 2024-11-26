
#ifndef DIPLOM_GOOGLECOMMON_H
#define DIPLOM_GOOGLECOMMON_H

#include <chrono>


#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <eigen3/Eigen/Dense>
#include <type_traits>
#include <concepts>


using namespace myConcepts;

namespace testCommon {
    constexpr double tool = std::numeric_limits<decltype(tool)>::epsilon();

    template<HasSizeMethod T1, HasSizeMethod T2>
    requires valueTyped<T1>&&valueTyped<T2>
            && std::common_with<typename T1::value_type,typename T2::value_type>
    void compareArrays(const T1 &solution,
                       const T2 &solution2,
                       const std::function<bool
                        (const typename T1::value_type & a,const typename T2::value_type & b,size_t i)>& eqOperator
                        )
    {
        ASSERT_TRUE(solution.size() == solution2.size())<< "Collections have different sizes:("
                                                        <<solution.size()<<", "<<solution2.size()<<")\n";


        for (size_t i = 0; i < solution.size(); ++i) {
            ASSERT_PRED3(eqOperator,solution[i],solution2[i],i);
        }

    }


    template<HasSizeMethod T1, HasSizeMethod T2>
    [[deprecated("use compareArrays")]] void compare_collections (const T1 &solution, const T2 &solution2, int ii, double tool) {
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
}


#endif //DIPLOM_GOOGLECOMMON_H
