#include "common/sharedDeclarations.h"
#include <ostream>

namespace shared {


    std::ostream &operator<<(std::ostream &out, const state_t &st) {
        out << stateToString.at(st);
        return out;
    }

}