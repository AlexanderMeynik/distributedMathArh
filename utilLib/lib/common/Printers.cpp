#include "common/Printers.h"

namespace printUtils {

    std::ostream &operator<<(std::ostream &os, const EFormat &fmt) {
        os << fmt.precision << ' '
           << fmt.flags << ' '
           << std::quoted(fmt.coeffSeparator) << ' '
           << std::quoted(fmt.rowSeparator) << ' '
           << std::quoted(fmt.rowPrefix) << ' '
           << std::quoted(fmt.rowSuffix) << ' '
           << std::quoted(fmt.matPrefix) << ' '
           << std::quoted(fmt.matSuffix) << ' '
           << std::quoted(std::string(1, fmt.fill));
        return os;
    }

    void printMesh(std::ostream &out,const MeshCreator&mesh, const printUtils::ioFormat &form,
                               const EFormat &eigenForm) {

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
            out<<mesh.limits[0]<<'\t'<<mesh.limits[1]<<'\n';
            out<<mesh.limits[2]<<'\t'<<mesh.limits[3]<<'\n';
            auto mm = toEigenRowVector(mesh.data.back());

            out << mm.format(eigenForm);
            return;
        }
        human:
        {
            IosStateScientific iosStateScientific(out, out.precision());
            meshStorage::printDec(mesh, out);
        };

    }
}
