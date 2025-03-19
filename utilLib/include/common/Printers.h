#pragma once

#ifndef DATA_DEDUPLICATION_SERVICE_PRINTERS_H
#define DATA_DEDUPLICATION_SERVICE_PRINTERS_H
//https://chat.deepseek.com/a/chat/s/f8c98b84-4982-46cd-900a-8e24ed042dee

#include <limits>


#include <json/json.h>
#include "common/sharedDeclarations.h"
#include "common/printUtils.h"
#include "common/typeCasts.h"
/// printUtils namespace
namespace printUtils {




    using shared::FloatType;
    using printUtils::IosStatePreserve, printUtils::IosStateScientific;

    /*class MeshCreator {//in comp node?
    public:
        //todo designated separated printer for bulk print*/

    /**
     * @b Cast any one dimensional array to Json::value
     * @tparam Struct
     * @param a
     * @returns Json::Value with serialized array
     */
    template<myConcepts::isOneDimensionalContinuous Struct>
    Json::Value continuousToJson(const Struct&a) {
        Json::Value res;
        //todo precision test
        res["size"] = a.size();
        for (size_t i = 0; i <= a.size(); i++) {
            res["data"][(Json::ArrayIndex)i] = a[i];
        }

        return res;
    }





    template<typename Container>
    requires myConcepts::isOneDimensionalContinuous<Container> &&
             std::is_floating_point_v<typename Container::value_type>
    int printSolutionFormat1(std::ostream &out, const Container &solution) {
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
    requires myConcepts::isOneDimensionalContinuous<Collection> &&
             std::is_floating_point_v<typename Collection::value_type>
    void inline oneDimSerialize(std::ostream &out, const Collection &xi,const EFormat &eigenForm=EFormat()) {
        auto map= toEigenRowVector(xi);
        out<<xi.size()<<'\n';
        out<<map.format(eigenForm);
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
    void printCoordinates(std::ostream &out, const Collection &xi,ioFormat format=ioFormat::Serializable,
                          const EFormat &eigenForm=EIGENF(EigenPrintFormats::BasicOneDimensionalVector)) {
        switch (format) {
            case ioFormat::Serializable:
                oneDimSerialize(out, xi, eigenForm);
                break;
            case ioFormat::HumanReadable: {
                IosStateScientific iosStateScientific(out, out.precision());
                printCoordinates2(out, xi);
            }
                break;
        }
    }


    template<typename Collection>
    requires myConcepts::isOneDimensionalContinuous<Collection> &&
             std::is_floating_point_v<typename Collection::value_type>
    void printSolution(std::ostream &out, const Collection &sol,ioFormat format=ioFormat::Serializable,
                       const EFormat &eigenForm=EIGENF(EigenPrintFormats::BasicOneDimensionalVector)) {
        switch (format) {
            case ioFormat::Serializable:
                oneDimSerialize(out, sol,  eigenForm);
                break;
            case ioFormat::HumanReadable: {
                IosStateScientific iosStateScientific(out, out.precision());
                printSolutionFormat1(out, sol);
            }
            break;
        }
    }




}


#endif //DATA_DEDUPLICATION_SERVICE_PRINTERS_H
