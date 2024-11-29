#include "iolib/Parsers.h"

#include "computationalLib/math_core/MeshProcessor2.h"
#include "computationalLib/math_core/dipolesCommon.h"

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



