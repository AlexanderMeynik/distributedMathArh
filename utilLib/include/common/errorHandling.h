#pragma once

#define DEFINE_EXCEPTION(Name, fmt_str, severity, ...) \
class Name : public MyException { \
public: \
    template<typename... Args> \
    Name(Args&&... args) \
        : MyException(fmt::format(fmt_string, std::forward<Args>(args)...),Severity::severity), \
          args_(std::forward<Args>(args)...) {} \
    \
    template<std::size_t N> \
    const auto& get() const { return std::get<N>(args_); } \
    const auto & getParams() const               \
    {                                       \
        return args_;                        \
    } \
    \
private: \
    static constexpr const char* fmt_string = fmt_str; \
    std::tuple<__VA_ARGS__> args_; \
};

#define DEFINE_EXCEPTION_IN(Name, fmt_str, ...) DEFINE_EXCEPTION(Name,fmt_str,info,__VA_ARGS__);
#define DEFINE_EXCEPTION_WA(Name, fmt_str, ...) DEFINE_EXCEPTION(Name,fmt_str,warning,__VA_ARGS__);
#define DEFINE_EXCEPTION_ER(Name, fmt_str, ...) DEFINE_EXCEPTION(Name,fmt_str,error,__VA_ARGS__);
#define DEFINE_EXCEPTION_Fa(Name, fmt_str, ...) DEFINE_EXCEPTION(Name,fmt_str,fatal,__VA_ARGS__);

#define EXC_TO_STR(ex, arr) ENUM_TO_STR(en.getSev(),arr)
#define STR(cc) std::string{cc}

#define VARIABLE_NAME(Variable) (#Variable)

#include <stdexcept>
#include <array>
#include <unordered_map>
#include <tuple>

#include <fmt/format.h>

#include <scn/scan.h>
#include "parallelUtils/timingUtils.h"

/**
 * @brief shared namespace
 * @details This namespace Contains multiple forward declarations for types to be use everywhere
 */
namespace shared {
using timing::LocationType;

/// severity enum
enum class Severity {
  info = 0,
  warning,
  error,
  fatal
};

/// Severity text enum to str lookup
constexpr static std::array<const char *, 4> kSevToStr
    {
        "info",
        "warning",
        "error",
        "fatal"
    };

/**
 * @brief String to severity lookup
 */
const std::unordered_map<std::string,Severity> kStrToSev
    {
        {"info",Severity::info},
        {"warning",Severity::warning},
        {"error",Severity::error},
        {"fatal",Severity::fatal},
    };
/**
 * @brief Default parent for user defined exception
 */
class MyException : public std::logic_error {
 public:
  using std::logic_error::logic_error;

  MyException(const std::string &arg, const Severity &sev) :
      std::logic_error(arg), m_sev_(sev) {}

  void setSeverity(const Severity &sev) {
    m_sev_ = sev;
  }

  Severity getSev() const {
    return m_sev_;
  }

 private:
  Severity m_sev_;
};

/**
 * @brief InvalidOption class
 * @details Use case: option resolution result in an error
 */
DEFINE_EXCEPTION_IN(InvalidOption, "Option {} does not exist!", std::string)

/**
 * @brief outOfRange class
 * @details Use case: index is out of range
 */
DEFINE_EXCEPTION_IN(outOfRange, "Value {} is out of range[{},{}]!", long, long, long)

/**
 * @brief zeroSize class
 * @details Use case: empty collection used as init
 */
DEFINE_EXCEPTION_IN(zeroSize, "Zero collection {} size!", std::string)

/**
 * @brief ioError class
 * @details Use case: error during ios management
 */
DEFINE_EXCEPTION_IN(ioError, "Io error state:{}!", const std::string&)

/**
 * @brief mismatchedSizes error
 * @details Use case: empty collection used as init
 */
DEFINE_EXCEPTION_IN(mismatchedSizes, "Mismatched container sizes for input : a.size = {}, b.size() = {} !", long,
                    long);

/**
 * @brief Error template that captures several size values
 * @details Captured size values may contain one(or more) that are
 * @details out of range for standard continuous types.
 */
DEFINE_EXCEPTION_IN(invalidSizes2, "Invalid sizes sz1 = {}, sz2 = {} received!", long, long)

/**
 * @brief Exception thrown when the number of rows does not divide the total collection size.
 */
DEFINE_EXCEPTION_ER(RowDivisionError,
                    "The total collection size {} cannot be evenly divided by the specified number of rows {}.",
                    std::size_t, std::size_t);


/**
 * @brief HttpError class
 */
DEFINE_EXCEPTION_IN(HttpError, "HTTP error: code {} , reason \"{}\" !", long, std::string)

/**
 * @brief Curl error class
 * @details Use case: handling curl abnormalities
 */
DEFINE_EXCEPTION_IN(CurlError, "Curl error: \"{}\" !", std::string)

/**
 * @brief SQL error class
 * @details Use case: handling SQL abnormalities
 */
DEFINE_EXCEPTION_IN(SQL_ERROR, "SQL Error: {}. Query: {}. SQL State: {}!", std::string, std::string, std::string)

/**
 * @brief Already conencted error class
 * @details Use case: to avoid reconnection
 */
DEFINE_EXCEPTION_IN(Already_Connected, "Service {} is already connected to {}!", std::string, std::string)
/**
 * @brief Broken connection error class
 * @details Use case: some thing cannot connect to some other thing
 */
DEFINE_EXCEPTION_IN(Broken_Connection, "Service {} is unable to connect to {}!", std::string, std::string)


using ScanErrorCode = decltype(scn::scan_error::code::value_negative_overflow);


static inline const char* ScanErrorCodeToString(ScanErrorCode c) {
  switch (c) {
    case scn::scan_error::end_of_input: return "end_of_input";
    case scn::scan_error::invalid_format_string: return "invalid_format_string";
    case scn::scan_error::invalid_scanned_value: return "invalid_scanned_value";
    case scn::scan_error::invalid_literal: return "invalid_literal";
    case scn::scan_error::invalid_fill: return "invalid_fill";
    case scn::scan_error::length_too_short: return "length_too_short";
    case scn::scan_error::invalid_source_state: return "invalid_source_state";
    case scn::scan_error::value_positive_overflow: return "value_positive_overflow";
    case scn::scan_error::value_negative_overflow: return "value_negative_overflow";
    case scn::scan_error::value_positive_underflow: return "value_positive_underflow";
    case scn::scan_error::value_negative_underflow: return "value_negative_underflow";
    case scn::scan_error::max_error: return "max_error";
    default: return "unknown_error";
  }
}



/**
 * @brief ScanningError error class
 * @details Use case: something went bad when using scn::scan
 */
DEFINE_EXCEPTION_IN(ScanningError, "Scanning error! Reason = \"{}\" Message = \"{}\"!",
                    ScanErrorCode, std::string)


}

namespace fmt {
using shared::ScanErrorCode;
using shared::ScanErrorCodeToString;
template<>
struct formatter<ScanErrorCode> : formatter<string_view> {
  // parse is inherited from formatter<string_view>.

  auto format(ScanErrorCode c, format_context &ctx) const
  -> format_context::iterator {
    return formatter<string_view>::format(ScanErrorCodeToString(c), ctx);
  }
};
}

