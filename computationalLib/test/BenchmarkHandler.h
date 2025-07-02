#pragma once

#include <functional>

#include "common/printUtils.h"
#include "testingUtils/FileHandler.h"
#include "parallelUtils/chronoClock.h"

/// bench_utils namespace
namespace bench_utils {
namespace fu = file_utils;
using print_utils::TupleToString;

template<typename ratio=std::milli>
using ClockType = chrono_clock::ChronoClockTemplate<ratio>;

/**
 * @brief Compile time function to compute cartessian product for arbitrary number of arrays
 * @tparam ARRAYS
 * @param arrays
 */
template<typename...ARRAYS>
constexpr auto CartesianProduct(ARRAYS...arrays) {
  using Type = std::tuple<typename ARRAYS::value_type...>;

  constexpr std::size_t kN = (1 * ... * arrays.size());

  std::array<std::size_t, sizeof...(arrays)> dims{arrays.size()...};
  for (std::size_t i = 1; i < dims.size(); ++i) { dims[i] *= dims[i - 1]; }

  return [&]() {
    std::array<Type, kN> result;

    for (std::size_t i = 0; i < result.size(); ++i) {
      [&]<std::size_t... Is>(std::index_sequence<Is...>) {
        auto idx = std::make_tuple(((i * std::get<Is>(dims)) / kN) % arrays.size() ...);
        result[i] = std::make_tuple(arrays[std::get<Is>(idx)]...);

      }(std::make_index_sequence<sizeof...(arrays)>{});
    }

    return result;
  }();
}

/**
 * @brief Class that handles benchmark file creation time measurements and other functionality
 */
template<typename range=std::micro>
class BenchmarkHandler {
 public:
  using Clk1 = ClockType<range>;

  /**
   *
   * @param name -benchmark name
   * @param path - may be optional -> current directory will be used
   */
  explicit BenchmarkHandler(std::string_view name,
                            std::optional<std::string> path = std::nullopt);

  /**
   * @tparam ARRAYS - input arrays types for cartessian product
   * @param benchNameGenerator - function that generates iteration name
   * @param benchFunction - function that performs benchmark iteration
   * @param arrays - arrays for cartessian product
   */
  static inline std::filesystem::path ddpath_ = "timers";

  template<typename...ARRAYS>
  constexpr void
  RunBenchmark(const std::function<std::string(typename ARRAYS::value_type ...)> &bench_name_generator,
               const std::function<void(Clk1 &, fu::FileHandler &, size_t &,
                                        typename ARRAYS::value_type ...)> &bench_function,
               ARRAYS...arrays);

  /**
   * @brief Prints each entry in clk to a designated file
   * @param clk
   * @param preprint - is printed before each entry
   * @param delim - delimeter that is printed after each timer record
   * @param multiplier - timer time is multiplied by given multiplier(defaults to 1)
   */
  void SnapshotTimers(Clk1 &clk, const std::string &preprint, const std::string &delim = "\n",
                      size_t multiplier = 1);

  /**
   * @brief Prints global clock results into designated file
   */
  void PrintClocks() {
    auto kName = "benchTimers.txt";
    fh_.Upsert(kName);
    fh_.Output(kName, clk_arr_);
  }

  ~BenchmarkHandler() {
    PrintClocks();
    fh_.CloseFiles();
  }

 private:
  fu::FileHandler fh_;
  std::string benchmark_name_;
  Clk1 clk_arr_;
  size_t mul_;
};

template<typename range>
void BenchmarkHandler<range>::SnapshotTimers(Clk1 &clk, const std::string &preprint, const std::string &delim,
                                             size_t multiplier) {
  for (auto &kVal : clk) {

    std::string name = ddpath_ / (kVal.first[3] + "_" + kVal.first[1]);
    fh_.Upsert(name,std::ios_base::app);
    fh_.Output(name, preprint);
    fh_.Output(name, kVal.second.time * (unsigned long long int) (multiplier));
    fh_.Output(name, delim);
    fh_.Close(name);

  }
}

template<typename range>
BenchmarkHandler<range>::BenchmarkHandler(std::string_view name, std::optional<std::string> path):
    benchmark_name_(name),
    fh_(path.has_value() ? fu::GetNormalAbs(path.value()) : ""),
    clk_arr_(),
    mul_(1) {

}

template<typename range>
template<typename... ARRAYS>
constexpr void
BenchmarkHandler<range>::RunBenchmark(
    const std::function<std::string(typename ARRAYS::value_type ...)> &bench_name_generator,
    const std::function<void(Clk1 &, fu::FileHandler &, size_t &,
                             typename ARRAYS::value_type ...)> &bench_function,
    ARRAYS...arrays) {
  Clk1 clkdc = this->clk_arr_;

  auto cart = CartesianProduct(std::forward<ARRAYS>(arrays)...);
  auto size = std::tuple_size<typename decltype(cart)::value_type>{};
  size_t inner_counter = 0;
  auto lambda = [this, &bench_function]<ARRAYS>(typename ARRAYS::value_type ... vals) {
    return bench_function(clk_arr_, fh_, mul_, std::forward<typename ARRAYS::value_type>(vals)...);
  };

  file_utils::CreateDirIfNotPresent(fh_.GetParentPath() / ddpath_);

  for (auto &tuple : cart) {
    auto itername = benchmark_name_ + std::apply(bench_name_generator, tuple);
    std::apply(lambda, tuple);

    SnapshotTimers(clk_arr_, TupleToString(tuple, "\t", "", "") + "\t", "\n", mul_);

    inner_counter++;
    clkdc.advance(clk_arr_);
    clk_arr_.Reset();
  }
  clk_arr_ = clkdc;
}
}

