#pragma once

#ifndef DATA_DEDUPLICATION_SERVICE_PRINTERS_H
#define DATA_DEDUPLICATION_SERVICE_PRINTERS_H


#include <limits>

#include <json/json.h>
#include "common/sharedDeclarations.h"
#include "common/printUtils.h"
#include "common/typeCasts.h"
#include "common/MeshCreator.h"

/// printUtils namespace
namespace printUtils {


    using shared::FloatType;
    using printUtils::IosStatePreserve, printUtils::IosStateScientific;
    namespace sh=shared;

    namespace ms=meshStorage;

    /**
     * @brief Print mesh to supplied std::ostream
     * @param mesh
     * @param out
     * @param form
     * @param printDims
     * @param printLims
     * @param eigenForm
     */
    void printMesh(std::ostream &out,
                   const ms::MeshCreator&mesh,
                   const ioFormat&form=ioFormat::Serializable,
                   bool printDims=true,
                   bool printLims=true,
                   const EFormat &eigenForm = EIGENF(EigenPrintFormats::VectorFormat1));

    /**
     * @brief Cast any one dimensional array to Json::value
     * @tparam Collection
     * @param col
     * @param printSize
     * @returns Json::Value with serialized array
     */
    template<myConcepts::isOneDimensionalContinuous Collection>
    Json::Value continuousToJson(const Collection &col,
                                 bool printSize = true);

    /**
     * @brief Transforms MeshCreator to json
     * @param mesh
     * @param printDims
     * @param printLims
     * @return Json::Value with serialized meshCreator
     */
    Json::Value toJson(const ms::MeshCreator&mesh,
                       bool printDims=true,
                       bool printLims=true);

    /**
     * @brief Prints one dimensional Collection using Eigen format
     * @tparam Collection
     * @param out
     * @param col
     * @param printSize
     * @param eigenForm
     */
    template<typename Collection>
    requires myConcepts::isOneDimensionalContinuous<Collection> &&
             std::is_floating_point_v<typename Collection::value_type>
    void inline oneDimSerialize(std::ostream &out,
                                const Collection &col,
                                bool printSize= true,
                                const EFormat &eigenForm = EFormat()) {
        auto map = toEigenRowVector(col);
        if(printSize) {
            out << col.size() << '\n';
        }
        out << map.format(eigenForm);
    }

    /**
     * Flattens and prints matrix as row Vector
     * @param out
     * @param matr
     * @param printSize
     * @param eigenForm
     */
    void matrixPrint1D(std::ostream &out,
                       const commonTypes::matrixType  &matr,
                       bool printSize= true,
                       const EFormat &eigenForm = EFormat());

    /**
     * @brief Serializes matrix with coordinates
     * @param out
     * @param matr
     * @param printDims
     * @param eigenForm
     */
    void matrixPrint2D(std::ostream &out,
                              const commonTypes::matrixType  &matr,
                              bool printDims= true,
                              const EFormat &eigenForm = EIGENF(EigenPrintFormats::MatrixFormat1));

    /**
     * @brief Human readable way to print solution vector
     * @tparam Collection
     * @param out
     * @param solution
     * @return
     */
    template<typename Collection>
    requires myConcepts::isOneDimensionalContinuous<Collection> &&
             std::is_floating_point_v<typename Collection::value_type>
    int printSolutionFormat1(std::ostream &out,
                             const Collection &solution);

    /**
     * @brief Human readable way to print coordinates vector
     * @tparam Collection
     * @param out
     * @param col
     */
    template<typename Collection>
    void printCoordinates2(std::ostream &out,
                           const Collection &col);

    /**
     * @brief Basic interface for printing coordinates
     * @tparam Collection
     * @param out
     * @param coord
     * @param format
     * @param printSize
     * @param eigenForm
     */
    template<typename Collection>
    requires myConcepts::isOneDimensionalContinuous<Collection> &&
             std::is_floating_point_v<typename Collection::value_type>
    void printCoordinates(std::ostream &out,
                          const Collection &coord,
                          ioFormat format = ioFormat::Serializable,
                          bool printSize= true,
                          const EFormat &eigenForm = EIGENF(EigenPrintFormats::VectorFormat1));

