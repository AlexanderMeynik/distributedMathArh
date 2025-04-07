#include "common/sharedDeclarations.h"
#include <ostream>

namespace shared {


    std::ostream &operator<<(std::ostream &out, const state_t &st) {
        out << stateToStr[static_cast<size_t>(st)];
        return out;
    }

}