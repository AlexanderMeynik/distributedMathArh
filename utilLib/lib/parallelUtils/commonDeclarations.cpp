#include "parallelUtils/commonDeclarations.h"

#include <vector>
#include <string>
#include <cstdio>
#include <cstdarg>

namespace commonDeclarations {

    clockType gClk{};

    std::string vformat(const char *const zcFormat, ...) {

        // initialize use of the variable argument array
        va_list vaArgs;
        va_start(vaArgs, zcFormat);

        // reliably acquire the size
        // from a copy of the variable argument array
        // and a functionally reliable call to mock the formatting
        va_list vaArgsCopy;
        va_copy(vaArgsCopy, vaArgs);
        const int iLen = std::vsnprintf(nullptr, 0, zcFormat, vaArgsCopy);
        va_end(vaArgsCopy);

        // return a formatted string without risking memory mismanagement
        // and without assuming any compiler or platform specific behavior
        std::vector<char> zc(iLen + 1);
        std::vsnprintf(zc.data(), zc.size(), zcFormat, vaArgs);
        va_end(vaArgs);
        return std::string(zc.data(), iLen);
    }
}