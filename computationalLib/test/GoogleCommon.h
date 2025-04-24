#pragma once

#include <tuple>
#include <gtest/gtest.h>
#include "common/myConcepts.h"
#include "common/sharedDeclarations.h"
#include "common/printUtils.h"
#include "common/MeshCreator.h"
#include "common/commonTypes.h"

#define SLEEP(dur) std::this_thread::sleep_for(dur)
#define ASSERT_NEAR_REL(val1, val2, rel_error) ASSERT_NEAR(val1,val2,rel_error*val1/100)
#define EXPECT_NEAR_REL(val1, val2, rel_error) EXPECT_NEAR(val1,val2,rel_error*val1/100)

#define EXPECT_EXCEPTION_WITH_ARGS_BASE(statement, exception_type, expected_tuple, CMP) \
do { \
    try { \
        statement; \
        FAIL() << "Expected " #exception_type " but nothing was thrown"; \
    } catch (const exception_type& e) { \
        CMP(expected_tuple, e.getParams()); \
    } catch (...) { \
        FAIL() << "Expected " #exception_type " but caught a different exception"; \
    } \
} while (0)

#define EXPECT_EXCEPTION_WITH_ARGS(statement, exception_type, expected_tuple) \
do { \
    EXPECT_EXCEPTION_WITH_ARGS_BASE(statement, exception_type, expected_tuple,\
    EXPECT_EQ);                                                                          \
} while (0)

using namespace my_concepts;
using shared::FloatType;
namespace sh = shared;
namespace pu = print_utils;
namespace ct = common_types;
namespace ms = mesh_storage;

/// Testing utilities namespace
namespace test_common {
constexpr double kTool = std::numeric_limits<decltype(kTool)>::epsilon();

struct AuthParams {
  std::string host;
  std::string username;
  std::string password;
};

/**
 * @brief Custom check for double near
 * @tparam T
 * @param val1
 * @param val2
 * @param abs_error
 */
template<typename T>
bool IsNear(T val1, T val2, T abs_error) {
  const T kDiff = std::abs(val1 - val2);
  return kDiff <= abs_error;
}

/**
 * @brief Printer function used to name tests in parameterized test suites
 * @tparam TestSuite
 * @tparam Args
 * @param info
 */
template<typename TestSuite, typename ...Args>
auto FirstValueTuplePrinter(const testing::TestParamInfo<typename TestSuite::ParamType> &info) {
  return get<0>(info.param);
}

/**
 * @brief converts tuple to string with _ as a delimiter
 * @tparam TestSuite
 * @param info
 */
template<typename TestSuite>
auto TupleToString(const testing::TestParamInfo<typename TestSuite::ParamType> &info) {
  return print_utils::TupleToString(info.param, "_", "", "");
}

/**
 * @brief Compares two continious collections
 * @tparam Expect - if true will use Expect pred instead of assert
 * @tparam T1
 * @tparam T2
 * @param solution
 * @param solution2
 * @param eq_operator - some user defined comparison function
 * @param tol
 */
template<bool Expect = false, HasSizeMethod T1, HasSizeMethod T2>
requires valueTyped<T1> && valueTyped<T2>
    && std::common_with<typename T1::value_type, typename T2::value_type>
void CompareArrays(const T1 &solution,
                   const T2 &solution2,
                   const std::function<bool
                                           (
                                               const typename T1::value_type &a,
                                               const typename T2::value_type &b,
                                               size_t i,
                                               FloatType tol
                                           )
                   > &eq_operator,
                   FloatType tol = kTool
) {
  ASSERT_TRUE(solution.size() == solution2.size()) << "Collections have different sizes:("
                                                   << solution.size() << ", " << solution2.size() << ")\n";
  for (size_t i = 0; i < solution.size(); ++i) {
    if constexpr (!Expect) {
      ASSERT_PRED4(eq_operator, solution[i], solution2[i], i, tol);
    } else {
      EXPECT_PRED4(eq_operator, solution[i], solution2[i], i, tol);
    }
  }
}

template<typename...>
struct get_template;
/**
 * @brief retrieves template template and saves it inside type
 * @tparam C
 * @tparam Args
 */
template<template<typename...> typename C, typename... Args>
struct get_template<C<Args...>> {
  template<typename... Ts> using Type = C<Ts...>;
};

/**
 * @brief Compares 2d arrays
 * @tparam Expect - if true will use Expect pred instead of assert
 * @tparam M1_t
 * @tparam M2_t
 * @param mat1
 * @param mat2
 * @param eq_operator
 * @param tol
 */
template<bool Expect = false, typename M1_t, typename M2_t>
void Compare2DArrays(const M1_t &mat1, const M2_t &mat2,
                     const std::function<bool
                                             (
                                                 const FloatType &a,
                                                 const FloatType &b,
                                                 size_t i,
                                                 size_t j,
                                                 FloatType tol
                                             )
                     > &eq_operator,
                     FloatType tol = kTool
) {
  auto shape = GetShape(mat1);
  auto shape2 = GetShape(mat2);
  int rows = shape[0];
  int cols = shape[1];
  ASSERT_EQ(rows, shape2[0]) << "Matrices has different row counts :("
                             << rows << ", " << shape2[0] << ")\n";
  ASSERT_EQ(cols, shape2[1]) << "Matrices has different column counts :("
                             << cols << ", " << shape2[1] << ")\n";

  for (int i = 0; i < rows; ++i) {

    for (int j = 0; j < cols; ++j) {
      if constexpr (!Expect) {
        ASSERT_PRED5(eq_operator, GetMatrElement(mat1, i, j), GetMatrElement(mat2, i, j), i, j, tol);
      } else {
        EXPECT_PRED5(eq_operator, GetMatrElement(mat1, i, j), GetMatrElement(mat2, i, j), i, j, tol);
      }

    }
  }

}

template<typename Type>
struct arrayEqualComparator {
  static inline auto call = [](Type a, Type b, size_t i, Type tol) {
    return a == b;
  };
};

template<typename FType=FloatType>
struct arrayDoubleComparator {
  static inline auto call = [](FType a, FType b, size_t i, FType tol) {
    return IsNear(a, b, tol);
  };
};

template<typename FType=FloatType>
struct twoDArrayDoubleComparator {
  static inline auto call = [](FType a, FType b, size_t i, size_t j, FType tol) {
    return IsNear(a, b, tol);
  };
};

}



