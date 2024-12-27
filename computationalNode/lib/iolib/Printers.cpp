#include "iolib/Printers.h"

void printDec(meshStorage::MeshCreator &mmesh, std::ostream &out, int N) {
    out << "Функция I(phi,th)\n";
    out << "phi\\th\t\t";

    /*auto [exet0,ext1]=mmesh.spans[2].extents();*/
    auto ext0=mmesh.spans[2].extent(0);
    auto ext1=mmesh.spans[2].extent(1);
    for (size_t i = 0; i < ext0 - 1; ++i) {
        out << scientificNumber(mmesh.spans[1][std::array{i,0UL}], N) << '\t';
    }

    out << scientificNumber(mmesh.spans[1][std::array{(ext0 - 1),1UL}], N)
        << '\n';

    for (size_t i = 0; i < ext1; ++i) {
        auto phi = mmesh.spans[0][std::array{0UL,i}];
        out << scientificNumber(phi, 5) << "\t";
        for (size_t j = 0; j < ext0 - 1; ++j) {
            out << scientificNumber(mmesh.spans[2][std::array{j,i}], N) << "\t";
        }
        out << scientificNumber(mmesh.spans[2][std::array{ext0- 1,i}], N) << "\n";
    }
}


template<>
int floatPrinter(std::ostream&out,const meshStorage::MeshCreator&printee,int N)
{
    auto ext0=printee.spans[2].extent(0);
    auto ext1=printee.spans[2].extent(1);
    for (size_t i = 0; i < ext1; ++i) {
        for (size_t j = 0; j < ext0 - 1; ++j) {
            out << scientificNumber(printee.spans[2][std::array{j,i}],N) << "\t";
        }
        out << scientificNumber(printee.spans[2][std::array{ext0- 1,i}], N) << "\n";
    }
    return 0;
}