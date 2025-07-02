#pragma once

#define EIGENF(form) kEnumToEigenFormatMap.at(form)

#include <limits>
#include <string_view>

#include <eigen3/Eigen/Dense>
#include "common/sharedDeclarations.h"

/// printUtils namespace
namespace print_utils {
using namespace enum_utils;
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
static const std::array<EFormat, 5> kEnumTo =
    {{
         // Format 0: Matrix with row enclosures "[...]"
         EFormat(
             Eigen::StreamPrecision,  // Precision
             Eigen::DontAlignCols,    // Flags (no column alignment)
             ",",                    // Coefficient separator (between elements in a row)
             "",                      // Row separator (between rows)
             "[",                     // Row prefix
             "]",                     // Row suffix
             "",                      // Matrix prefix
             "\n"                     // Matrix suffix
         ),
         // Format 1: Simple space-separated values
         EFormat(
             Eigen::StreamPrecision,
             Eigen::DontAlignCols,
             "\t",
             "",
             "",
             "",
             "",
             "\n"
         ),
         // Format 2: Row-enclosed with newline separators
         EFormat(
             Eigen::StreamPrecision,
             Eigen::DontAlignCols,
             "\t",
             "\n",
             "[",
             "]",
             "",
             "\n"
         ),
         // Format 3: Newline-separated rows
         EFormat(
             Eigen::StreamPrecision,
             Eigen::DontAlignCols,
             "\t",
             "\n",
             "",
             "",
             "",
             "\n"
         ),
         EFormat(
             Eigen::StreamPrecision,  // Precision
             Eigen::DontAlignCols,    // Flags (no column alignment)
             ",",                    // Coefficient separator (between elements in a row)
             "",                      // Row separator (between rows)
             "{",                     // Row prefix
             "}",                     // Row suffix
             "",                      // Matrix prefix
             ""                     // Matrix suffix
         ),
     }};

enum class EigenPrintFormats {
  BASIC_ONE_DIMENSIONAL_VECTOR = 0,
  VECTOR_FORMAT_1,
  MATRIX_FORMAT,
  MATRIX_FORMAT_1,
  VECTOR_DB_FORMAT
};
static const std::vector<EnumDoubleMapping<EigenPrintFormats,EFormat>> kEfEnToEigenFormatMappings = {
    {EigenPrintFormats::BASIC_ONE_DIMENSIONAL_VECTOR, kEnumTo[0]},
    {EigenPrintFormats::VECTOR_FORMAT_1, kEnumTo[1]},
    {EigenPrintFormats::MATRIX_FORMAT, kEnumTo[2]},
    {EigenPrintFormats::MATRIX_FORMAT_1, kEnumTo[3]},
    {EigenPrintFormats::VECTOR_DB_FORMAT, kEnumTo[4]},
};

const auto kEnumToEigenFormatMap= CreateEnumToTypeMap(kEfEnToEigenFormatMappings);

/**
 * @brief Casts EigenFormat to Eformat
 * @param fmt
 */
const EFormat &PrintEnumToFormat(EigenPrintFormats fmt);

/**
 * @brief General IOformat
 */
enum class IoFormat {
  SERIALIZABLE,///< Basic Print format(look a lot alike json)
  HUMAN_READABLE///<Human-readable representation for printed struct
};

static const std::vector<EnumMapping<IoFormat>> kIoFormatMappings = {
    {IoFormat::SERIALIZABLE, "Serializable"},
    {IoFormat::HUMAN_READABLE, "HumanReadable"}
};

/// String lookup map for ioformat
static const auto kStringToIoFormat =
    CreateStrToEnumMap(kIoFormatMappings);

static auto kIoToStr =
    CreateEnumToStrMap(kIoFormatMappings);

/**
 * @brief Struct to parse and compare delimiters
 */
struct Delimiter {
  std::string_view str;
  Delimiter(std::string_view s);
};

/**
 * @brief Parses specified Delimiter from is
 * @details Sets failbit if delim is not found
 * @param is
 * @param delim
 */
std::istream &operator>>(std::istream &is, const Delimiter &delim);

/**
 * @brief Tries to parse Delim with str content
 * @param in
 * @param str
 * @return in.good()
 */
bool ParseDelim(std::istream &in, std::string_view str);

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
