#pragma once
#ifndef DATA_DEDUPLICATION_SERVICE_PARSERS_H
#define DATA_DEDUPLICATION_SERVICE_PARSERS_H

#include <iostream>

#include <algorithm>//do weneed it?
#include <numeric>
#include <fstream>
#include <iomanip>
#include <optional>

#include <json/json.h>

#include "common/MeshCreator.h"
#include "common/myConcepts.h"
/// printUtils namespace
namespace printUtils {
    using namespace shared;
    using myConcepts::isOneDimensionalContinuous;
    namespace ms=meshStorage;
    /**
     * @brief Parse one dimensional array from json
     * @tparam Struct
     * @param val
     * @param sz
     */
    template<isOneDimensionalContinuous Struct>
    Struct parseCont(Json::Value &val, std::optional<size_t > sz= std::nullopt);

    /**
     * @brief Parse MeshCreator from json
     * @param val
     * @param dimOpt
     * @param limOpt
     * @return
     */
    ms::MeshCreator fromJson(Json::Value&val,std::optional<ms::dimType> dimOpt=std::nullopt,
                                    std::optional<ms::limType> limOpt=std::nullopt);




    /**
     * @brief Parse one dimensional array from provided istream
     * @tparam Struct
     * @param in
     * @param size
     * @param ef
     * @return Struct to store the values
     */
    template<isOneDimensionalContinuous Struct>
    Struct parseOneDim(std::istream &in,long size=-1,const EFormat& ef=EFormat());

    /**
     * @brief Parse matrix
     * @tparam Struct
     * @param in
     * @param vecSize
     * @param ef
     * @return Struct to store the values
     */
    commonTypes::matrixType parseMatrix(std::istream &in, long rows= -1, long cols= -1, const EFormat& ef= EFormat());

    /**
     * @brief Parses mesh creator from provided istream
     * @param in
     * @param ef
     * @param dimOpt
     * @param limOpt
     * @return MeshCreator instance
     */
    meshStorage::MeshCreator parseMeshFrom(std::istream &in,std::optional<ms::dimType> dimOpt=std::nullopt,
                                           std::optional<ms::limType> limOpt=std::nullopt,
                                           const EFormat& ef= EFormat());

}


namespace printUtils
{
    template<isOneDimensionalContinuous Struct>
    Struct parseCont(Json::Value &val, std::optional<size_t > sz) {

        size_t size = sz.value_or(val["size"].asUInt());
        std::valarray<double> res(size);

        for (int i = 0; i < size; ++i) {
            res[i] = val["data"][i].asDouble();
        }
        return res;
    }

    template<isOneDimensionalContinuous Struct>
    Struct parseOneDim(std::istream &in,long size,const EFormat& ef) {
        if(size==-1)
        {
            in>>size;
        }

        if(!in)
        {
            throw ioError(to_string(in.rdstate()));
        }

        if(size<0)
        {
            throw outOfRange(size,0,LONG_MAX);
        }

        Struct res(size);


        for (size_t i = 0; i < size; ++i) {
            in>>res[i];
        }
        return res;
    }

}
#endif //DATA_DEDUPLICATION_SERVICE_PARSERS_H
