#pragma once

#include <optional>
#include <filesystem>
#include <omp.h>
#include <fstream>
#include <functional>

#include <eigen3/Eigen/Core>


#include "common/MeshCreator.h"
#include "parallelUtils/chronoClock.h"
#include "common/Generator.h"
#include "parallelUtils/clockArray.h"
#include "common/commonTypes.h"
#include "computationalLib/math_core/Dipoles.h"

template<typename ...Args>
struct functable {
    std::function<void(Args...)> func;
    const char *name;
};
//todo clean up all redudndant types
using namespace printUtils;
using chronoClock::gClk;
using commonTypes::FloatType;
using namespace commonTypes;
using namespace shared;

using coordinates = EigenVec;
using solution = EigenVec;

/**
 * @brief testRunner class todo doc
 */
class TestRunner {

public:
    TestRunner();

    TestRunner(size_t N, size_t Ns, double aRange, std::string dirname = "", std::string subdir = "",
               state_t state = state_t::openmp_new);

    template<typename... Args>
    void generateGeneralized(const std::function<coordinates(Args...)> &functor, Args ... args) {
        coords_.resize(Nsym_.value());
        for (int i = 0; i < Nsym_; ++i) {
            coords_[i] = functor(args...);
        }
        if (coords_[0].size() != 2 * N_.value())//может ввести отдельнкю спецаилизацтю для данного случая
        {
            N_ = coords_[0].size() / 2;
        }
    }

    void solve();

    void generateFunction();

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

