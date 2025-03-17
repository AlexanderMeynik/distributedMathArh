#pragma once
#ifndef DIPLOM_DIPOLESCOMMON_H
#define DIPLOM_DIPOLESCOMMON_H


#include "common/myConcepts.h"
#include "common/commonTypes.h"


///dipoles namespace
namespace dipoles {
    using namespace commonDeclarations;
    using namespace commonTypes;


    /**
     * Check that matrix is symmetric
     * @param matr
     */
    bool isSymmetric(const matrixType &matr);
}

#endif //DIPLOM_DIPOLESCOMMON_H
