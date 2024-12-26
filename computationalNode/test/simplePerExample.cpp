
#include "common/Generator.h"
#include "computationalLib/math_core/Dipoles.h"
#include "computationalLib/math_core/MeshCreator.h"

int main()
{
    constexpr size_t N=100;
    auto coordinates=generators::normal<std::valarray>(N,0.0,1.e-6*sqrt(2));

    dipoles::Dipoles dip(coordinates);
    auto solution =dip.solve<std::valarray<FloatType>>();

    dip.getFullFunction_(coordinates,solution);

    auto f=dip.getI2function();

    meshStorage::MeshCreator mh;
    mh.constructMeshes();
    mh.applyFunction(f);
    /*mh.plotAndSave("result.png",meshStorage::plotFunction);*/
    return 0;
}