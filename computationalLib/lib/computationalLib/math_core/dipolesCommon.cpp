#include "computationalLib/math_core/dipolesCommon.h"

namespace dipoles {

bool IsSymmetric(const MatrixType &matr) {
  size_t n = matr.rows();

  for (int i = 0; i < n; ++i) {

    for (int j = 0; j < i; ++j) {
      if (matr.coeffRef(i, j) != matr.coeffRef(j, i)) {
        return false;
      }
    }

  }
  return true;
}
}
