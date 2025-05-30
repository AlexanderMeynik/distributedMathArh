#include "common/printUtils.h"

#include <ostream>
#include <iomanip>

namespace print_utils {
using namespace shared;

const EFormat &PrintEnumToFormat(EigenPrintFormats fmt) {
  return kEnumTo.at(static_cast<size_t>(fmt));
}

std::ostream &operator<<(std::ostream &out, const IoFormat &form) {
  out << enum_utils::EnumToStr(form, kIoToStr);
  return out;
}

std::istream &operator>>(std::istream &in, IoFormat &form) {
  std::string a;
  in >> a;

  form = enum_utils::StrToEnum(a, kStringToIoFormat);
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

Delimiter::Delimiter(std::string_view s) : str(s) {}

bool inline isOnlyWhitespace(std::string_view text) {
  return !text.empty() && std::all_of(text.begin(), text.end(), [](unsigned char c) { return std::isspace(c); });
}
std::istream &operator>>(std::istream &is, const Delimiter &delim) {
  char c;
  if (isOnlyWhitespace(delim.str)) {
    return is;
  }

  for (auto &kCh : delim.str) {
    if (!is.get(c) || c != kCh) {
      is.setstate(std::ios::failbit);
      return is;
    }
  }
  return is;
}

bool ParseDelim(std::istream &in, std::string_view str) {
  Delimiter delimiter(str);
  in >> delimiter;
  return in.good();
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