#include <eigen3/Eigen/Dense>
#include <random>
#include <iostream>
#include "math_core/Dipoles.h"
#include "common/lib.h"
int main(int argc, char* argv[]) {
    int N=10;
    //std::cin>>N;
    using namespace Eigen;
    CoordGenerator<double> genr(0,1e-6);
    auto coords=genr.generateCoordinates(N);
    dipoles::Dipoles d(N,coords);


    double stime=omp_get_wtime();
    d.solve_();
    double ftime = omp_get_wtime()-stime;
    std::cout<<ftime<<"\t"<<N<<"\n";
    /*Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> tt = (M1_ * M1_ + M2_ * M2_).inverse();
            solution_[0] = tt * (M1_ * f1 + M2_ * f2);
            solution_[1] = tt * (M1_ * f2 - M2_ * f1);*/
}