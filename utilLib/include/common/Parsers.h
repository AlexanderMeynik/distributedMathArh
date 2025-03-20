#pragma once
#ifndef DATA_DEDUPLICATION_SERVICE_PARSERS_H
#define DATA_DEDUPLICATION_SERVICE_PARSERS_H

#include <iostream>

#include <algorithm>
#include <numeric>
#include <fstream>
#include <iomanip>

#include <json/json.h>

#include "plotUtils/MeshCreator.h"
#include "common/myConcepts.h"

/// printUtils namespace
namespace printUtils {
    using namespace shared;
    using myConcepts::isOneDimensionalContinuous;

    /**
     * @brief Parse one dimensional array from json
     * @tparam Struct
     * @param val
     */
    template<isOneDimensionalContinuous Struct>
    Struct parseCont(Json::Value &val) {
        int size = val["size"].asUInt();

        std::valarray<double> res(size);

        for (int i = 0; i < size; ++i) {
            res[i] = val["data"][i].asDouble();
        }
        return res;
    }


    /**
     * @brief Parser for EFormat
     * @param is
     * @param fmt
     * @return
     */
    std::istream& operator>>(std::istream& is, EFormat & fmt);

    /**
     * @brief Elmentwise comparison between EFormat
     * @param lhs
     * @param rhs
     * @return
     */
    bool operator==(const EFormat & lhs, const EFormat& rhs);

    /**
     * @brief Parse one dimensional array from provided istream
     * @tparam Struct
     * @param in
     * @param vecSize
     * @param ef
     * @return Struct to store the values
     */
    template<isOneDimensionalContinuous Struct>
    Struct parseOneDim(std::istream &in,long vecSize=-1,const EFormat& ef=EFormat()) {
        if(vecSize==-1)
        {
            in>>vecSize;
        }

        if(!in)
        {
            throw ioError(to_string(in.rdstate()));
        }

        if(vecSize<0)
        {
            throw outOfRange(vecSize,0,LONG_MAX);
        }

        Struct res(vecSize);


        for (size_t i = 0; i < vecSize; ++i) {
            in>>res[i];
        }
        return res;
    }

    /**
     * @brief Parse matrix
     * @tparam Struct
     * @param in
     * @param vecSize
     * @param ef
     * @return Struct to store the values
     */
    commonTypes::matrixType inline parseMatrix(std::istream &in, long rows= -1, long cols= -1, const EFormat& ef= EFormat()) {
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
            //todo better
            throw outOfRange(rows,0,LONG_MAX);
        }

        commonTypes::matrixType res(rows,cols);

        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                in>>res(i,j);
            }
        }
        return res;
    }


    /**
     * @brief Parses mesh creator from provided istream
     * @param in
     * @param ef
     * @return
     */
    meshStorage::MeshCreator parseMeshFrom(std::istream &in, const EFormat& ef= EFormat());

    /*
    template<typename T>
    std::istream &operator>>(std::istream &in, std::array<std::vector<T>, 2> &xi) {
        size_t size;
        in >> size;
        for (int i = 0; i < 2; ++i) {
            xi[i].resize(size);
            for (int j = 0; j < size; ++j) {
                in >> xi[i][j];
            }
        }
        return in;
    }

    template<typename T>
    std::istream &operator>>(std::istream &in, std::array<Eigen::Vector<T, Eigen::Dynamic>, 2> &sol) {
        size_t size;
        in >> size;
        for (int i = 0; i < 2; ++i) {
            sol[i].resize(size);
            for (int j = 0; j < size; ++j) {
                in >> sol[i][j];
            }
        }
        return in;
    }*/


}

#endif //DATA_DEDUPLICATION_SERVICE_PARSERS_H
