#pragma once

#define EIGENF(form) kEnumTo.at(static_cast<size_t>(form))

#include <iosfwd>
#include <limits>

#include <eigen3/Eigen/Dense>
#include "common/sharedDeclarations.h"
#include "common/errorHandling.h"

/// printUtils namespace
namespace print_utils {

using EFormat = Eigen::IOFormat;

static constexpr size_t kDefaultPrec = std::numeric_limits<shared::FloatType>::digits10;

/**
 * @brief Prints tuple to string
 * @tparam TupleT
 * @tparam TupSize
 * @param tp
 * @param delim - is printed after each tuple element
 * @param left - is printed before all tuple elements
 * @param right  - is printed after all tuple elements
 */
template<typename TupleT, std::size_t TupSize = std::tuple_size_v<TupleT>>
std::string TupleToString(const TupleT &tp,
                          const char *delim = ",", const char *left = "(",
                          const char *right = ")");

/**
 * @brief Eformat lookup Table
 */
extern const std::array<EFormat, 4> kEnumTo;
enum class EigenPrintFormats {
  BASIC_ONE_DIMENSIONAL_VECTOR = 0,
  VECTOR_FORMAT_1,
  MATRIX_FORMAT,
  MATRIX_FORMAT_1
};

/**
 * @brief Casts EigenFormat to Eformat
 * @param fmt
 */
const EFormat &PrintEnumToFormat(EigenPrintFormats fmt);

/**
 * @brief General IOformat
 */
enum class IoFormat {
  ///Basic print format(look a lot alike json)
  SERIALIZABLE,
  //Human-readable representation for printed struct
  HUMAN_READABLE
};

/// String lookup map for ioformat
static const std::unordered_map<std::string, IoFormat> kStringToIoFormat =
    {
        {"Serializable", IoFormat::SERIALIZABLE},
        {"HumanReadable", IoFormat::HUMAN_READABLE}
    };

/// String lookup table for strings
static constexpr std::array<const char *, 2> kIoToStr =
    {
        "Serializable",
        "HumanReadable"
    };

/**
 * @brief Printer for ioFormat
 * @param out
 * @param form
 */
std::ostream &operator<<(std::ostream &out, const IoFormat &form);

/**
 * @brief Parser for ioFormat
 * @param in
 * @param form
 * @throws InvalidOption
 */
std::istream &operator>>(std::istream &in, IoFormat &form);

/**
 * @brief Parser for EFormat
 * @param is
 * @param fmt
 */
std::istream &operator>>(std::istream &is, EFormat &fmt);

/**
 * @brief Printer for EFormat
 * @details This implementation uses \" \" to separate escape characters inside separators
 * @param os
 * @param fmt
 */
std::ostream &operator<<(std::ostream &os, const EFormat &fmt);

/**
 * @brief elementwise comparison between EFormat
 * @param lhs
 * @param rhs
 * @return
 */
bool operator==(const EFormat &lhs, const EFormat &rhs);

/**
 * @brief A guard class to save current iostream state
 * @details Object of this class stores current iostream state and restores it when it goes out of scope
 */
class IosStatePreserve {
 public:
  explicit IosStatePreserve(std::ostream &out);

  IosStatePreserve(IosStatePreserve &oother) = delete;

  ~IosStatePreserve();

  [[nodiscard]] std::ios_base::fmtflags GetFlags() const;

 protected:
  std::ios_base::fmtflags flags_;
  std::ostream &out_;
};

/**
 * @brief Sets iostream precision to a fixed value after saving it's state
 */
class IosStateScientific : public IosStatePreserve {
 public:
  using IosStatePreserve::IosStatePreserve;

  explicit IosStateScientific(std::ostream &out, size_t precision = kDefaultPrec);

  ~IosStateScientific();

 protected:
  size_t old_precision_;
};

template<typename TupleT, std::size_t TupSize>
std::string TupleToString(const TupleT &tp,
                          const char *delim, const char *left,
                          const char *right) {
  return []<typename TupleTy, std::size_t... Is>(const TupleTy &tp, const char *delim, const char *left,
                                                 const char *right, std::index_sequence<Is...>) -> std::string {
    std::stringstream res;
    res << left;

    (..., (res << (Is == 0 ? "" : delim) << get<Is>(tp)));

    res << right;
    return res.str();
  }.operator()(tp, delim, left, right, std::make_index_sequence<TupSize>{});
}
}
