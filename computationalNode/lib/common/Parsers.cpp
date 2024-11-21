#include "Parsers.h"

int getConfSize(std::string &filename) {
    std::ifstream in(filename);
    char c = parseChar(in);
    assert(c == 'C');
    int Nconf;
    in >> Nconf;
    return Nconf;
}

char parseChar(std::istream &in) {
    if (!in.eof()) {
        return static_cast<char>(in.get());
    }
    return {};
}
