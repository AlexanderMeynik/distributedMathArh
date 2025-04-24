#pragma once

#include <amqpcpp/message.h>
#include "parallelUtils/chronoClock.h"
#include "common/sharedDeclarations.h"
#include "network_shared/networkTypes.h"
#include "common/Generator.h"
#include "math_core/Dipoles.h"
#include "common/Printers.h"

/// Namespace for comp_node services
namespace comp_services {
using shared::FloatType;

constexpr FloatType kArange = 1e-6;
constexpr size_t iter_num = 10000;

/**
 * @class BenchmarkRunner
 * @brief A class to run performance benchmarks for dipole and mesh computations.
*/
class BenchmarkRunner {
 public:
  /**
   * @brief Constructs a BenchmarkRunner with benchmark parameters.
   * @param nss
   * @param iter_counts
   * @throws std::invalid_argument
   */
  BenchmarkRunner(const shared::BenchResVec &nss,
                  const shared::BenchResVec &iter_counts);

  /**
   * @brief Runs the benchmark for all parameter pairs and returns the results.
   */
  std::pair<shared::BenchResVec, shared::BenchResVec> Run(bool use_omp_outer = false, int eigen_threads = false);

 private:
  static constexpr double k_arange_ = 1e-6;
  static constexpr size_t iter_num_ = 10000;
  static auto inline ff_ = generators::get_normal_generator(0.0, k_arange_ * std::sqrt(2.0));
  shared::BenchResVec ns_;
  shared::BenchResVec iter_counts_;
  chrono_clock::ChronoClockTemplate<std::milli> clk;

  /**
   * @brief Runs the benchmark for a single pair of N and conf_num.
   * @param N
   * @param conf_num
   */
  shared::BenchResultType RunSingleBenchmark(size_t N, size_t conf_num, bool use_omp_outer, int eigen_threads);
};
extern shared::BenchResVec ns;
extern shared::BenchResVec iter_count;

extern BenchmarkRunner benchmarkRunner;

static auto inline n_message_callback =
    [](const AMQP::Message &message,
       uint64_t delivery_tag,
       bool redelivered) {

      Json::Reader rd;
      Json::Value val;

      rd.parse(message.body(), val);

      network_types::TestSolveParam ts(val);

      auto functor = generators::ParseFunc(
          get<std::string>(ts.args["type"]),
          ts.args
      );

      using namespace common_types;
      EigenVec coords(2 * ts.N_);
      EigenVec solution(4 * ts.N_);
      dipoles::Dipoles dp;
      mesh_storage::MeshCreator mc;

      mc.ConstructMeshes();
      StdValarr res = mc.data_[0];
      //todo use openmp
      for (int i = ts.range.first; i <= ts.range.second; ++i) {
        std::generate(std::begin(coords), std::end(coords), functor);

        dp.SetNewCoordinates(coords);

        solution = dp.Solve();
        dp.GetFullFunction(coords, solution);

        auto ff = dp.GetI2Function();

        mc.ApplyFunction(ff);
        res += mc.data_[2];
      }

      std::cout << print_utils::ContinuousToJson(res, false, true);

      std::cout << "Body: " << std::string(message.body(), message.bodySize()) << '\n';
      std::cout << "Priority: " << (int) message.priority() << '\n';
      std::cout << "Persistent: " << message.persistent() << '\n';
      std::cout << "Content-Type: " << message.contentType() << '\n';
      std::cout << "Timestamp: " << message.timestamp() << '\n';
      for (const auto &key : message.headers().keys()) {
        std::cout << "Header [" << key << "] = " << message.headers().operator[](key) << '\t'
                  << message.headers().operator[](key).typeID() << '\n';//typeId
      }
      std::cout << '\n';
    };

}