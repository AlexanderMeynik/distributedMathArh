#pragma once

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
    namespace ms = meshStorage;
    namespace ct = commonTypes;

    //todo we can get array size for json
    /**
     * @brief Parse one dimensional array from json
     * @tparam Collection
     * @param val
     * @param sz
     */
    template<isOneDimensionalContinuous Collection>
    Collection jsonToContinuous(Json::Value &val,
                                std::optional<size_t> sz = std::nullopt);

    /**
     * @brief Parse MeshCreator from json
     * @param val
     * @param dimOpt
     * @param limOpt
     * @return
     */
    ms::MeshCreator fromJson(Json::Value &val,
                             std::optional<ms::dimType> dimOpt = std::nullopt,
                             std::optional<ms::limType> limOpt = std::nullopt);


    /**
     * @brief Parse one dimensional array from provided istream
     * @tparam Struct
     * @param in
     * @param sizeOpt
     * @param ef
     * @return Struct to store the values
     */
    template<isOneDimensionalContinuous Struct>
    Struct parseOneDim(std::istream &in,
                       std::optional<size_t> sizeOpt = std::nullopt,
                       const EFormat &ef = EFormat());

    /**
     * @brief Parse matrix
     * @param in
     * @param dimOpt
     * @param ef
     */
    ct::matrixType parseMatrix(std::istream &in,
                               std::optional<ct::dimType> dimOpt = std::nullopt,
                               const EFormat &ef = EFormat());


    /**
     * @brief Parse solution function interface
     * @tparam Struct
     * @param in
     * @param format
     * @param sizeOpt
     * @param ef
     * @return solution vector
     */
    template<isOneDimensionalContinuous Struct>
    Struct parseSolution(std::istream &in,
                         ioFormat format = ioFormat::Serializable,
                         std::optional<size_t> sizeOpt = std::nullopt,
                         const EFormat &ef = EFormat());

    /**
     * @brief Parse coordinates function interface
     * @tparam Struct
     * @param in
     * @param format
     * @param sizeOpt
     * @param ef
     * @return coordinates vector
     */
    template<isOneDimensionalContinuous Struct>
    Struct parseCoordinates(std::istream &in,
                            ioFormat format = ioFormat::Serializable,
                            std::optional<size_t> sizeOpt = std::nullopt,
                            const EFormat &ef = EFormat());

    /**
     * @brief Parses mesh creator from provided istream
     * @param in
     * @param ef
     * @param format
     * @param dimOpt
     * @param limOpt
     * @return MeshCreator instance
     */
    meshStorage::MeshCreator parseMeshFrom(std::istream &in,
                                           ioFormat format = ioFormat::Serializable,
                                           std::optional<ms::dimType> dimOpt = std::nullopt,
                                           std::optional<ms::limType> limOpt = std::nullopt,
                                           const EFormat &ef = EFormat());

}


namespace printUtils {
    template<isOneDimensionalContinuous Struct>
    Struct jsonToContinuous(Json::Value &val,
                            std::optional<size_t> sz) {

        size_t size = sz.value_or(val["size"].asUInt());
        Struct res(size);

        for (int i = 0; i < size; ++i) {
            res[i] = val["data"][i].asDouble();
        }
        return res;
    }

    template<isOneDimensionalContinuous Struct>
    Struct parseOneDim(std::istream &in,
                       std::optional<size_t> sizeOpt,
                       const EFormat &ef) {

        size_t size;
        if (sizeOpt.has_value()) {
            size = sizeOpt.value();
        } else {
            in >> size;
        }

        if (!in) {
            throw ioError(to_string(in.rdstate()));
        }

        Struct res(size);


        for (size_t i = 0; i < size; ++i) {
            in >> res[i];
        }
        return res;
    }


    template<isOneDimensionalContinuous Struct>
    Struct parseSolution(std::istream &in,
                         ioFormat format,
                         std::optional<size_t> sizeOpt,
                         const EFormat &ef) {

        Struct res;
        switch (format) {
            case ioFormat::Serializable:
                return parseOneDim<Struct>(in, sizeOpt, ef);
            case ioFormat::HumanReadable: {
                size_t N;
                in >> N;
                std::string dummy;
                std::getline(in, dummy);
                std::getline(in, dummy);
                std::getline(in, dummy);

                if (!in) {
                    throw ioError(to_string(in.rdstate()));
                }


                Struct sol(N * 4);

                for (int i = 0; i < N; i++) {
                    FloatType ax, bx, cx;
                    FloatType ay, by, cy;
                    in >> ax >> bx >> cx >> ay >> by >> cy;

                    sol[2 * i] = ax;
                    sol[2 * i + 2 * N] = bx;
                    sol[2 * i + 1] = ay;
                    sol[2 * i + 1 + 2 * N] = by;

                }
                return sol;

            }
        }
        return res;
    }

    template<isOneDimensionalContinuous Struct>
    Struct parseCoordinates(std::istream &in,
                            ioFormat format,
                            std::optional<size_t> sizeOpt,
                            const EFormat &ef) {
        Struct res;
        switch (format) {
            case ioFormat::Serializable:
                return parseOneDim<Struct>(in, sizeOpt, ef);
            case ioFormat::HumanReadable: {
                std::string dummy;
                std::getline(in, dummy);
                size_t N;
                in >> N;
                if (!in) {
                    throw ioError(to_string(in.rdstate()));
                }

                Struct coord(N * 2);

                for (int i = 0; i < N; i++) {
                    FloatType x, y;
                    in >> coord[i] >> coord[i + N];
                }
                return coord;

            }
        }
        return res;
    }
}
