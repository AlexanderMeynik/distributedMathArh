#include "common/Printers.h"

namespace printUtils {

    Json::Value toJson(const ms::MeshCreator&mesh,
                       bool printDims,
                       bool printLims)
    {
        Json::Value res=continuousToJson(mesh.data[2],false);;

        if(printDims)
        {
            res["dimensions"][0]=mesh.dimensions[0];
            res["dimensions"][1]=mesh.dimensions[1];
        }
        if(printLims)
        {
            for(int i=0;i<mesh.limits.size();i++)
            {
                res["limits"][i]=mesh.limits[i];
            }
        }
        return res;
    }

    void printMesh(std::ostream &out,
                   const ms::MeshCreator&mesh,
                   const ioFormat&form,
                   bool printDims,
                   bool printLims,
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
            if(printDims) {
                out << mesh.dimensions[0] << '\t' << mesh.dimensions[1] << '\n';
            }
            if(printLims) {
                out << mesh.limits[0] << '\t' << mesh.limits[1] << '\n';
                out << mesh.limits[2] << '\t' << mesh.limits[3] << '\n';
            }
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

    void
    matrixPrint1D(std::ostream &out,
                  const commonTypes::matrixType &matr,
                  bool printSize,
                  const EFormat &eigenForm) {
        auto map = Eigen::Map<const Eigen::RowVector<FloatType,-1>>(matr.data(),matr.size());
        if(printSize) {
            out << matr.size() << '\n';
        }
        out << map.format(eigenForm);
    }

    void inline
    matrixPrint2D(std::ostream &out,
                  const commonTypes::matrixType &matr,
                  bool printDims,
                  const EFormat &eigenForm) {
        if(printDims) {
            out << matr.rows() << '\t' << matr.cols() << '\n';
        }
        out << matr.format(eigenForm);
    }
}
