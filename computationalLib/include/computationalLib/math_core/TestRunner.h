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
#include "math_core/Dipoles.h"

template<typename ...Args>
struct functable {
  std::function<void(Args...)> func;
  const char *name;
};


using namespace print_utils;
using chrono_clock::gClk;
using common_types::FloatType;
using namespace common_types;
using namespace shared;

using Coordinates = EigenVec;
using Solution = EigenVec;

/**
 * @brief testRunner class
 */
class TestRunner {

 public:
  TestRunner();

  TestRunner(size_t n, size_t ns, double a_range, std::string dirname = "", std::string subdir = "",
             StateT state = StateT::OPENMP_NEW);

  template<typename... Args>
  void GenerateGeneralized(const std::function<Coordinates(Args...)> &functor, Args ... args) {
    coords_.resize(nsym_.value());
    for (int i = 0; i < nsym_; ++i) {
      coords_[i] = functor(args...);
    }
    if (coords_[0].size() != 2 * n_.value())//может ввести отдельнкю спецаилизацтю для данного случая
    {
      n_ = coords_[0].size() / 2;
    }
  }

  void Solve();

  void GenerateFunction();

  std::vector<Eigen::Vector<FloatType, Eigen::Dynamic>> &GetCoordRef() {
    return coords_;
  }

  std::vector<Solution> &GetSolRef() {
    return solutions_;
  }

 private:
  static void CreateSubDirectory(const std::string &dirname, const std::string &subdirectory = "");

  static std::fstream OpenOrCreateFile(std::string filename);

  static std::string GetString(const std::string &dirname, std::string &&name, int i, std::string &&end);

  std::vector<Eigen::Vector<FloatType, Eigen::Dynamic>> coords_;
  std::vector<Solution> solutions_;
  std::optional<std::string> subdir_;
  std::optional<std::string> dir_;
  std::optional<FloatType> a_range_;
  std::optional<size_t> n_;
  std::optional<size_t> nsym_;

  std::map<int, std::string> clock_names_ = {{0, "Generate time"},
                                             {1, "Solve time"},
                                             {2, "Function time"}};

  StateT inner_state_ = StateT::OPENMP_NEW;
};

template<typename ...Args>
static constexpr functable<Args...> kFuncArray[3] = {
    {&TestRunner::GenerateGeneralized, "coord_generation"},
    {&TestRunner::Solve, "system_of_equation_solution"},
    {&TestRunner::GenerateFunction, "function_generation"}
};

