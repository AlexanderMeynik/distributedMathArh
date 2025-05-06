#pragma once

#include <valarray>
#include "enumUtils.h"

/**
 * @brief shared namespace
 * @details This namespace Contains multiple forward declarations for types to be use everywhere
 */
namespace shared {
using FloatType = double;
using BenchResultType = uint64_t;

using namespace enum_utils;

using BenchResVec = std::valarray<BenchResultType>;

static inline const std::valarray<size_t> kNValues = {1ul, 2ul, 4ul,
                                                      5ul, 8ul, 10ul,
                                                      20ul, 40ul, 50ul,
                                                      100ul, 200ul, 400ul,
                                                      500ul};

/**
 * @brief Returns properly resized valarray for bench works
 * @param init - values to set for valarray
 */
BenchResVec DefaultBench(BenchResultType init = 1);

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

static const std::vector<EnumMapping<StateT>> kStateTMappings = {
    {StateT::NEW, "openmp_new"},
    {StateT::OLD, "new"},
    {StateT::OPENMP_NEW, "openmp_old"},
    {StateT::OPENMP_OLD, "old"},
    {StateT::PRINT, "printImpl"}
};
const static auto kStateToStr = createEnumToStrMap(kStateTMappings);

const static auto kStringToState = createStrToEnumMap(kStateTMappings);

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
