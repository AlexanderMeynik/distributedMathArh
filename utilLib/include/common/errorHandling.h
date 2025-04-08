#pragma once

#define DEFINE_EXCEPTION(Name, fmt_str, severity, ...) \
class Name : public myException { \
public: \
    template<typename... Args> \
    Name(Args&&... args) \
        : myException(fmt::format(fmt_string, std::forward<Args>(args)...),Severity::severity), \
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

#define ENUM_TO_STR(en, arr) arr[static_cast<size_t>(en)]

#define EXC_TO_STR(ex, arr) ENUM_TO_STR(en.getSev(),arr)
#define STR(cc) std::string{cc}

#define VARIABLE_NAME(Variable) (#Variable)

#include <stdexcept>
#include <array>

#include <tuple>

#include <fmt/format.h>

/**
 * @brief shared namespace
 * @details This namespace Contains multiple forward declarations for types to be use everywhere
 */
namespace shared {

/// severity enum
enum class Severity {
  info = 0,
  warning,
  error,
  fatal
};

/// Severity text enum to str lookup
constexpr static std::array<const char *, 4> sevToStr
    {
        "info",
        "warning",
        "error",
        "fatal"
    };
//todo logging
/**
 * @brief Default parent for user defined exception
 */
class myException : public std::logic_error {
 public:
  using std::logic_error::logic_error;

  myException(const std::string &arg, const Severity &sev) :
      std::logic_error(arg), m_sev(sev) {}

  void setSeverity(const Severity &sev) {
    m_sev = sev;
  }

  Severity getSev() const {
    return m_sev;
  }

 private:
  Severity m_sev;
};

/**
 * @brief InvalidOption class
 */
DEFINE_EXCEPTION_IN(InvalidOption, "Option {} does not exist!", const std::string&)

/**
 * @brief outOfRange class
 */
DEFINE_EXCEPTION_IN(outOfRange, "Value {} is out of range[{},{}]!", long, long, long)

/**
 * @brief zeroSize class
 */
DEFINE_EXCEPTION_IN(zeroSize, "Zero collection {} size!", std::string)

/**
 * @brief ioError class
 */
DEFINE_EXCEPTION_IN(ioError, "Io error state:{}!", const std::string&)

/**
 * @brief mismatchedSizes error
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

//todo stack tracing
//https://stackoverflow.com/questions/77005/how-to-automatically-generate-a-stacktrace-when-my-program-crashes

/**
 * @brief HttpError class
 */
//todo can return body with reason
DEFINE_EXCEPTION_IN(httpError, "HTTP error: {} !", long)

/**
 * @brief HttpError class
 */
DEFINE_EXCEPTION_IN(curlError, "Curl error: {} !", std::string)
}

