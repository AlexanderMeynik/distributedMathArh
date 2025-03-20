#ifndef DATA_DEDUPLICATION_SERVICE_ERRORHANDLING_H
#define DATA_DEDUPLICATION_SERVICE_ERRORHANDLING_H

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
    const auto & getParams()                \
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

#define ENUM_TO_STR(en,arr) arr[static_cast<size_t>(en)]

#define EXC_TO_STR(ex,arr) ENUM_TO_STR(en.getSev(),arr)
#define STR(cc) std::string{cc}
#include <stdexcept>
#include <array>
/*#include <source_location>*/
#include <tuple>


#include <fmt/format.h>

/**
 * @brief shared namespace
 * @details This namespace contains multiple forward declarations for types to be use everywhere
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
    constexpr static std::array<const char*,4>sevToStr
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
    class MyException : public std::logic_error {
    public:
        using std::logic_error::logic_error;

        MyException(const std::string& arg,const Severity&sev):
        std::logic_error(arg),m_sev(sev){}

        void setSeverity(const Severity&sev)
        {
            m_sev=sev;
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
    DEFINE_EXCEPTION_IN(InvalidOption, "Option {} does not exist!", const std::string&);

    /**
     * @brief outOfRange class
     */
    DEFINE_EXCEPTION_IN(outOfRange, "Value {} is out of range[{}{}]!", long, long, long)

    /**
     * @brief ioError class
     */
    DEFINE_EXCEPTION_IN(ioError, "Io error state:{}!", const std::string&);

}


#endif //DATA_DEDUPLICATION_SERVICE_ERRORHANDLING_H
