
#include <iostream>
#include <utility>
#include <tuple>
#include <algorithm>

#include <omp.h>

#include "fileHandler.h"
#include "benchmarkHandler.h"
#include "common/Generator.h"
#include "computationalLib/math_core/Dipoles.h"
#include "common/MeshCreator.h"
#include "common/sharedDeclarations.h"


using fileUtils::fileHandler;
using namespace shared;
using benchUtils::benchmarkHandler;

void func(int a, char b, uint c) {
    std::cout << a << '\n' << b << '\n' << c << '\n';
}


constexpr auto chararr = std::array<char, 3>{'a', 'b', 'c'};
constexpr auto intarr = std::array<int, 4>{1, 2, 3, 4};
constexpr FloatType arange = 1e-6;


void
loop(const std::valarray<FloatType> &coordinates, auto &clk, dipoles::Dipoles &dipoles1,
     meshStorage::MeshCreator &ms, size_t confNum, state_t st) {
    for (size_t i = 0; i < confNum; ++i) {
        clk.tik();
        dipoles1.setNewCoordinates(coordinates);
        clk.tak();
        clk.tik();
        auto sol = dipoles1.solve();
        clk.tak();


        clk.tik();
        dipoles1.getFullFunction_(coordinates, sol);
        clk.tak();

        clk.tik();
        if (st == state_t::new_) {
            ms.applyFunction(dipoles1.getI2function());
        } else {
            ms.applyIntegrate(dipoles1.getIfunction());
        }
        clk.tak();

    }

}


auto firstBench = []
        (auto &clk, fileUtils::fileHandler &handler, size_t &mul, state_t st, size_t N) {
    auto confNum = 1000;
    auto sig = arange * sqrt(2);
    auto coordinates = generators::normal<std::valarray>(N, 0.0, sig);


    dipoles::Dipoles dipoles1;

    meshStorage::MeshCreator ms;
    ms.constructMeshes();
    loop(coordinates, clk, dipoles1, ms, confNum, st);


};


auto nameGenerator1 =
        [](state_t st, size_t N) {
            return STR(ENUM_TO_STR(st, stateToStr)) + "_" + std::to_string(N);
        };


const std::array<size_t, 5> rlims = {10, 50, 200, 500, 1000};
const std::array<size_t, 6> divs
        {
                1, 10, 40, 100, 200, 400
        };

auto rangeFinder = [](size_t N) -> size_t {
    auto rangeNum = std::lower_bound(rlims.begin(), rlims.end(), N) - rlims.begin();
    auto currdiv = divs[rangeNum];
    return currdiv;
};

auto secondBench = []
        (auto &clk, fileUtils::fileHandler &handler, size_t &mul, size_t N) {
    auto div = rangeFinder(N);
    mul = div;
    auto confNum = 10000 / div;
    auto sig = arange * sqrt(2);
    auto coordinates = generators::normal<std::valarray>(N, 0.0, sig);


    dipoles::Dipoles dipoles1;

    meshStorage::MeshCreator ms;
    ms.constructMeshes();


    for (size_t i = 0; i < confNum; ++i) {
        clk.tik();
        dipoles1.setNewCoordinates(coordinates);
        clk.tak();
        clk.tik();
        auto sol = dipoles1.solve();
        clk.tak();


        clk.tik();
        dipoles1.getFullFunction_(coordinates, sol);
        clk.tak();

        clk.tik();
        ms.applyFunction(dipoles1.getI2function());
        clk.tak();

    }
    std::cout << N << '\n';
    std::cout << clk << "\n\n";


};


std::function<std::string(size_t)> nameGenerator2 =
        [](size_t N) {
            return std::to_string(N) + "_";
        };
