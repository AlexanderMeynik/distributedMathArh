#include "common/printUtils.h"

#include <ostream>
#include <iomanip>
#include <cstdio>
#include <cstdarg>

namespace printUtils {

    std::string vformat(const char *const zcFormat, ...) {

        // initialize use of the variable argument array
        va_list vaArgs;
        va_start(vaArgs, zcFormat);

        // reliably acquire the size
        // from a copy of the variable argument array
        // and a functionally reliable call to mock the formatting
        va_list vaArgsCopy;
        va_copy(vaArgsCopy, vaArgs);
        const int iLen = std::vsnprintf(nullptr, 0, zcFormat, vaArgsCopy);
        va_end(vaArgsCopy);

        // return a formatted string without risking memory mismanagement
        // and without assuming any compiler or platform specific behavior
        std::vector<char> zc(iLen + 1);
        std::vsnprintf(zc.data(), zc.size(), zcFormat, vaArgs);
        va_end(vaArgs);
        return std::string(zc.data(), iLen);
    }

    IosStatePreserve::IosStatePreserve(std::ostream &out) : out_(out) {
        flags_ = out.flags();
    }

    std::ios_base::fmtflags IosStatePreserve::getFlags() const {
        return flags_;
    }

    IosStatePreserve::~IosStatePreserve() {
        out_.flags(flags_);
    }


    Eigen::IOFormat &printEnumToFormat(EigenPrintFormats fmt) {
        return enumTo[static_cast<size_t>(fmt)];
    }

    std::ostream &operator<<(std::ostream &out, const ioFormat &form) {
        auto it=std::find_if(stringToIoFormat.begin(), stringToIoFormat.end(), [&form](const auto &item) {
            return item.second==form;
        });

        out<<it->first<<'\n';
        return out;
    }


    std::istream &operator>>(std::istream &in, ioFormat &form) {
        std::string a;
        in>>a;
        if(!stringToIoFormat.count(a))
        {
            throw invalidOption<std::string>(std::move(a));
        }
        form=stringToIoFormat.at(a);
        return in;
    }

    IosStateScientific::IosStateScientific(std::ostream &out, int precision) : IosStatePreserve(out) {
        out << std::setprecision(precision) << std::fixed << std::scientific;
    }
}