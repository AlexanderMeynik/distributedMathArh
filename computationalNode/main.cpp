#include <drogon/drogon.h>
#include <controller/CompNode.h>
#include "parallelUtils/chronoClock.h"
#include "common/Printers.h"

using namespace drogon;
using rest::v1::CompNode;


/**
 * @class BenchmarkRunner
 * @brief A class to run performance benchmarks for dipole and mesh computations.
*/
class BenchmarkRunner {
 public:
  /**
   * @brief Constructs a BenchmarkRunner with benchmark parameters.
   * @param ns
   * @param iter_counts
   * @throws std::invalid_argument
   */
  BenchmarkRunner(const shared::BenchResVec& ns, const shared::BenchResVec& iter_counts)
      : ns_(ns), iter_counts_(iter_counts) {
    if (ns_.size() != iter_counts_.size()) {
      throw std::invalid_argument("ns and iter_counts must have the same size");
    }
  }

  /**
   * @brief Runs the benchmark for all parameter pairs and returns the results.
   */
  shared::BenchResVec Run() {
    shared::BenchResVec results(ns_.size());
    for (size_t j = 0; j < ns_.size(); ++j) {
      results[j] = RunSingleBenchmark(ns_[j], iter_counts_[j]);
    }
    return results;
  }

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
  shared::BenchResultType RunSingleBenchmark(size_t N, size_t conf_num) {
    //todo sometimes result in sigerro 139
    auto mul = iter_num_ / conf_num;

    dipoles::Dipoles dipoles1;

    mesh_storage::MeshCreator ms;

    ms.ConstructMeshes();

    common_types::StdValarr coordinates(2*N);

    auto loc= clk.TikLoc();


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
};



constexpr FloatType kArange = 1e-6;
constexpr size_t iter_num=10000;


chrono_clock::ChronoClockTemplate<std::milli> clk;
auto secondBench = []
    (size_t N,size_t conf_num)->shared::BenchResultType {

  auto mul = iter_num / conf_num;

  auto sig = kArange * sqrt(2);

  common_types::StdValarr coordinates(2*N);

  auto ff= generators::get_normal_generator(0.0,sig);


  dipoles::Dipoles dipoles1;

  mesh_storage::MeshCreator ms;
  ms.ConstructMeshes();
  auto loc= clk.TikLoc();


  #pragma omp parallel for firstprivate(coordinates,dipoles1, ms), default(shared)
  for (size_t i = 0; i < conf_num; ++i) {

    std::generate(std::begin(coordinates),std::end(coordinates),ff);

    dipoles1.SetNewCoordinates(coordinates);
    auto sol = dipoles1.Solve();

    dipoles1.GetFullFunction(coordinates, sol);

    ms.ApplyFunction(dipoles1.GetI2Function());
  }

  clk.Tak();
  auto rr=clk[loc].time*mul;
  clk.ResetTimer(loc);


  return rr;
};

struct TestSolveParam
{
  TestSolveParam()=default;
  size_t experiment_id_;
  std::pair<size_t, size_t > range;

  std::unordered_map<std::string,double> args;
  bool operator==(const TestSolveParam&oth)const=default;
  Json::Value ToJson()
  {
    Json::Value val;

    val["experiment_id"]=experiment_id_;
    val["range"][0]=range.first;
    val["range"][1]=range.second;

    for(auto&[key,value]:args)
    {
      val["args"][key]=value;
    }
    return val;
  }

  TestSolveParam(Json::Value&val):
  experiment_id_(val["experiment_id"].asUInt()),
  range(val["range"][0].asUInt(), val["range"][1].asUInt())
  {
    auto &vv=val["args"];
    for(auto&key:vv.getMemberNames())
    {
      args[key]=vv[key].asDouble();
    }
  }




};

int main(int argc, char *argv[]) {


  TestSolveParam pp;
  pp.experiment_id_=12133;
  pp.range={100,1000};
  pp.args["a1"]=1.3;
  pp.args["a2"]=1e10;
  pp.args["a3"]=124;
  pp.args["a4"]=1000;
  auto j=pp.ToJson();

  std::cout<<j.toStyledString()<<'\n';

  TestSolveParam pp2(j);
  std::cout<<(pp2==pp);
  pp2.args==pp.args;

  return 0;
  shared::BenchResVec ns = {1ul, 2ul, 4ul, 5ul, 8ul, 10ul,20ul, 40ul, 50ul, 100ul, 200ul, 400ul, 500ul};//, 800ul, 1000ul ,2000ul};
  shared::BenchResVec iter_count = {10000ul, 10000ul, 10000ul, 10000ul, 10000ul, 10000ul,1000ul, 1000ul, 1000ul, 250ul, 250ul, 50ul, 50ul};//, 25ul, 10ul, 2ul};
  //Eigen::initParallel();
  Eigen::setNbThreads(1);
  BenchResVec vec(ns.size());
  for (int j = 0; j < ns.size(); ++j) {
    std::cout<<ns[j]<<'\t'<<iter_count[j]<<'\t'<<'\n';
    vec[j]=secondBench(ns[j],iter_count[j]);
  }

  /*BenchmarkRunner runner(ns, iter_count);
  auto vec = runner.Run();*/

  print_utils::OneDimSerialize(std::cout,vec, false,print_utils::kEnumTo.at(
      static_cast<unsigned long>(print_utils::EigenPrintFormats::BASIC_ONE_DIMENSIONAL_VECTOR
      )));//todo too cumbersome
  std::cout<<'\n';
  std::cout<<print_utils::ContinuousToJson(vec, false).toStyledString()<<'\n';




  return 0;
  app().loadConfigFile("config/server_config.json");
  if (argc == 2) {
    int port = std::stoi(argv[1]);
    auto list = app().getListeners();
    app().addListener("0.0.0.0", port);
    std::cout << port << '\n';
    app().run();
  } else {
    return -1;
  }

}