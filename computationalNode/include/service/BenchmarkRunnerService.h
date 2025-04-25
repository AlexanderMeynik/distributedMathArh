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
   * @param use_omp_outer - uses openmp during runs
   * @param eigen_threads - sets num of eigen threads(0 for default num)
   */
  std::pair<shared::BenchResVec, shared::BenchResVec> Run(bool use_omp_outer = true, int eigen_threads = 0);

 private:
  static constexpr double k_arange_ = 1e-6;
  static constexpr size_t iter_num_ = 10000;
  static thread_local auto inline ff_ = generators::get_normal_generator(0.0, k_arange_ * std::sqrt(2.0));
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


      using namespace common_types;
      dipoles::Dipoles dipoles1;

      mesh_storage::MeshCreator ms;

      ms.ConstructMeshes();

      StdValarr coordinates(2 * ts.N_);

      EigenVec sol;

      StdValarr res = ms.data_[0];

      #pragma omp parallel firstprivate(coordinates, dipoles1, ms) private(sol) shared(res)
      {
        thread_local auto functor = generators::ParseFunc(
            get<std::string>(ts.args["type"]),
            ts.args
        );
        #pragma omp for
        for (int i = ts.range.first; i <= ts.range.second; ++i) {
          std::generate(std::begin(coordinates), std::end(coordinates), functor);
          dipoles1.SetNewCoordinates(coordinates);

          sol = std::move(dipoles1.Solve());

          dipoles1.GetFullFunction(coordinates, sol);

          ms.ApplyFunction(dipoles1.GetI2Function());
          #pragma  omp critical
          {
            res += ms.data_[2];
          }
        }
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