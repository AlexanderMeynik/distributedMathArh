#pragma once

#include <stdexcept>
#include <unordered_map>
#include <tuple>

#include <fmt/format.h>
#include <scn/scan.h>
#include <cpptrace/cpptrace.hpp>

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

/**
 * @brief shared namespace
 * @details This namespace Contains multiple forward declarations for types to be use everywhere
 */
namespace shared {

/// severity enum
enum class Severity {
  info = 0, //< this exception
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
 * @details Uses cpptrace::logic_error logic to store stack traces
 */
class MyException : public cpptrace::logic_error {
 public:
  using cpptrace::logic_error::logic_error;

  /**
   * @brief Retreives error message and stack trace using cpptrace
   * @param arg
   * @param sev
   */
  MyException(const std::string &arg, const Severity &sev):
  cpptrace::logic_error(std::string{arg}), m_sev_(sev)
  {}

  MyException(MyException&my_exception)=default;

  void SetSeverity(const Severity &sev) {
    m_sev_ = sev;
  }

  Severity getSev() const {
    return m_sev_;
  }

 private:
  Severity m_sev_;//< severity for this exception
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
DEFINE_EXCEPTION_IN(ioError, "Io error state:{}!",std::string)

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

/// Alias for scnlib error code enum
using ScanErrorCode = decltype(scn::scan_error::code::value_negative_overflow);

/// Lookup table for scn code strings
constexpr std::array<const char*, static_cast<size_t>(ScanErrorCode::max_error) + 1> ScanErrorStrings = {
    "end_of_input",
    "invalid_format_string",
    "invalid_scanned_value",
    "invalid_literal",
    "invalid_fill",
    "length_too_short",
    "invalid_source_state",
    "value_positive_overflow",
    "value_negative_overflow",
    "value_positive_underflow",
    "value_negative_underflow",
    "max_error"
};

/**
 * @brief Casts scan error to string
 * @param c
 * @return
 */
static inline const char* ScanErrorCodeToString(ScanErrorCode c) {
  size_t index = static_cast<size_t>(c);
  return (index <= ScanErrorStrings.size()) ? ScanErrorStrings[index] : "unknown_error";
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
/**
 * @brief Formatter for ScanErrorCode
 */
struct formatter<ScanErrorCode> : formatter<string_view> {
  auto format(ScanErrorCode c, format_context &ctx) const
  -> format_context::iterator {
    return formatter<string_view>::format(ScanErrorCodeToString(c), ctx);
  }
};
}

