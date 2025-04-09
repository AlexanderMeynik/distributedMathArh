#include <drogon/drogon.h>
#include <controller/CompNode.h>
#include "parallelUtils/chronoClock.h"
#include "common/Printers.h"

using namespace drogon;
using rest::v1::CompNode;
using shared::BenchResVec;
constexpr FloatType kArange = 1e-6;
constexpr size_t iter_num=10000;
const std::array<size_t, 5> kRlims = {10, 50, 200, 500, 1000};
const std::array<size_t, 6> kDivs
    {
        1, 50, 200, 500, 1000, 2500
    };

auto rangeFinder = [](size_t N) -> size_t {
  auto range_num = std::lower_bound(kRlims.begin(), kRlims.end(), N) - kRlims.begin();
  auto currdiv = kDivs[range_num];
  return currdiv;
};
//todo test bench


chrono_clock::ChronoClockTemplate<std::nano> clk;
auto secondBench = []
    (size_t mul, size_t N)->std::pair<shared::BenchResultType,uint64_t > {

  auto conf_num = iter_num / mul;
  auto sig = kArange * sqrt(2);

  auto coordinates = generators::normal<std::valarray>(N, 0.0, sig);

  dipoles::Dipoles dipoles1;

  mesh_storage::MeshCreator ms;
  ms.ConstructMeshes();
  using namespace std::chrono_literals;
  auto loc=clk.tikLoc();
  const auto start = chrono_clock::ClockType::now();
  std::this_thread::sleep_for(std::chrono::milliseconds(N));
  const auto end = chrono_clock::ClockType::now();
  /*const std::chrono::duration<uint64_t, std::micro> elapsed = ;*/

  clk.tak();
  auto rr=clk[loc].time;
  clk.reset();
  /*#pragma omp parallel for firstprivate(dipoles1, ms), default(shared)
  for (size_t i = 0; i < conf_num; ++i) {
    dipoles1.SetNewCoordinates(coordinates);
    auto sol = dipoles1.Solve();

    dipoles1.GetFullFunction(coordinates, sol);

    ms.ApplyFunction(dipoles1.GetI2Function());
  }

  std::cout << N << '\n';
  std::cout << clk << "\n\n";*/

  return {rr,(end - start).count()};
};
BenchResVec ns={1ul, 2ul, 4ul, 5ul, 8ul, 10ul,
                20ul, 40ul, 50ul, 100ul, 200ul, 400ul, 500ul, 800ul, 1000ul, 2000ul};

int main(int argc, char *argv[]) {
  BenchResVec vec(ns.size());
  std::valarray<uint64_t> v1(ns.size());
  int i=0;
  for (auto N:{1ul, 2ul, 4ul, 5ul, 8ul, 10ul,
               20ul, 40ul, 50ul, 100ul, 200ul, 400ul, 500ul, 800ul, 1000ul, 2000ul}) {
    auto rsr=rangeFinder(N);
    std::cout<<N<<'\t'<<rsr<<'\t'<<iter_num/rsr<<'\n';
    std::tie(vec[i],v1[i])=secondBench(rsr,N);
    i++;
  }
  std::cout<<'\n';
  std::cout<<print_utils::ContinuousToJson(vec, false).toStyledString()<<'\n';
  std::cout<<print_utils::ContinuousToJson(v1, false).toStyledString();
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