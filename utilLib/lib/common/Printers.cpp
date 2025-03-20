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

    void printMesh(const MeshCreator &mesh, std::ostream &out, const printUtils::ioFormat &form,
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
            out << mesh.data[0][0] << '\t' << mesh.data[0][mesh.dimensions[0] - 1] << '\n';
            out << mesh.data[1][0] << '\t' << mesh.data[1][(mesh.dimensions[1]) * (mesh.dimensions[0] - 1)] << '\n';
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
