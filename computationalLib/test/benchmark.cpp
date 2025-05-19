
#include <iostream>
#include <utility>
#include <tuple>
#include <algorithm>

#include <omp.h>

#include "fileHandler.h"
#include "BenchmarkHandler.h"
#include "common/Generator.h"
#include "math_core/Dipoles.h"
#include "common/MeshCreator.h"
#include "common/sharedDeclarations.h"

using file_utils::fileHandler;
using namespace shared;
using bench_utils::BenchmarkHandler;

void Func(int a, char b, uint c) {
  std::cout << a << '\n' << b << '\n' << c << '\n';
}

constexpr auto kChararr = std::array<char, 3>{'a', 'b', 'c'};
constexpr auto kIntarr = std::array<int, 4>{1, 2, 3, 4};
constexpr FloatType kArange = 1e-6;

auto normal_gen = generators::get_normal_generator(0.0, kArange * sqrt(2));

void
Loop(const std::valarray<FloatType> &coordinates, auto &clk, dipoles::Dipoles &dipoles1,
     mesh_storage::MeshCreator &ms, size_t confNum, StateT st) {
  for (size_t i = 0; i < confNum; ++i) {
    clk.Tik();
    dipoles1.SetNewCoordinates(coordinates);
    clk.Tak();
    clk.Tik();
    auto sol = dipoles1.Solve();
    clk.Tak();

    clk.Tik();
    dipoles1.GetFullFunction(coordinates, sol);
    clk.Tak();

    clk.Tik();
    if (st == StateT::NEW) {
      ms.ApplyFunction(dipoles1.GetI2Function());
    } else {
      ms.ApplyIntegrate(dipoles1.GetIfunction());
    }
    clk.Tak();

  }

}

auto firstBench = []
    (auto &clk, file_utils::fileHandler &handler, size_t &mul, StateT st, size_t N) {
  auto conf_num = 1000;

  std::valarray<FloatType> coordinates(2 * N);
  std::generate(std::begin(coordinates), std::end(coordinates), normal_gen);

  dipoles::Dipoles dipoles1;

  mesh_storage::MeshCreator ms;
  ms.ConstructMeshes();
  Loop(coordinates, clk, dipoles1, ms, conf_num, st);

};

auto nameGenerator1 =
    [](StateT st, size_t N) {
      return EnumToStr(st, kStateToStr) + "_" + std::to_string(N);
    };

const std::array<size_t, 5> kRlims = {10, 50, 200, 500, 1000};
const std::array<size_t, 6> kDivs
    {
        1, 10, 40, 100, 200, 400
    };

auto rangeFinder = [](size_t N) -> size_t {
  auto range_num = std::lower_bound(kRlims.begin(), kRlims.end(), N) - kRlims.begin();
  auto currdiv = kDivs[range_num];
  return currdiv;
};

auto secondBench = []
    (auto &clk, file_utils::fileHandler &handler, size_t &mul, size_t N) {
  auto div = rangeFinder(N);
  mul = div;
  auto confNum = 10000 / div;
  std::valarray<FloatType> coordinates(2 * N);
  std::generate(std::begin(coordinates), std::end(coordinates), normal_gen);

  dipoles::Dipoles dipoles1;

  mesh_storage::MeshCreator ms;
  ms.ConstructMeshes();

  for (size_t i = 0; i < confNum; ++i) {
    clk.Tik();
    dipoles1.SetNewCoordinates(coordinates);
    clk.Tak();
    clk.Tik();
    auto sol = dipoles1.Solve();
    clk.Tak();

    clk.Tik();
    dipoles1.GetFullFunction(coordinates, sol);
    clk.Tak();

    clk.Tik();
    ms.ApplyFunction(dipoles1.GetI2Function());
    clk.Tak();

  }
  std::cout << N << '\n';
  std::cout << clk << "\n\n";

};

std::function<std::string(size_t)> nameGenerator2 =
    [](size_t N) {
      return std::to_string(N) + "_";
    };
auto thirdBench = []
    (auto &clk, file_utils::fileHandler &handler, size_t &mul, StateT st, size_t N) {
  auto conf_num = 1000;
  std::valarray<FloatType> coordinates(2 * N);
  std::generate(std::begin(coordinates), std::end(coordinates), normal_gen);

  dipoles::Dipoles dipoles1;

  mesh_storage::MeshCreator ms;
  ms.ConstructMeshes();
  dipoles1.SetNewCoordinates(coordinates);
  auto sol = dipoles1.Solve();
  dipoles1.GetFullFunction(coordinates, sol);
  clk.Tik();
  if (static_cast<size_t>(st) <= 1) {
    for (size_t i = 0; i < conf_num; ++i) {
      if (st == StateT::NEW) {
        ms.ApplyFunction(dipoles1.GetI2Function());
      } else {
        ms.ApplyIntegrate(dipoles1.GetIfunction());
      }

    }
  } else {
#pragma omp parallel for firstprivate(dipoles1, ms), default(shared)
    for (size_t i = 0; i < conf_num; ++i) {

      if (st == StateT::OPENMP_NEW) {
        ms.ApplyFunction(dipoles1.GetI2Function());
      } else {
        ms.ApplyIntegrate(dipoles1.GetIfunction());
      }
    }
  }

  clk.Tak();

};

auto nameGenerator3 =
    [](StateT st, size_t N) {
      return STR(EnumToStr(st, kStateToStr)) + "_" + std::to_string(N);
    };

