#include "iolib/Printers.h"




/*
template<>
int floatPrinter(std::ostream&out,const meshStorage::MeshCreator&printee,int N)
{
    IosStateScientific ioc(out,N);
    auto ext0=printee.spans[2].extent(0);
    auto ext1=printee.spans[2].extent(1);
    for (size_t i = 0; i < ext1; ++i) {
        for (size_t j = 0; j < ext0 - 1; ++j) {
            out << (printee.spans[2][std::array{j,i}],N) << "\t";
        }
        out << (printee.spans[2][std::array{ext0- 1,i}], N) << "\n";
    }
    return 0;
}*/
