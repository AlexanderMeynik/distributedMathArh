#include <eigen3/Eigen/Dense>
#include <random>
#include <iostream>
#include "Dipoles.h"
#include "lib.h"
#include <matplot/matplot.h>
#include "MeshProcessor.h"


//template <class T>
std::array<std::vector<double>,2> triangularGrid(double l,double a)//a-r,l-длинна
{
    long double c1= 1/2.0;
    long double c2= sqrt(3)/2.0;
    //std::array<std::vector<T>,2> res={std::vector<T>(),std::vector<T>()};
    int coef=a/l+100;//todo надо определить дургое улсовие для того, чтобы затронуть все
    //todo 2 круг симметричная фигура-> можно подсчитать результаты в 1 четверти
    int funccount=(2*coef)+1;
    std::vector<double> x1= myLinspace(-coef*l,coef*l,funccount);
    std::vector<double> y1(funccount,0);

    std::vector<double> x(funccount*funccount,0);
    std::vector<double> y(funccount*funccount,0);

    for (int i = 0; i < funccount; ++i) {
        for (int j = 0; j < funccount; ++j) {

            x[j+i*funccount]=x1[i]/2+x1[j]/2+(c1/c2)*(y1[i]+y1[j])/2;
            y[j+i*funccount]=(-c2/c1)*x[j+i*funccount]+(c2/c1)*x1[i]-y1[i];
        }
    }

    std::vector<double> x_filtered, y_filtered;

    for (int i = 0; i < funccount*funccount; ++i) {
        if (std::sqrt(x[i]*x[i] + y[i]*y[i]) <= a) {
            x_filtered.push_back(x[i]);
            y_filtered.push_back(y[i]);
        }
    }



    return {x_filtered, y_filtered};
}
int main(int argc, char* argv[]) {
    int N=10;
    //std::cin>>N;
    using namespace Eigen;
    double a=1e-6;
    double l=a/10.0;
    std::cin>>a>>l;


   // CoordGenerator<double> genr(0.0,a);
    auto coords=triangularGrid(l,a);
    N=coords[0].size();

    auto ax=gca();

    /* for (int i = 0; i < xi_[0].size(); ++i) {
         std::cout<<xi_[0][i]<<'\t'<<xi_[1][i]<<"\n";
     }*/

    ax->scatter(coords[0],coords[1]);//-> view(213,22)->xlim({-40,40})->ylim({-40,40});

    hold(on);

    std::stringstream ss;
    ss<<a;

    auto t= myLinspace(0.0,2*M_PI,200);

    auto x=transform(t, [&a](auto t) { return a*cos(t); });
    auto y=transform(t, [&a](auto t) { return a*sin(t); });
    ax->scatter(x,y);
    hold(off);



    //ax->ezpolar(ss.str());

    //std::cout<<"\n\n\n";

    save("result.png");


}