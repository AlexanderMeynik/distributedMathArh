//
// Created by Lenovo on 23.04.2024.
//

#ifndef DIPLOM_TESTRUNNER_H
#define DIPLOM_TESTRUNNER_H

#include <optional>
#include <filesystem>
#include <omp.h>
#include <fstream>
#include <eigen3/Eigen/Core>
#include "MeshProcessor.h"
#include "../common/lib.h"
#include "../parallelUtils//OpenmpParallelClock.h"
#include "Dipoles.h"
#include "../common/Generator.h"
#include "../parallelUtils/clockArray.h"

typedef double FloatType;
template<typename T>
using geNsolution = std::array<Eigen::Vector<T, Eigen::Dynamic>, 2>;
template<typename T>
using geNcoordinates = std::array<std::vector<T>, 2>;
using solution = geNsolution<FloatType>;
using coordinates = geNcoordinates<FloatType>;


//todo почитать про вариадичные шаблоны, чтобы указывать стратегию для генерации(или  сделать это отдельно для метода)
class TestRunner {
//для возможности запуска цепочки методов нужно сделать паттерн декоратор
//инициализацию вещей стоит вынести в отдельные методы
//создать хэш таблицу(массив) для всех функций-этапов
//сериализатор для данных
public:


    TestRunner();

    TestRunner(size_t N, size_t Ns, double aRange, std::string dirname = "", std::string subdir = "",
               state_t state = state_t::openmp_new) {
        N_ = N;
        Nsym_ = Ns;
        aRange_ = aRange;
        subdir_ = subdir;
        std::stringstream ss;
        ss << aRange << ".csv";
        std::string aStr = ss.str();
        aStr.erase(std::remove(aStr.begin(), aStr.end(), '+'), aStr.end());
        std::replace(aStr.begin(), aStr.end(), '-', '_');
        if (dirname.empty()) {
            dir_ = "results/" + subdir_.value() + "experiment_N=" + std::to_string(N) +
                   "_Nsym=" + std::to_string(Nsym_.value()) + "_a=" + aStr + "_mode=" +
                   stateToString.find(inner_state)->second + "/";
        } else {
            dir_ = "results/" + subdir_.value() + dirname;
        }
        createSubDirectory(dir_.value(), subdir_.value());
        solutions_.resize(Nsym_.value());
        coords_.resize(Nsym_.value());
        inner_state = state;
    };
    //todo использовать паттерн стратегия(https://sourcemaking.com/design_patterns/strategy/cpp/1)

    void generateCoords(Generator<FloatType> &gen);

    template<typename... Args>
    void generateGeneralized(std::function<coordinates(Args...)> &functor, Args ... args) {
        //clocks_[0].tik();
        coords_.resize(Nsym_.value());
        for (int i = 0; i < Nsym_; ++i) {
            coords_[i] = functor(args...);
        }
        if (coords_[0][0].size() != N_)//может ввести отдельнкю спецаилизацтю для данного случая
        {
            N_ = coords_[0][0].size();
        }

        //clocks_[0].tak();
    }

    void solve();

    void generateFunction();
    //при первом тесте можно создавать бд с названием математического ядра под нужды пользаков
    //логика use if exists create if not(саму эту логику надо добавить в менеджер бд)


    std::vector<array<vector<FloatType>, 2>> &getCoordRef() {
        return coords_;
    }

    std::vector<solution> &getSolRef() {
        return solutions_;
    }

private:
    static void createSubDirectory(const std::string &dirname, const std::string &subdirectory = "");

    static std::fstream openOrCreateFile(std::string filename);

    static std::string getString(const std::string &dirname, std::string &&name, int i, std::string &&end);

    std::vector<array<vector<FloatType>, 2>> coords_;
    std::vector<solution> solutions_;
    std::optional<std::string> subdir_;
    std::optional<std::string> dir_;
    std::optional<FloatType> aRange_;
    std::optional<size_t> N_;
    std::optional<size_t> Nsym_;
    //static constexpr size_t NumCalc=3;
    //std::array<OpenmpParallelClock,NumCalc> clocks_;
    std::map<int, std::string> clock_names = {{0, "Generate time"},
                                              {1, "Solve time"},
                                              {2, "Function time"}};


    state_t inner_state = state_t::openmp_new;
};

template<typename ...Args>
functable<Args...> func_array[3] = {
        {&TestRunner::generateGeneralized, "coord_generation"},
        {&TestRunner::solve,               "system_of_equation_solution"},
        {&TestRunner::generateFunction,    "function_generation"}
};


#endif //DIPLOM;_TESTRUNNER_H
