#include "common/Parsers.h"

namespace printUtils {


    ms::MeshCreator fromJson(Json::Value&val,
                             std::optional<ms::dimType> dimOpt,
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
        mm.data[2]= jsonToContinuous<commonTypes::meshStorageType>(val, dims[0] * dims[1]);

        return mm;
    }

    meshStorage::MeshCreator parseMeshFrom(std::istream &in,
                                           ioFormat format,
                                           std::optional<ms::dimType> dimOpt,
                                           std::optional<ms::limType> limOpt,
                                           const EFormat &ef) {

        meshStorage::MeshCreator mm;
        ms::dimType dims{};
        ms::limType lims{};

        switch (format) {
            case ioFormat::Serializable:



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
                        throw ioError(to_string(in.rdstate()));
                    }
                }

                mm.data[2]= parseOneDim<commonTypes::meshStorageType>(in,dims[0]*dims[1]);

                break;

            case ioFormat::HumanReadable:

                in>>dims[0]>>dims[1];

                if(!in)
                {
                    throw ioError(to_string(in.rdstate()));
                }


                std::string dummy;
                std::getline(in, dummy);
                std::getline(in, dummy);
                in>>dummy;

                in>>lims[0];
                FloatType a;
                for (int i = 0; i <dims[0]-2; ++i) {
                    in>>a;
                }
                in>>lims[1];


                ct::meshStorageType m(dims[0]*dims[1]);


                for (int i = 0; i < dims[1]; ++i) {

                    FloatType temp = 0;
                    in >> temp;

                    if(i==0)
                    {
                        lims[2]=temp;
                    }
                    if(i==dims[1]-1)
                    {
                        lims[3]=temp;
                    }
                    for (int j = 0; j < dims[0]; ++j) {
                        FloatType val;
                        in >> val;
                        m[j * dims[1] + i] = val;

                    }
                }

                mm.constructMeshes(dims,lims);




                mm.data[2] = m;

                break;

        }
        mm.constructMeshes(dims,lims);


        return mm;


    }


    ct::matrixType parseMatrix(std::istream &in,
                                        std::optional<ct::dimType> dimOpt,
                                        const EFormat& ef) {
        size_t rows,cols;
        if(dimOpt.has_value())
        {
            rows=dimOpt.value()[0];
            cols=dimOpt.value()[1];
        }
        else
        {
            in>>rows>>cols;
        }

        if(!in)
        {
            throw ioError(to_string(in.rdstate()));
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



