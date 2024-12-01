#include <eigen3/Eigen/Dense>
#include <random>
#include <iostream>

#include "common/lib.h"
#include "computationalLib/math_core/Dipoles.h"
#include "parallelUtils/commonDeclarations.h"
//todo maybe remove
int main(int argc, char *argv[]) {
    int N = 10;
    //std::cin>>N;
    using namespace Eigen;
    CoordGenerator<double> genr(0, 1e-6);
    auto coords = genr.generateCoordinates2(N);
    dipoles::Dipoles d(N, coords);//todo old implementation used


    auto stime = commonDeclarations::gClk.tikLoc();
    d.solve<dipoles::Arr2EigenVec>();
    commonDeclarations::gClk.tak();
    std::cout << commonDeclarations::gClk[stime].time << "\t" << N << "\n";

}