#pragma once

#include "common/myConcepts.h"
#include "common/commonTypes.h"

///dipoles namespace
namespace dipoles {
    //using namespace chronoClock;
    using namespace commonTypes;

    /**
     * Check that matrix is symmetric
     * @param matr
     */
    bool isSymmetric(const matrixType &matr);
}
