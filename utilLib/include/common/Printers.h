#pragma once

#ifndef DATA_DEDUPLICATION_SERVICE_PRINTERS_H
#define DATA_DEDUPLICATION_SERVICE_PRINTERS_H
//https://chat.deepseek.com/a/chat/s/f8c98b84-4982-46cd-900a-8e24ed042dee

#include <limits>


#include <json/json.h>
#include <valarray>
#include <concepts>
#include "common/sharedDeclarations.h"
#include "common/printUtils.h"
#include "common/myConcepts.h"
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
        res["size"] = a.size();
        for (size_t i = 0; i <= a.size(); i++) {
            res["data"][(Json::ArrayIndex)i] = a[i];
        }

        return res;
    }



//todo printer for vectors,
//todo printer for matrix
//todo find and get solutions form

    template<typename PrintType>
    int floatPrinter(std::ostream &out, const PrintType &printee, int N = std::numeric_limits<FloatType>::digits10 - 1);


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
            IosStatePreserve state(out);
            out << std::scientific;

            out << solution[2 * i] << "\t"
                << solution[2 * i + 2 * N_] << "\t"
                << cx << "\n";
            out << solution[2 * i + 1] << "\t"
                << solution[2 * i + 1 + 2 * N_] << "\t"
                << cy << "\n";

        }
        return 0;
    }


    /*template<class T>
    void printSolution(std::ostream &out, std::vector<T> &solution_, Eigen::IOFormat format = Eigen::IOFormat()) {
        Eigen::Map<Eigen::Vector<T, Eigen::Dynamic>> map(solution_.data(),
                                                         solution_.size());//todo copy impl(as template
        out << "Вектор решения\n" << map.format(format) << "\n";
    }

//todo make this one support all structs
    template<class T>
    void printSolution(std::ostream &out, Eigen::Vector<T, Eigen::Dynamic> &solution_,
                       Eigen::IOFormat format = Eigen::IOFormat()) {
        out << "Вектор решения\n" << solution_.format(format) << "\n";
    }*/


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


}


#endif //DATA_DEDUPLICATION_SERVICE_PRINTERS_H
