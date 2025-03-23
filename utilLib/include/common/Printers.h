#pragma once

#ifndef DATA_DEDUPLICATION_SERVICE_PRINTERS_H
#define DATA_DEDUPLICATION_SERVICE_PRINTERS_H
//https://chat.deepseek.com/a/chat/s/f8c98b84-4982-46cd-900a-8e24ed042dee

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
     * @param eigenForm
     */
    void printMesh(std::ostream &out,const ms::MeshCreator&mesh,const ioFormat&form=ioFormat::Serializable,
               const EFormat &eigenForm = EIGENF(EigenPrintFormats::VectorFormat1));

    /**
     * @brief Cast any one dimensional array to Json::value
     * @tparam Struct
     * @param a
     * @param printSize
     * @returns Json::Value with serialized array
     */
    template<myConcepts::isOneDimensionalContinuous Struct>
    Json::Value continuousToJson(const Struct &a,bool printSize = true);

    /**
     * @brief Transforms MeshCreator to json
     * @param mesh
     * @param printDims
     * @param printLims
     * @return Json::Value with serialized meshCreator
     */
    Json::Value toJson(const ms::MeshCreator&mesh,bool printDims=true,bool printLims=true);

    /**
     * @brief Prints one dimensional Collection using Eigen format
     * @tparam Collection
     * @param out
     * @param xi
     * @param eigenForm
     */
    template<typename Collection>
    requires myConcepts::isOneDimensionalContinuous<Collection> &&
             std::is_floating_point_v<typename Collection::value_type>
    void inline oneDimSerialize(std::ostream &out, const Collection &xi, const EFormat &eigenForm = EFormat()) {
        auto map = toEigenRowVector(xi);
        out << xi.size() << '\n';
        out << map.format(eigenForm);
    }

    /**
     * @brief Flattens and prints matrix as row Vector
     * @tparam Collection
     * @param out
     * @param xi
     * @param eigenForm
     */
    void inline matrixPrint1D(std::ostream &out, const commonTypes::matrixType  &matr, const EFormat &eigenForm = EFormat()) {
        auto map = Eigen::Map<const Eigen::RowVector<FloatType,-1>>(matr.data(),matr.size());
        out << matr.size() << '\n';
        out << map.format(eigenForm);
    }

    /**
     * @brief Serializes matrix with coordinates
     * @tparam Collection
     * @param out
     * @param xi
     * @param eigenForm
     */
    void inline matrixPrint2D(std::ostream &out, const commonTypes::matrixType  &matr, const EFormat &eigenForm = EIGENF(EigenPrintFormats::MatrixFormat1)) {
        out << matr.rows() << '\t' << matr.cols() << '\n';
        out << matr.format(eigenForm);
    }

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
    int printSolutionFormat1(std::ostream &out, const Collection &solution);

    /**
     * @brief Human readable way to print coordinates vector
     * @tparam Collection
     * @param out
     * @param xi
     */
    template<typename Collection>
    void printCoordinates2(std::ostream &out, const Collection &xi);

    /**
     * @brief Basic interface for printing coordinates
     * @tparam Collection
     * @param out
     * @param coord
     * @param format
     * @param eigenForm
     */
    template<typename Collection>
    requires myConcepts::isOneDimensionalContinuous<Collection> &&
             std::is_floating_point_v<typename Collection::value_type>
    void printCoordinates(std::ostream &out, const Collection &coord, ioFormat format = ioFormat::Serializable,
                          const EFormat &eigenForm = EIGENF(EigenPrintFormats::VectorFormat1));

    /**
     * @brief Basic interface for printing solution
     * @tparam Collection
     * @param out
     * @param sol
     * @param format
     * @param eigenForm
     */
    template<typename Collection>
    requires myConcepts::isOneDimensionalContinuous<Collection> &&
             std::is_floating_point_v<typename Collection::value_type>
    void printSolution(std::ostream &out, const Collection &sol, ioFormat format = ioFormat::Serializable,
                       const EFormat &eigenForm = EIGENF(EigenPrintFormats::VectorFormat1));

}

namespace printUtils {


    template<myConcepts::isOneDimensionalContinuous Struct>
    Json::Value continuousToJson(const Struct &a,bool printSize) {
        Json::Value res;
        if(printSize) {
            res["size"] = a.size();
        }
        for (size_t i = 0; i < a.size(); i++) {
            res["data"][(Json::ArrayIndex) i] = a[i];
        }

        return res;
    }


    template<typename Collection>
    requires myConcepts::isOneDimensionalContinuous<Collection> &&
             std::is_floating_point_v<typename Collection::value_type>
    int printSolutionFormat1(std::ostream &out, const Collection &solution) {
        out << "Решение системы диполей\n Ai(x\\ny)\tBi(x\\ny)\tCi(x\\ny)\n";
        int N_ = solution.size() / 4.0;

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
    void printCoordinates2(std::ostream &out, const Collection &xi) {
        out << "Координаты диполей\n";

        if constexpr (not myConcepts::HasBracketsNested<Collection>) {
            auto N = xi.size() / 2;
            for (int i = 0; i < N; ++i) {
                out << xi[i] << '\t' << xi[i + N] << "\n";
            }
        } else {
            auto N = xi[0].size();
            for (int i = 0; i < N; ++i) {
                out << xi[0][i] << '\t' << xi[1][i] << "\n";
            }
        }
    }


    template<typename Collection>
    requires myConcepts::isOneDimensionalContinuous<Collection> &&
             std::is_floating_point_v<typename Collection::value_type>
    void printSolution(std::ostream &out, const Collection &sol, ioFormat format,
                       const EFormat &eigenForm) {
        switch (format) {
            case ioFormat::Serializable:
                oneDimSerialize(out, sol, eigenForm);
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
    void printCoordinates(std::ostream &out, const Collection &coord, ioFormat format,
                          const EFormat &eigenForm) {
        switch (format) {
            case ioFormat::Serializable:
                oneDimSerialize(out, coord, eigenForm);
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
