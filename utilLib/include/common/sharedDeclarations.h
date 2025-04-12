#pragma once

#include <iosfwd>
#include <string>
#include <unordered_map>
#include <valarray>

/**
 * @brief shared namespace
 * @details This namespace Contains multiple forward declarations for types to be use everywhere
 */
namespace shared {
using FloatType = double;
using BenchResultType=uint64_t;

using BenchResVec=std::valarray<BenchResultType>;

/**
 * @brief Common numeric parameters values
 */
struct params {
  static constexpr FloatType c = 3.0 * 1e8;
  static constexpr FloatType yo = 1e7;
  static constexpr FloatType omega = 1e15;
  static constexpr FloatType omega0 = omega;
  static constexpr FloatType a = 1;
  static constexpr FloatType eps = 1;
};

/// enum for computation mode selection
enum class StateT {
  NEW,
  OLD,
  OPENMP_NEW,
  OPENMP_OLD,
  PRINT
};

constexpr std::array<const char *, 5> kStateToStr =
    {
        "openmp_new",
        "new",
        "openmp_old",
        "old",
        "printImpl"
    };

const static std::unordered_map<std::string, StateT> kStringToState = {
    {"openmp_new", StateT::OPENMP_NEW},
    {"new", StateT::NEW},
    {"openmp_old", StateT::OPENMP_OLD},
    {"old", StateT::OLD},
    {"printImpl", StateT::PRINT},
};

std::ostream &operator<<(std::ostream &out, const StateT &st);

/**
* Return codes enum
*/
enum ReturnCodes {
  WARNING_MESSAGE = -3,
  ALREADY_EXISTS = -2,
  ERROR_OCCURED = -1,
  RETURN_SUCESS = 0
};

/**
 *  Parameter type enum
 */
enum ParamType {
  EMPTY_PARAMETER_VALUE = -1
};

}
