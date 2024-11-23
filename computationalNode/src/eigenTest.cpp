#include <eigen3/Eigen/Dense>
#include <random>
#include <iostream>

#include "common/lib.h"
#include "computationalLib/math_core/Dipoles2.h"

int main(int argc, char *argv[]) {
    int N = 10;
    //std::cin>>N;
    using namespace Eigen;
    CoordGenerator<double> genr(0, 1e-6);
    auto coords = genr.generateCoordinates2(N);
    dipoles1::Dipoless d(N, coords);//todo old implementation used


    double stime = omp_get_wtime();
    d.solve2();
    double ftime = omp_get_wtime() - stime;
    std::cout << ftime << "\t" << N << "\n";
    /*Eigen::Matrix<Tr, Eigen::Dynamic, Eigen::Dynamic> tt = (M1_ * M1_ + M2_ * M2_).inverse();
            solution_[0] = tt * (M1_ * f1 + M2_ * f2);
            solution_[1] = tt * (M1_ * f2 - M2_ * f1);*/
}