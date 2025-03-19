#include "common/Printers.h"


void printUtils::printMesh(const MeshCreator&mesh,std::ostream &out, const printUtils::ioFormat &form,const EFormat &eigenForm) {

    switch (form) {
        case ioFormat::Serializable:
            goto ser;
        case ioFormat::HumanReadable: {
            goto human;
        }
    }

    ser:
    {
        IosStateScientific iosStateScientific(out, out.precision());
        out << mesh.dimensions[0] << '\t' << mesh.dimensions[1] << '\n';
        out << mesh.data[0][0] << '\t' << mesh.data[0][mesh.dimensions[0] - 1] << '\n';
        out << mesh.data[1][0] << '\t' << mesh.data[1][(mesh.dimensions[1])*(mesh.dimensions[0]-1)] << '\n';
        auto mm = toEigenRowVector(mesh.data.back());

        out << mm.format(eigenForm);
        return;
    }
    human:
    {
        IosStateScientific iosStateScientific(out, out.precision());
        meshStorage::printDec(mesh,out);
    };

}