    /**
     * @brief Basic interface for printing solution
     * @tparam Collection
     * @param out
     * @param sol
     * @param format
     * @param printSize
     * @param eigenForm
     */
    template<typename Collection>
    requires myConcepts::isOneDimensionalContinuous<Collection> &&
             std::is_floating_point_v<typename Collection::value_type>
    void printSolution(std::ostream &out,
                       const Collection &sol,
                       ioFormat format = ioFormat::Serializable,
                       bool printSize= true,
                       const EFormat &eigenForm = EIGENF(EigenPrintFormats::VectorFormat1));

}

namespace printUtils {


    template<myConcepts::isOneDimensionalContinuous Struct>
    Json::Value continuousToJson(const Struct &col,
                                 bool printSize) {
        Json::Value res;
        if(printSize) {
            res["size"] = col.size();
        }
        for (size_t i = 0; i < col.size(); i++) {
            res["data"][(Json::ArrayIndex) i] = col[i];
        }

        return res;
    }


    template<typename Collection>
    requires myConcepts::isOneDimensionalContinuous<Collection> &&
             std::is_floating_point_v<typename Collection::value_type>
    int printSolutionFormat1(std::ostream &out,
                             const Collection &solution) {
        int N_ = solution.size() / 4.0;
        out<<N_<<'\n';
        out << "Решение системы диполей\n";
        out<<" Ai(x\\ny)\tBi(x\\ny)\tCi(x\\ny)\n";


        for (int i = 0; i < N_; i++) {
            auto cx = sqrt(solution[2 * i] * solution[2 * i] +
                           solution[2 * i + 2 * N_] * solution[2 * i + 2 * N_]);
            auto cy = sqrt(solution[2 * i + 1] * solution[2 * i + 1] +
                           solution[2 * i + 1 + 2 * N_] * solution[2 * i + 1 + 2 * N_]);

            out << solution[2 * i] << "\t"
                << solution[2 * i + 2 * N_] << "\t"
                << cx << "\n";
            out << solution[2 * i + 1] << "\t"
                << solution[2 * i + 1 + 2 * N_] << "\t"
                << cy << "\n";

        }
        return 0;
    }


    template<typename Collection>
    void printCoordinates2(std::ostream &out,
                           const Collection &col) {
        out << "Координаты диполей\n";

        if constexpr (not myConcepts::HasBracketsNested<Collection>) {
            auto N = col.size() / 2;
            out<<N<<'\n';
            for (int i = 0; i < N; ++i) {
                out << col[i] << '\t' << col[i + N] << "\n";
            }
        } else {
            auto N = col[0].size();
            out<<N<<'\n';
            for (int i = 0; i < N; ++i) {
                out << col[0][i] << '\t' << col[1][i] << "\n";
            }
        }
    }


    template<typename Collection>
    requires myConcepts::isOneDimensionalContinuous<Collection> &&
             std::is_floating_point_v<typename Collection::value_type>
    void printSolution(std::ostream &out,
                       const Collection &sol,
                       ioFormat format,
                       bool printSize,
                       const EFormat &eigenForm) {
        switch (format) {
            case ioFormat::Serializable:
                oneDimSerialize(out, sol,printSize, eigenForm);
                break;
            case ioFormat::HumanReadable: {
                IosStateScientific iosStateScientific(out, out.precision());
                printSolutionFormat1(out, sol);
                break;
            }
        }
    }


    template<typename Collection>
    requires (myConcepts::isOneDimensionalContinuous<Collection> &&
             std::is_floating_point_v<typename Collection::value_type>)
    void printCoordinates(std::ostream &out,
                          const Collection &coord,
                          ioFormat format,bool printSize,
                          const EFormat &eigenForm) {
        switch (format) {
            case ioFormat::Serializable:
                oneDimSerialize(out, coord,printSize, eigenForm);
                break;
            case ioFormat::HumanReadable: {
                IosStateScientific iosStateScientific(out, out.precision());
                printCoordinates2(out, coord);
                break;
            }
        }
    }
}


#endif //DATA_DEDUPLICATION_SERVICE_PRINTERS_H
