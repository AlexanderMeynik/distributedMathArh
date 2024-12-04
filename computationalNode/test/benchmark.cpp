
#include <iostream>
#include <utility>
#include <tuple>


#include <omp.h>

#include "fileHandler.h"
#include "benchmarkHandler.h"
#include "common/Generator.h"
#include "computationalLib/math_core/Dipoles.h"
#include "computationalLib/math_core/MeshCreator.h"
#include "common/lib.h"


using fileUtils::fileHandler;
std::filesystem::path dir("becnhmarkData");
using benchUtils::benchmarkHandler;

void func(int a, char b, uint c) {
    std::cout << a << '\n' << b << '\n' << c << '\n';
}


constexpr auto chararr = std::array<char, 3>{'a', 'b', 'c'};
constexpr auto intarr = std::array<int, 4>{1, 2, 3, 4};
constexpr FloatType arange = 1e-6;

void
loop(const std::vector<std::valarray<FloatType>> &coordinates, benchUtils::clk1 &clk, dipoles::Dipoles &dipoles1,
     meshStorage::MeshCreator &ms, size_t confNum, state_t st) {
    for (size_t i = 0; i < confNum; ++i) {
        clk.tik();
        dipoles1.setNewCoordinates(coordinates[i]);
        clk.tak();
        clk.tik();
        auto sol = dipoles1.solve();
        clk.tak();


        clk.tik();
        dipoles1.getFullFunction_(coordinates[i], sol);
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
        (benchUtils::clk1 &clk, fileUtils::fileHandler &handler, size_t N, state_t st) {
    auto confNum = 100;
    auto sig = arange * sqrt(2);
    std::vector<std::valarray<FloatType>> coordinates;
    coordinates.resize(confNum);
    clk.tik();
    for (auto &it: coordinates) {
        it = generators::normal<std::valarray>(N, 0.0, arange);
    }
    clk.tak();

    dipoles::Dipoles dipoles1;

    meshStorage::MeshCreator ms;
    ms.constructMeshes();
    loop(coordinates, clk, dipoles1, ms, confNum, st);


};


std::function<std::string(size_t, state_t)> nameGenerator1 =
        [](size_t N, state_t st) {
            return std::to_string(N) + "_" + stateToString.at(st);
        };

auto secondBench = []
        (benchUtils::clk1 &clk, fileUtils::fileHandler &handler, size_t N) {
    auto confNum = 10000;
    auto sig = arange * sqrt(2);
    std::vector<std::valarray<FloatType>> coordinates;
    coordinates.resize(confNum);
    clk.tik();
    for (auto &it: coordinates) {
        it = generators::normal<std::valarray>(N, 0.0, arange);
    }
    clk.tak();

    dipoles::Dipoles dipoles1;

    meshStorage::MeshCreator ms;
    ms.constructMeshes();


    for (size_t i = 0; i < confNum; ++i) {
        clk.tik();
        dipoles1.setNewCoordinates(coordinates[i]);
        clk.tak();
        clk.tik();
        auto sol = dipoles1.solve();
        clk.tak();


        clk.tik();
        dipoles1.getFullFunction_(coordinates[i], sol);
        clk.tak();

        clk.tik();
        ms.applyFunction(dipoles1.getI2function());
        clk.tak();

    }
    std::cout<<N<<'\n';
    std::cout<<clk<<"\n\n";


};


std::function<std::string(size_t)> nameGenerator2 =
        [](size_t N) {
            return std::to_string(N) + "_";
        };
auto thirdBench = []
        (benchUtils::clk1 &clk, fileUtils::fileHandler &handler, size_t N, state_t st) {
    auto confNum = 100;
    auto sig = arange * sqrt(2);
    auto coordinates=generators::normal<std::valarray>(N, 0.0, arange);


    dipoles::Dipoles dipoles1;

    meshStorage::MeshCreator ms;
    ms.constructMeshes();
    dipoles1.setNewCoordinates(coordinates);
    auto sol =dipoles1.solve();
    dipoles1.getFullFunction_(coordinates, sol);
    clk.tik();
    if(static_cast<size_t>(st)<=1) {
        for (size_t i = 0; i < confNum; ++i) {
            if (st == state_t::new_) {
                ms.applyFunction(dipoles1.getI2function());
            } else {
                ms.applyIntegrate(dipoles1.getIfunction());
            }

        }
    }
    else
    {
#pragma omp parallel for firstprivate(dipoles1,ms), default(shared)
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


std::function<std::string(size_t, state_t)> nameGenerator3 =
        [](size_t N, state_t st) {
            return std::to_string(N) + "_" + stateToString.at(st);
        };
int main() {


    benchmarkHandler bh("benchFirst", {"benchFirst"});
    bh.runThing(nameGenerator1, firstBench, std::array{1ul, 2ul, 4ul, 8ul,10ul,20ul},
                std::array{state_t::new_, state_t::old});


    benchmarkHandler bh2("benchSecond", {"benchSecond"});
    bh2.runThing(nameGenerator2, secondBench, std::array{1ul, 2ul, 4ul, 8ul,10ul,
                                                         20ul,40ul,50ul,100ul,200ul}
                );

    benchmarkHandler bh3("benchThird", {"benchThird"});
    bh3.runThing(nameGenerator3, thirdBench,std::array{1ul, 2ul, 4ul, 8ul,10ul,20ul},
                 std::array{state_t::new_, state_t::old,state_t::openmp_new,state_t::openmp_old}
    );

}

////todo integration accuracy/speed depending on Ndots and quadrature
////todo gbench
