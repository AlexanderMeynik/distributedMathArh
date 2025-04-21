#include "service/BenchmarkRunnerService.h"
#include "common/MeshCreator.h"

namespace comp_services {

shared::BenchResVec ns = {1ul, 2ul, 4ul, 5ul, 8ul, 10ul,20ul, 40ul, 50ul, 100ul, 200ul, 400ul, 500ul};//, 800ul, 1000ul ,2000ul};
shared::BenchResVec iter_count = {10000ul, 10000ul, 10000ul, 10000ul, 10000ul, 10000ul,1000ul, 1000ul, 1000ul, 250ul, 250ul, 50ul, 50ul};//, 25ul, 10ul, 2ul};


BenchmarkRunner benchmarkRunner{ns,iter_count};

shared::BenchResultType BenchmarkRunner::RunSingleBenchmark(size_t N, size_t conf_num) {

  auto mul = iter_num_ / conf_num;

  dipoles::Dipoles dipoles1;

  mesh_storage::MeshCreator ms;

  ms.ConstructMeshes();

  common_types::StdValarr coordinates(2 * N);

  auto loc = clk.TikLoc();

  //todo fix segfault
#pragma omp parallel for firstprivate(coordinates, dipoles1, ms), default(shared)
  for (size_t i = 0; i < conf_num; ++i) {

    std::generate(std::begin(coordinates), std::end(coordinates), ff_);

    dipoles1.SetNewCoordinates(coordinates);

    auto sol = dipoles1.Solve();

    dipoles1.GetFullFunction(coordinates, sol);

    ms.ApplyFunction(dipoles1.GetI2Function());
  }

  clk.Tak();
  auto rr = clk[loc].time * mul;
  clk.ResetTimer(loc);

  return rr;
}

shared::BenchResVec BenchmarkRunner::Run() {
  //Eigen::setNbThreads(1);
  omp_set_num_threads(1);
  shared::BenchResVec results(ns_.size());
  for (size_t j = 0; j < ns_.size(); ++j) {
    results[j] = RunSingleBenchmark(ns_[j], iter_counts_[j]);
  }
  return results;
}
BenchmarkRunner::BenchmarkRunner(const shared::BenchResVec &nss,
                                 const shared::BenchResVec &iter_counts) :
                                 ns_(nss),
                                 iter_counts_(iter_counts) {
  if (ns_.size() != iter_counts_.size()) {
    throw std::invalid_argument("ns and iter_counts must have the same size");
  }
}

}