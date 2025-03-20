#include "common/Parsers.h"

namespace printUtils {


    meshStorage::MeshCreator parseMeshFrom(std::istream &in, const EFormat &ef) {
        meshStorage::MeshCreator mm;

        std::array<size_t,2> dims{};
        in>>dims[0]>>dims[1];
        std::array<shared::FloatType,4> lims{};

        in>>lims[0]>>lims[1]>>lims[2]>>lims[3];

        mm.constructMeshes(dims,lims);
        mm.data[2]= parseOneDim<commonTypes::meshStorageType>(in,dims[0]*dims[1]);

        mm.computeViews();
        return mm;

    }


    std::istream &operator>>(std::istream &is, EFormat &fmt) {
        int precision, flags;
        std::string coeffSeparator, rowSeparator, rowPrefix, rowSuffix, matPrefix, matSuffix, fillStr;

        is >> precision >> flags
           >> std::quoted(coeffSeparator)
           >> std::quoted(rowSeparator)
           >> std::quoted(rowPrefix)
           >> std::quoted(rowSuffix)
           >> std::quoted(matPrefix)
           >> std::quoted(matSuffix)
           >> std::quoted(fillStr);

        if (fillStr.size() != 1) {
            is.setstate(std::ios::failbit);
            return is;
        }

        char fill = fillStr[0];

        fmt = EFormat(precision, flags, coeffSeparator, rowSeparator, rowPrefix, rowSuffix, matPrefix, matSuffix, fill);

        return is;
    }


    bool operator==(const EFormat & lhs, const EFormat& rhs) {
        return lhs.precision == rhs.precision &&
               lhs.flags == rhs.flags &&
               lhs.coeffSeparator == rhs.coeffSeparator &&
               lhs.rowSeparator == rhs.rowSeparator &&
               lhs.rowPrefix == rhs.rowPrefix &&
               lhs.rowSuffix == rhs.rowSuffix &&
               lhs.matPrefix == rhs.matPrefix &&
               lhs.matSuffix == rhs.matSuffix &&
               lhs.fill == rhs.fill;
    }
}



