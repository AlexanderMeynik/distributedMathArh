#include "computationalLib/math_core/dipolesCommon.h"


namespace dipoles {

    bool isSymmetric(const co::matrixType &matr) {
        size_t N = matr.rows();

        for (int i = 0; i < N; ++i) {

            for (int j = 0; j < i; ++j) {
                if (matr.coeffRef(i, j) != matr.coeffRef(j, i)) {
                    return false;
                }
            }

        }
        return true;
    }
}
