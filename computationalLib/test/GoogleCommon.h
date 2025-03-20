#pragma once
#ifndef DIPLOM_GOOGLECOMMON_H
#define DIPLOM_GOOGLECOMMON_H


#include <tuple>
#include <gtest/gtest.h>
#include "common/myConcepts.h"
#include "common/sharedDeclarations.h"
#include "common/printUtils.h"

using namespace myConcepts;
using shared::FloatType;
/// Testing utilities namespace
namespace testCommon {
    constexpr double tool = std::numeric_limits<decltype(tool)>::epsilon();


    /**
     * @brief Custom check for double near
     * @tparam T
     * @param val1
     * @param val2
     * @param abs_error
     */
    template<typename T>
    bool isNear(T val1, T val2, T abs_error) {
        const T diff = std::abs(val1 - val2);
        return diff <= abs_error;
    }


    /**
     * @brief Printer function used to name tests in parameterized test suites
     * @tparam TestSuite
     * @tparam Args
     * @param info
     */
    template<typename TestSuite, typename ...Args>
    auto firstValueTuplePrinter(const testing::TestParamInfo<typename TestSuite::ParamType> &info) {
        return get<0>(info.param);
    }


    /**
     * @brief converts tuple to string with _ as a delimiter
     * @tparam TestSuite
     * @param info
     */
    template<typename TestSuite>
    auto tupleToString(const testing::TestParamInfo<typename TestSuite::ParamType> &info) {
        return printUtils::tupleToString(info.param,"_","","");
    }

    /**
     * @brief Compares two continious collections
     * @tparam Expect - if true will use Expect pred instead of assert
     * @tparam T1
     * @tparam T2
     * @param solution
     * @param solution2
     * @param eqOperator - some user defined comparison function
     * @param tol
     */
    template<bool Expect = false, HasSizeMethod T1, HasSizeMethod T2>
    requires valueTyped<T1> && valueTyped<T2>
             && std::common_with<typename T1::value_type, typename T2::value_type>
    void compareArrays(const T1 &solution,
                       const T2 &solution2,
                       const std::function<bool
                               (
                                       const typename T1::value_type &a,
                                       const typename T2::value_type &b,
                                       size_t i,
                                       FloatType tol
                               )
                       > &eqOperator,
                       FloatType tol = tool
    ) {
        ASSERT_TRUE(solution.size() == solution2.size()) << "Collections have different sizes:("
                                                         << solution.size() << ", " << solution2.size() << ")\n";
        for (size_t i = 0; i < solution.size(); ++i) {
            if constexpr (!Expect) {
                ASSERT_PRED4(eqOperator, solution[i], solution2[i], i, tol);
            } else {
                EXPECT_PRED4(eqOperator, solution[i], solution2[i], i, tol);
            }
        }
    }


    /**
     * @brief Compares 2d arrays
     * @tparam Expect - if true will use Expect pred instead of assert
     * @tparam M1_t
     * @tparam M2_t
     * @param mat1
     * @param mat2
     * @param eqOperator
     * @param tol
     */
    template<bool Expect = false, typename M1_t, typename M2_t>
    void compare2dArrays(const M1_t &mat1, const M2_t &mat2,
                         const std::function<bool
                                 (
                                         const FloatType &a,
                                         const FloatType &b,
                                         size_t i,
                                         size_t j,
                                         FloatType tol
                                 )
                         > &eqOperator,
                         FloatType tol = tool
    ) {
        auto shape = get_shape(mat1);
        auto shape2 = get_shape(mat2);
        int rows = shape[0];
        int cols = shape[1];
        ASSERT_EQ(rows, shape2[0]) << "Matrices has different row counts :("
                                   << rows << ", " << shape2[0] << ")\n";
        ASSERT_EQ(cols, shape2[1]) << "Matrices has different column counts :("
                                   << cols << ", " << shape2[1] << ")\n";


        for (int i = 0; i < rows; ++i) {

            for (int j = 0; j < cols; ++j) {
                if constexpr (!Expect) {
                    ASSERT_PRED5(eqOperator, getMatrElement(mat1, i, j), getMatrElement(mat2, i, j), i, j, tol);
                } else {
                    EXPECT_PRED5(eqOperator, getMatrElement(mat1, i, j), getMatrElement(mat2, i, j), i, j, tol);
                }

            }
        }

    }


    static inline auto arrayDoubleComparator = []<typename FType=FloatType>
            (FType a, FType b, size_t i, FType tol) {
        return isNear(a, b, tol);
    };

    static inline auto twoDArrayDoubleComparator = []<typename FType=FloatType>
            (FType a, FType b, size_t i, size_t j, FType tol) {
        return isNear(a, b, tol);
    };

}


#endif //DIPLOM_GOOGLECOMMON_H
