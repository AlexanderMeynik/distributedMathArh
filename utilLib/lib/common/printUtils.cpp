#include "common/printUtils.h"

#include <ostream>
#include <iomanip>

namespace printUtils {
    using namespace shared;
    const std::array<EFormat, 4> enumTo =
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
                     )
             }};


    const EFormat &printEnumToFormat(EigenPrintFormats fmt) {
        return enumTo.at(static_cast<size_t>(fmt));
    }

    std::ostream &operator<<(std::ostream &out, const ioFormat &form) {
        out << ENUM_TO_STR(form, ioToStr) << '\n';
        return out;
    }


    std::istream &operator>>(std::istream &in, ioFormat &form) {
        std::string a;
        in >> a;
        if (!stringToIoFormat.count(a)) {
            throw InvalidOption(a);
        }
        form = stringToIoFormat.at(a);
        return in;
    }


    std::istream &operator>>(std::istream &is, EFormat &fmt) {
        int precision, flags;
        std::string coeffSeparator, rowSeparator, rowPrefix, rowSuffix, matPrefix, matSuffix, fillStr;

        is >> precision >> flags
           >> std::quoted(coeffSeparator)
           >> std::quoted(rowSeparator)
           >> std::quoted(rowPrefix)
           >> std::quoted(rowSuffix)
           >> std::quoted(matPrefix)
           >> std::quoted(matSuffix)
           >> std::quoted(fillStr);

        if (fillStr.size() != 1) {
            is.setstate(std::ios::failbit);
            return is;
        }

        char fill = fillStr[0];

        fmt = EFormat(precision, flags, coeffSeparator, rowSeparator, rowPrefix, rowSuffix, matPrefix, matSuffix, fill);

        return is;
    }


    bool operator==(const EFormat &lhs, const EFormat &rhs) {
        return lhs.precision == rhs.precision &&
               lhs.flags == rhs.flags &&
               lhs.coeffSeparator == rhs.coeffSeparator &&
               lhs.rowSeparator == rhs.rowSeparator &&
               lhs.rowPrefix == rhs.rowPrefix &&
               lhs.rowSuffix == rhs.rowSuffix &&
               lhs.matPrefix == rhs.matPrefix &&
               lhs.matSuffix == rhs.matSuffix &&
               lhs.fill == rhs.fill;
    }

    std::ostream &operator<<(std::ostream &os, const EFormat &fmt) {
        os << fmt.precision << ' '
           << fmt.flags << ' '
           << std::quoted(fmt.coeffSeparator) << ' '
           << std::quoted(fmt.rowSeparator) << ' '
           << std::quoted(fmt.rowPrefix) << ' '
           << std::quoted(fmt.rowSuffix) << ' '
           << std::quoted(fmt.matPrefix) << ' '
           << std::quoted(fmt.matSuffix) << ' '
           << std::quoted(std::string(1, fmt.fill));
        return os;
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

    IosStateScientific::IosStateScientific(std::ostream &out, size_t precision) :
            IosStatePreserve(out),
            oldPrecision(out.precision()) {

        if (precision > 10 * defaultPrec) {
            throw outOfRange(precision, 0, 10 * defaultPrec);
        }

        out.precision(precision);

        out << std::fixed << std::scientific;
    }

    IosStateScientific::~IosStateScientific() {

        out_.flags(flags_);
        out_.precision(oldPrecision);
    }


}