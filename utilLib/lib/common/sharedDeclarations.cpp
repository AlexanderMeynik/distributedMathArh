#include "common/sharedDeclarations.h"
#include <ostream>

namespace shared {

std::ostream &operator<<(std::ostream &out, const StateT &st) {
  out << EnumToStr(st, kStateToStr);
  return out;
}
std::valarray<uint64_t> DefaultBench(uint64_t init) {
  BenchResVec res;
  res.resize(kNValues.size(), init);
  return res;
}

}