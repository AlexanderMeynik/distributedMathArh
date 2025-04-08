#pragma once

#include "common/myConcepts.h"
#include "common/commonTypes.h"

///dipoles namespace
namespace dipoles {
//using namespace chronoClock;
using namespace common_types;

/**
 * Check that matrix is symmetric
 * @param matr
 */
bool IsSymmetric(const MatrixType &matr);
}
