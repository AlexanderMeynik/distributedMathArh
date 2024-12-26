#pragma once
#ifndef DIPLOM_PLOTINGUTILS_H
#define DIPLOM_PLOTINGUTILS_H

#include <functional>
/*

;*/
#include "common/commonDecl.h"
using const_::FloatType;
namespace meshStorage
{
    class MeshCreator;
}
void  plotFunction(const std::string& filename,const meshStorage::MeshCreator&mesh);


void plotCoordinates(const std::string& name, FloatType ar, std::array<std::vector<FloatType >, 2> &xi);

//todo N x's then N y'x in xi
void  plotCoordinates(const std::string& name, FloatType ar, const std::vector<FloatType> &xi);
#endif //DIPLOM_PLOTINGUTILS_H
