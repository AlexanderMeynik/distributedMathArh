#include "common/Parsers.h"

namespace printUtils {


    ms::MeshCreator fromJson(Json::Value&val,std::optional<ms::dimType> dimOpt,
                             std::optional<ms::limType> limOpt)
    {
        meshStorage::MeshCreator mm;

        ms::dimType dims;

        if(dimOpt.has_value())
        {
            dims=dimOpt.value();
        }
        else
        {
            dims[0]=val["dimensions"][0].asUInt();
            dims[1]=val["dimensions"][1].asUInt();
        }
        ms::limType lims{};

        if(limOpt.has_value())
        {
            lims=limOpt.value();
        }
        else
        {
            for (int i = 0; i < lims.size(); ++i) {
                lims[i]=val["limits"][i].asDouble();
            }
        }


        mm.constructMeshes(dims,lims);
        mm.data[2]= parseCont<commonTypes::meshStorageType>(val["data"],dims[0]*dims[1]);

        mm.computeViews();
        return mm;
    }

    meshStorage::MeshCreator parseMeshFrom(std::istream &in,std::optional<ms::dimType> dimOpt,
                                           std::optional<ms::limType> limOpt, const EFormat &ef) {
        meshStorage::MeshCreator mm;

        ms::dimType dims{};
        ms::limType lims{};


        if(dimOpt.has_value())
        {
            dims=dimOpt.value();
        }
        else
        {
            in>>dims[0]>>dims[1];

            if(!in)
            {
                throw ioError(to_string(in.rdstate()));
            }
        }

        if(limOpt.has_value())
        {
            lims=limOpt.value();
        }
        else
        {
            in>>lims[0]>>lims[1]>>lims[2]>>lims[3];
            if(!in)
            {
                throw ioError(to_string(in.rdstate()));//todo fileleines
            }
        }


        mm.constructMeshes(dims,lims);
        mm.data[2]= parseOneDim<commonTypes::meshStorageType>(in,dims[0]*dims[1]);

        mm.computeViews();
        return mm;

    }


    commonTypes::matrixType parseMatrix(std::istream &in, long rows, long cols, const EFormat& ef) {
        if(rows==-1||cols==-1)
        {
            in>>rows>>cols;
        }

        if(!in)
        {
            throw ioError(to_string(in.rdstate()));
        }

        if(rows<0||cols<0)
        {
            throw invalidSizes2(rows,cols);
        }

        commonTypes::matrixType res(rows,cols);

        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                in>>res(i,j);
            }
        }
        return res;
    }

}