auto thirdBench = []
        (auto &clk, fileUtils::fileHandler &handler, size_t &mul, state_t st, size_t N) {
    auto confNum = 1000;
    auto sig = arange * sqrt(2);
    auto coordinates = generators::normal<std::valarray>(N, 0.0, sig);


    dipoles::Dipoles dipoles1;

    meshStorage::MeshCreator ms;
    ms.constructMeshes();
    dipoles1.setNewCoordinates(coordinates);
    auto sol = dipoles1.solve();
    dipoles1.getFullFunction_(coordinates, sol);
    clk.tik();
    if (static_cast<size_t>(st) <= 1) {
        for (size_t i = 0; i < confNum; ++i) {
            if (st == state_t::new_) {
                ms.applyFunction(dipoles1.getI2function());
            } else {
                ms.applyIntegrate(dipoles1.getIfunction());
            }

        }
    } else {
#pragma omp parallel for firstprivate(dipoles1, ms), default(shared)
        for (size_t i = 0; i < confNum; ++i) {

            if (st == state_t::openmp_new) {
                ms.applyFunction(dipoles1.getI2function());
            } else {
                ms.applyIntegrate(dipoles1.getIfunction());
            }
        }
    }

    clk.tak();


};

auto nameGenerator3 =
        [](state_t st, size_t N) {
            return STR(ENUM_TO_STR(st, stateToStr)) + "_" + std::to_string(N);
        };


auto fourthBench = []
        (auto &clk, fileUtils::fileHandler &handler, size_t &mul, size_t confNum, size_t N) {
    auto sig = arange * sqrt(2);
    auto coordinates = generators::normal<std::valarray>(N, 0.0, sig);


    dipoles::Dipoles dipoles1;

    meshStorage::MeshCreator ms;
    ms.constructMeshes();


    for (size_t i = 0; i < confNum; ++i) {
        clk.tik();
        dipoles1.setNewCoordinates(coordinates);
        clk.tak();
        clk.tik();
        auto sol = dipoles1.solve();
        clk.tak();


        clk.tik();
        dipoles1.getFullFunction_(coordinates, sol);
        clk.tak();

        clk.tik();
        ms.applyFunction(dipoles1.getI2function());
        clk.tak();

    }
    std::cout << N << '\n';
    std::cout << clk << "\n\n";


};


auto nameGenerator4 =
        [](size_t confNum, size_t N) {
            return std::to_string(confNum) + "_" + std::to_string(N);
        };


int main() {

    for (auto &N: std::array{1ul, 2ul, 4ul, 5ul, 8ul, 10ul,
                             20ul, 40ul, 50ul, 100ul, 200ul, 400ul, 500ul, 800ul, 1000ul, 2000ul}) {
        std::cout << N << '\t' << rangeFinder(N) << '\n';
    }


    benchmarkHandler bh("benchFirst", {"benchFirst"});
    bh.runBenchmark(nameGenerator1, firstBench, std::array{state_t::new_, state_t::old},
                    std::array{1ul, 2ul, 4ul, 5ul, 8ul, 10ul, 20ul, 40ul, 50ul}
    );


    benchmarkHandler bh2("benchSecond", {"benchSecond"});
    bh2.runBenchmark(nameGenerator2, secondBench, std::array{1ul, 2ul, 4ul, 5ul, 8ul, 10ul,
                                                             20ul, 40ul, 50ul, 100ul, 200ul, 400ul, 500ul, 800ul,
                                                             1000ul, 2000ul}
    );

    benchmarkHandler bh3("benchThird", {"benchThird"});
    bh3.runBenchmark(nameGenerator3, thirdBench,
                     std::array{state_t::new_, state_t::old, state_t::openmp_new, state_t::openmp_old},
                     std::array{1ul, 2ul, 4ul, 5ul, 8ul, 10ul, 20ul, 40ul, 50ul}

    );

    benchmarkHandler bh4("benchFourth", {"benchFourth"});
    bh4.runBenchmark(nameGenerator4, fourthBench,
                     std::array{1ul, 10ul, 100ul, 1000ul, 10000ul},
                     std::array{1ul, 2ul, 4ul, 5ul, 8ul, 10ul,
                                20ul, 40ul, 50ul, 100ul, 200ul}

    );

}
