#include "common/Parsers.h"

namespace printUtils {
    int getConfSize(std::string &filename) {
        std::ifstream in(filename);
        char c = parseChar(in);
        assert(c == 'C');
        int Nconf;
        in >> Nconf;
        return Nconf;
    }

    char parseChar(std::istream &in) {
        if (!in.eof()) {
            return static_cast<char>(in.get());
        }
        return {};
    }

    meshStorage::MeshCreator parseFrom(std::istream &in, const EFormat &ef) {
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
}



