

#ifndef DATA_DEDUPLICATION_SERVICE_ERRORHANDLING_H
#define DATA_DEDUPLICATION_SERVICE_ERRORHANDLING_H

#include <exception>
#include <source_location>
#include <tuple>
#include <string>
#include <fmt/format.h>


class MyException : public std::logic_error {
public:
    using std::logic_error::logic_error;
};

// Macro to define exceptions with parameters and formatted message
#define DEFINE_EXCEPTION(Name,fmt_str, ...) \
class Name : public MyException { \
public: \
    template<typename... Args> \
    Name(Args&&... args) \
        : MyException(fmt::format(fmt_string, std::forward<Args>(args)...)), \
          args_(std::forward<Args>(args)...) {} \
    \
    template<std::size_t N> \
    const auto& get() const { return std::get<N>(args_); } \
    \
private: \
    static constexpr const char* fmt_string = fmt_str; \
    std::tuple<__VA_ARGS__> args_; \
};






// Example usage
DEFINE_EXCEPTION(FileNotFound, "File not found: {}", const std::string&)
DEFINE_EXCEPTION(NetworkError, "Error {}: {}", int, const std::string&)
#endif //DATA_DEDUPLICATION_SERVICE_ERRORHANDLING_H