auto fourthBench = []
    (auto &clk, file_utils::fileHandler &handler, size_t &mul, size_t confNum, size_t N) {
  std::valarray<FloatType> coordinates(2 * N);
  std::generate(std::begin(coordinates), std::end(coordinates), normal_gen);

  dipoles::Dipoles dipoles1;

  mesh_storage::MeshCreator ms;
  ms.ConstructMeshes();

  for (size_t i = 0; i < confNum; ++i) {
    clk.Tik();
    dipoles1.SetNewCoordinates(coordinates);
    clk.Tak();
    clk.Tik();
    auto sol = dipoles1.Solve();
    clk.Tak();

    clk.Tik();
    dipoles1.GetFullFunction(coordinates, sol);
    clk.Tak();

    clk.Tik();
    ms.ApplyFunction(dipoles1.GetI2Function());
    clk.Tak();

  }
  std::cout << N << '\n';
  std::cout << clk << "\n\n";

};

auto nameGenerator4 =
    [](size_t confNum, size_t N) {
      return std::to_string(confNum) + "_" + std::to_string(N);
    };

auto fifthBench = []
    (auto &clk, file_utils::fileHandler &handler,size_t &mul, size_t eigen_threads, size_t N) {
  //Eigen::setNbThreads(eigen_threads);
  size_t conf_num = 10000;
  mul=1;

  dipoles::Dipoles dipoles1;

  mesh_storage::MeshCreator ms;

  ms.ConstructMeshes();

  common_types::StdValarr coordinates(2 * N);

  auto loc = clk.TikLoc();

  common_types::EigenVec sol;

  common_types::StdValarr res = ms.data_[0];
///@todo ticloc with average for threads
#pragma omp parallel firstprivate(coordinates, dipoles1, ms) private(sol)  shared(res) num_threads(eigen_threads)
  {
    thread_local auto functor = generators::get_normal_generator(0.0, kArange);
#pragma for
    for (size_t i = 0; i < conf_num; ++i) {

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

  clk.Tak();
  std::cout << fmt::format("Thread num: {}, N = {} ",eigen_threads,N) << '\n';
  std::cout << clk << "\n\n";
};

auto nameGenerator5 =
    [](size_t thread_count, size_t N) {
      return std::to_string(thread_count) + "_" + std::to_string(N);
    };

int main() {

  for (auto &N : std::array{1ul, 2ul, 4ul, 5ul, 8ul, 10ul,
                            20ul, 40ul, 50ul, 100ul, 200ul, 400ul, 500ul, 800ul, 1000ul, 2000ul}) {
    std::cout << N << '\t' << rangeFinder(N) << '\n';
  }




  BenchmarkHandler bh("benchFirst", {"benchFirst"});
  bh.RunBenchmark(nameGenerator1, firstBench, std::array{StateT::NEW, StateT::OLD},
                  std::array{1ul, 2ul, 4ul, 5ul, 8ul, 10ul, 20ul, 40ul, 50ul}
  );

  BenchmarkHandler bh2("benchSecond", {"benchSecond"});
  bh2.RunBenchmark(nameGenerator2, secondBench, std::array{1ul, 2ul, 4ul, 5ul, 8ul, 10ul,
                                                           20ul, 40ul, 50ul, 100ul, 200ul, 400ul, 500ul, 800ul,
                                                           1000ul, 2000ul}
  );

  BenchmarkHandler bh3("benchThird", {"benchThird"});
  bh3.RunBenchmark(nameGenerator3, thirdBench,
                   std::array{StateT::NEW, StateT::OLD, StateT::OPENMP_NEW, StateT::OPENMP_OLD},
                   std::array{1ul, 2ul, 4ul, 5ul, 8ul, 10ul, 20ul, 40ul, 50ul}

  );

  BenchmarkHandler bh4("benchFourth", {"benchFourth"});
  bh4.RunBenchmark(nameGenerator4, fourthBench,
                   std::array{1ul, 10ul, 100ul, 1000ul, 10000ul},
                   std::array{1ul, 2ul, 4ul, 5ul, 8ul, 10ul,
                              20ul, 40ul, 50ul, 100ul, 200ul}

  );

  BenchmarkHandler bh5("benchFifth", {"benchFifth"});
  bh5.RunBenchmark(nameGenerator5, fifthBench,
                   std::array{1ul, 2ul, 3ul, 4ul, 5ul, 6ul, 7ul, 8ul, 9ul, 10ul, 11ul, 12ul, 13ul, 14ul, 15ul, 16ul},
                   std::array{1ul, 2ul, 4ul, 5ul, 8ul, 10ul,
                              20ul, 40ul, 50ul}

  );

  /*BenchmarkHandler bh6("benchSixth", {"benchSixth"});
  bh6.RunBenchmark(nameGenerator6, sixthBench,
                   std::array{1ul, 2ul, 3ul, 4ul, 5ul, 6ul, 7ul, 8ul, 9ul, 10ul, 11ul, 12ul, 13ul, 14ul, 15ul, 16ul},
                   std::array{1ul, 2ul, 4ul, 5ul, 8ul, 10ul,
                              20ul, 40ul, 50ul}

  );

  BenchmarkHandler bh7("benchSeventh", {"benchSeventh"});
  bh5.RunBenchmark(nameGenerator7, seventhBench,
                   std::array{1ul, 2ul, 3ul, 4ul, 5ul, 6ul, 7ul, 8ul, 9ul, 10ul, 11ul, 12ul, 13ul, 14ul, 15ul, 16ul},
                   std::array{1ul, 2ul, 4ul, 5ul, 8ul, 10ul,
                              20ul, 40ul, 50ul}

  );*/



}
