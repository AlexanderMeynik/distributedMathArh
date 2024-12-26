#pragma once
#ifndef DIPLOM_DIPOLESCOMMON_H
#define DIPLOM_DIPOLESCOMMON_H


#include "common/myConcepts.h"
#include "const.h"

using namespace commonDeclarations;
namespace co=const_;
///dipoles namespace
namespace dipoles {

    using std::pair, std::vector, std::array;




    /**
     * Check that matrix is symmetric
     * @param matr
     */
    bool isSymmetric(const co::matrixType &matr);
}

#endif //DIPLOM_DIPOLESCOMMON_H
