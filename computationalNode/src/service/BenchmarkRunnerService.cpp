#include "service/BenchmarkRunnerService.h"
#include "common/MeshCreator.h"

namespace comp_services {

shared::BenchResVec
    ns = {1ul, 2ul, 4ul, 5ul, 8ul, 10ul, 20ul, 40ul, 50ul, 100ul, 200ul, 400ul, 500ul};//, 800ul, 1000ul ,2000ul};
shared::BenchResVec iter_count =
    {1000ul, 1000ul, 1000ul, 1000ul, 1000ul, 1000ul, 100ul, 100ul, 100ul, 25ul, 25ul, 5ul, 5ul};//, 25ul, 10ul, 2ul};


BenchmarkRunner benchmarkRunner{ns, iter_count};

shared::BenchResultType BenchmarkRunner::RunSingleBenchmark(size_t N,
                                                            size_t conf_num,
                                                            bool use_omp_outer,
                                                            int eigen_threads) {

  Eigen::setNbThreads(eigen_threads);

  auto mul = iter_num_ / conf_num;

  dipoles::Dipoles dipoles1;

  mesh_storage::MeshCreator ms;

  ms.ConstructMeshes();

  common_types::StdValarr coordinates(2 * N);

  auto loc = clk.TikLoc();

  common_types::EigenVec sol;

#pragma omp parallel for if(use_omp_outer) firstprivate(coordinates, dipoles1, ms) private(sol)
  for (size_t i = 0; i < conf_num; ++i) {

    std::generate(std::begin(coordinates), std::end(coordinates), ff_);
    dipoles1.SetNewCoordinates(coordinates);

    sol = std::move(dipoles1.Solve());

    dipoles1.GetFullFunction(coordinates, sol);

    ms.ApplyFunction(dipoles1.GetI2Function());

  }

  clk.Tak();
  auto rr = clk[loc].time * mul;
  clk.ResetTimer(loc);

  return rr;
}

std::pair<shared::BenchResVec, shared::BenchResVec> BenchmarkRunner::Run(bool use_omp_outer, int eigen_threads) {
  std::cout << omp_get_max_threads() << '\n';
  shared::BenchResVec results(ns_.size());
  shared::BenchResVec results2(ns_.size());
  for (size_t j = 0; j < ns_.size(); ++j) {
    auto loc = clk.TikLoc();

    results[j] = RunSingleBenchmark(ns_[j], iter_counts_[j], use_omp_outer, eigen_threads);
    clk.Tak();
    auto rr = clk[loc].time;
    clk.ResetTimer(loc);
    std::cout << ns[j] << '\t' << iter_counts_[j] << '\t';
    std::cout << rr << '\n';
    results2[j] = rr;

  }

  return {results, results2};
}
BenchmarkRunner::BenchmarkRunner(const shared::BenchResVec &nss,
                                 const shared::BenchResVec &iter_counts) :
    ns_(nss),
    iter_counts_(iter_counts) {
  if (ns_.size() != iter_counts_.size()) {

    ///@tododo use myException
    throw std::invalid_argument("ns and iter_counts must have the same size");
  }
}

}