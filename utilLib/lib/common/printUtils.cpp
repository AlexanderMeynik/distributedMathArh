#include "common/printUtils.h"

#include <ostream>
#include <iomanip>

namespace printUtils {

    IosStatePreserve::IosStatePreserve(std::ostream &out) : out_(out) {
        flags_ = out.flags();
    }

    std::ios_base::fmtflags IosStatePreserve::getFlags() const {
        return flags_;
    }

    IosStatePreserve::~IosStatePreserve() {
        out_.flags(flags_);
    }


    const EFormat &printEnumToFormat(EigenPrintFormats fmt) {
        return enumTo.at(static_cast<size_t>(fmt));
    }

    std::ostream &operator<<(std::ostream &out, const ioFormat &form) {
        auto it = std::find_if(stringToIoFormat.begin(), stringToIoFormat.end(), [&form](const auto &item) {
            return item.second == form;
        });

        out << it->first << '\n';
        return out;
    }


    std::istream &operator>>(std::istream &in, ioFormat &form) {
        std::string a;
        in >> a;
        if (!stringToIoFormat.count(a)) {
            //todo why rvalue?
            throw InvalidOption(a);
        }
        form = stringToIoFormat.at(a);
        return in;
    }

    IosStateScientific::IosStateScientific(std::ostream &out, long precision) : IosStatePreserve(out) {
        out << std::setprecision(precision) << std::fixed << std::scientific;
    }
}