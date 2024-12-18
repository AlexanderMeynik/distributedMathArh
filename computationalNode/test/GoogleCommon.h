#pragma once
#ifndef DIPLOM_GOOGLECOMMON_H
#define DIPLOM_GOOGLECOMMON_H

#include <chrono>
#include <type_traits>
#include <concepts>
#include <tuple>

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <eigen3/Eigen/Dense>

#include "common/myConcepts.h"


using namespace commonDeclarations;

namespace testCommon {
    constexpr double tool = std::numeric_limits<decltype(tool)>::epsilon();




    template<typename T>
    bool is_near(T val1, T val2, T abs_error) {
        const T diff = std::abs(val1 - val2);
        return  diff <= abs_error;
    }


    template<typename TestSuite,typename ...Args>
    auto firstValueTuplePrinter(const testing::TestParamInfo<typename TestSuite::ParamType> &info) {
        return get<0>(info.param);
    }


    template<class TupType, size_t... I>
    void print(std::ostream &out,const TupType& _tup, std::index_sequence<I...>,const char* delim=", ",const char* start="(",const char* end=")")
    {
        out << start;
        (..., (out << (I == 0? "" : delim) << std::get<I>(_tup)));
        out << end;
    }

    template<class... T>
    void print (std::ostream &out,const std::tuple<T...>& _tup,const char* delim=", ",const char* start="(",const char* end=")")
    {
        print(out,_tup, std::make_index_sequence<sizeof...(T)>(),delim,start,end);
    }

    template<typename TestSuite>
    auto tupleToString(const testing::TestParamInfo<typename TestSuite::ParamType> &info) {
        std::stringstream result;
        print(result,info.param,"_","","");

        return result.str();
    }

    template<bool Expect=false,HasSizeMethod T1, HasSizeMethod T2>
    requires valueTyped<T1>&&valueTyped<T2>
            && std::common_with<typename T1::value_type,typename T2::value_type>
    void compareArrays(const T1 &solution,
                       const T2 &solution2,
                       const std::function<bool
                        (
                                const typename T1::value_type & a,
                                const typename T2::value_type & b,
                                size_t i,
                                FloatType tol
                                )
                                >& eqOperator,
                                FloatType tol=tool
                        )
    {
        ASSERT_TRUE(solution.size() == solution2.size())<< "Collections have different sizes:("
                                                        <<solution.size()<<", "<<solution2.size()<<")\n";
        for (size_t i = 0; i < solution.size(); ++i) {
            if constexpr (!Expect)
            {
                ASSERT_PRED4(eqOperator, solution[i], solution2[i], i, tol);
            }
            else
            {
                EXPECT_PRED4(eqOperator, solution[i], solution2[i], i, tol);
            }
        }
    }




    template<bool Expect=false,typename M1_t,typename M2_t>
    void compare2dArrays(const M1_t &mat1, const M2_t &mat2,
                         const std::function<bool
    (
                    const FloatType & a,
                    const FloatType & b,
                    size_t i,
                    size_t j,
                    FloatType tol
                    )
                    >& eqOperator,
                         FloatType tol= tool
    )
    {
        auto shape = get_shape(mat1);
        auto shape2 = get_shape(mat2);
        int rows = shape[0];
        int cols = shape[1];
        ASSERT_EQ(rows, shape2[0])<< "Matrices has different row counts :("
                                  <<rows<<", "<<shape2[0]<<")\n";
        ASSERT_EQ(cols, shape2[1])<< "Matrices has different column counts :("
                                  <<cols<<", "<<shape2[1]<<")\n";


        for (int i = 0; i < rows; ++i) {

            for (int j = 0; j < cols; ++j) {
                if constexpr (!Expect)
                {
                    ASSERT_PRED5(eqOperator,getMatrElement(mat1,i,j),getMatrElement(mat2,i,j),i,j,tol);
                }
                else
                {
                    EXPECT_PRED5(eqOperator,getMatrElement(mat1,i,j),getMatrElement(mat2,i,j),i,j,tol);
                }

            }
        }

    }

    static inline auto double_comparator=[]<typename FType=FloatType>
            (FType a,FType b, size_t i)
    {
        const testing::internal::FloatingPoint<FType> lhs(a), rhs(b);

        return lhs.AlmostEquals(rhs);
    };

    static inline auto double_comparator2=[]<typename FType=FloatType>
            (FType a,FType b ,size_t i, FType tol)
    {
        return is_near(a,b,tol);
    };

    static inline auto double_comparator3=[]<typename FType=FloatType>
            (FType a,FType b, size_t i,size_t j, FType tol)
    {
        return is_near(a,b,tol);
    };






   /* template<typename T1, typename T2>
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
    }*/
}


#endif //DIPLOM_GOOGLECOMMON_H
