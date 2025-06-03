#include "service/BenchmarkRunnerService.h"
#include "network_shared/sharedConstants.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <omp.h>
#include <Eigen/Core>

int main() {
  using shared::ns;
  using shared::iter_count;
  struct Config {
    std::string description;
    bool use_omp_outer;
    int eigen_threads;
  };
  std::vector<Config> configs = {
      {"OpenMP default + Eigen OpenMP", true, 0},
      {"OpenMP (outer loop) only", true, 1},
      {"Eigen with OpenMP only", false, 0},
      {"No OpenMP at all", false, 1}
  };

  std::ofstream out("outputs.txt");
  if (!out) {
    std::cerr << "Failed to open outputs.txt" << std::endl;
    return 1;
  }

  comp_services::BenchmarkRunner benchmarkRunner(ns, iter_count);

  for (const auto &config : configs) {
    out << "Configuration: " << config.description << "\n";
    out << "N\titer_count\ttime\n";

    auto [results, results2] = benchmarkRunner.Run(config.use_omp_outer, config.eigen_threads);

    for (size_t j = 0; j < ns.size(); ++j) {
      out << ns[j] << "\t" << iter_count[j] << "\t" << results[j]
          << "\t" << results2[j] << "\n";
    }
    out << "\n";
  }

  out.close();
  return 0;
}