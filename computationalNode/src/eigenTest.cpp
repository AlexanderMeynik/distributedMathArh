#include <eigen3/Eigen/Dense>
#include <random>
#include <iostream>

#include "common/lib.h"
#include "computationalLib/math_core/Dipoles.h"
#include "parallelUtils/commonDeclarations.h"
#include "common/Generator.h"
#include "iolib/Printers.h"
#include "computationalLib/math_core/MeshCreator.h"
auto aRange=1.0e-6;

using commonDeclarations::gClk;

int main(int argc, char *argv[]) {
    /*int N = 10;
    timing::chronoClockTemplate<std::milli > clk;
    //std::cin>>N;
    using namespace Eigen;
    *//*CoordGenerator<double> genr(0, 1e-6);*//*
    auto coords = generators::normal<std::vector>(N,0.0,aRange* sqrt(2));
    dipoles::Dipoles d(N, coords);
    auto dur=std::chrono::duration<double,std::milli>(10);



    auto stime = clk.tikLoc();
    *//*std::this_thread::sleep_for(dur);*//*
    auto res=d.solve<dipoles::standartVec>();
   clk.tak();
    std::cout << clk[stime].time << "\t" << N << "\n";*/
    std::vector<double> a={0,1,6,1,0,-6};
    plotCoordinates("ass.png",1,a);

}