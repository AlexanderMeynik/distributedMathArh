#include "common/printUtils.h"

#include <ostream>
#include <iomanip>




IosStatePreserve::IosStatePreserve(std::ostream &out):out_(out) {
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

IosStateScientific::IosStateScientific(std::ostream &out, int precision)         : IosStatePreserve(out) {
    out << std::setprecision(precision) << std::fixed << std::scientific;
}