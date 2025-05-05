#include "common/printUtils.h"

#include <ostream>
#include <iomanip>

namespace print_utils {
using namespace shared;

const EFormat &PrintEnumToFormat(EigenPrintFormats fmt) {
  return kEnumTo.at(static_cast<size_t>(fmt));
}

std::ostream &operator<<(std::ostream &out, const IoFormat &form) {
  out << ENUM_TO_STR(form, kIoToStr) << '\n';
  return out;
}

std::istream &operator>>(std::istream &in, IoFormat &form) {
  std::string a;
  in >> a;
  if (!kStringToIoFormat.count(a)) {
    throw InvalidOption(a);
  }
  form = kStringToIoFormat.at(a);
  return in;
}

std::istream &operator>>(std::istream &is, EFormat &fmt) {
  int precision, flags;
  std::string coeff_separator, row_prefix, row_suffix, mat_prefix, mat_suffix, fill_str;
  std::string row_separator;

  is >> precision >> flags
     >> std::quoted(coeff_separator)
     >> std::quoted(row_separator)
     >> std::quoted(row_prefix)
     >> std::quoted(row_suffix)
     >> std::quoted(mat_prefix)
     >> std::quoted(mat_suffix)
     >> std::quoted(fill_str);

  if (fill_str.size() != 1) {
    is.setstate(std::ios::failbit);
    return is;
  }

  char fill = fill_str[0];

  fmt = EFormat(precision, flags, coeff_separator, row_separator, row_prefix, row_suffix, mat_prefix, mat_suffix, fill);

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

std::ios_base::fmtflags IosStatePreserve::GetFlags() const {
  return flags_;
}

IosStatePreserve::~IosStatePreserve() {
  out_.flags(flags_);
}

IosStateScientific::IosStateScientific(std::ostream &out, size_t precision) :
    IosStatePreserve(out),
    old_precision_(out.precision()) {

  if (precision > 10 * kDefaultPrec) {
    throw outOfRange(precision, 0, 10 * kDefaultPrec);
  }

  out.precision(precision);

  out << std::fixed << std::scientific;
}

IosStateScientific::~IosStateScientific() {

  out_.flags(flags_);
  out_.precision(old_precision_);
}

}