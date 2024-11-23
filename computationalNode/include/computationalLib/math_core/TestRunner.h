
#ifndef DIPLOM_TESTRUNNER_H
#define DIPLOM_TESTRUNNER_H

#include <optional>
#include <filesystem>
#include <omp.h>
#include <fstream>

#include <eigen3/Eigen/Core>


#include "MeshProcessor.h"
#include "common/lib.h"
#include "parallelUtils/OpenmpParallelClock.h"
#include "Dipoles.h"
#include "common/Generator.h"
#include "parallelUtils/clockArray.h"
#include "common/constants.h"
#include "computationalLib/math_core/Dipoles2.h"

using const_::FloatType;
template<typename T>
using geNsolution = Eigen::Vector<T, Eigen::Dynamic>;
template<typename T>
using geNcoordinates = Eigen::Vector<T, Eigen::Dynamic>;
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

    void generateCoords(Generator<FloatType> &gen);

    template<typename... Args>
    //todo переделать часть в сервере для генерации какашек
    void generateGeneralized(std::function<coordinates(Args...)> &functor, Args ... args) {
        //clocks_[0].tik();
        coords_.resize(Nsym_.value());
        for (int i = 0; i < Nsym_; ++i) {
            coords_[i] = functor(args...);
        }
        if (coords_[0].size() != 2 * N_.value())//может ввести отдельнкю спецаилизацтю для данного случая
        {
            N_ = coords_[0].size() / 2;
        }

        //clocks_[0].tak();
    }

    void solve();

    void generateFunction();
    //при первом тесте можно создавать бд с названием математического ядра под нужды пользаков
    //логика use if exists create if not(саму эту логику надо добавить в менеджер бд)


    std::vector<Eigen::Vector<FloatType, Eigen::Dynamic>> &getCoordRef() {
        return coords_;
    }

    std::vector<solution> &getSolRef() {
        return solutions_;
    }

private:
    static void createSubDirectory(const std::string &dirname, const std::string &subdirectory = "");

    static std::fstream openOrCreateFile(std::string filename);

    static std::string getString(const std::string &dirname, std::string &&name, int i, std::string &&end);

    std::vector<Eigen::Vector<FloatType, Eigen::Dynamic>> coords_;
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
static constexpr functable<Args...> func_array[3] = {
        {&TestRunner::generateGeneralized, "coord_generation"},
        {&TestRunner::solve,               "system_of_equation_solution"},
        {&TestRunner::generateFunction,    "function_generation"}
};


#endif //DIPLOM;_TESTRUNNER_H
