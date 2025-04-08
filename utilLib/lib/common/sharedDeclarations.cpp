#include "common/sharedDeclarations.h"
#include <ostream>

namespace shared {

std::ostream &operator<<(std::ostream &out, const StateT &st) {
  out << kStateToStr[static_cast<size_t>(st)];
  return out;
}

}