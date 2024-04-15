#include <eigen3/Eigen/Dense>
#include <random>
#include <iostream>
#include "Dipoles.h"
#include "lib.h"
#include <matplot/matplot.h>
#include "MeshProcessor.h"
#include <vector>

//template <class T>
std::array<std::vector<double>,2> triangularGrid(double l,double r,double a=0,double b=0)//r-r,l-длинна
{
    long double c1= 1/2.0;
    long double c2= sqrt(3)/2.0;
    double k=c2/c1;
    double c=0;

    double D=sqrt(k*k*r*r/(k*k+1));
    std::vector<double> x_t={-D,D};
    std::vector<double> y_t(2);
    for (int i = 0; i < 2; ++i) {
        y_t[i]=-(x_t[i])/k;
    }
    std::vector<double> c_t(2);

    for (int i = 0; i < 2; ++i) {
        c_t[i]=y_t[i]-x_t[i]*k;
    }
    std::vector<int> l_t(2,0);

    for (int i = 0; i < 2; ++i) {
        l_t[i]=-(c_t[i])/(k*l);
    }
    if(l_t[0]>0)
    {
        std::swap(l_t[0],l_t[1]);
    }


    //todo 2 круг симметричная фигура-> можно подсчитать результаты в 1 четверти
    //а что будет если поместить треугольники так, чробы центр центрального был в центре круга
    int funccount=std::abs(l_t[1]-l_t[0])+1;
    std::vector<double> x1= myLinspace(l_t[0]*l,l_t[1]*l,funccount);
    std::vector<double> y1(funccount,0);

    std::vector<double> x(funccount*funccount,0);
    std::vector<double> y(funccount*funccount,0);



    for (int i = 0; i < funccount; ++i) {
        for (int j = 0; j < funccount; ++j) {

            x[j+i*funccount]=x1[i]/2+x1[j]/2+(c1/c2)*(y1[i]+y1[j])/2;
            y[j+i*funccount]=(-c2/c1)*x[j+i*funccount]+(c2/c1)*x1[i]-y1[i];

            x[j+i*funccount]+=a;
            y[j+i*funccount]+=b;
        }
    }




    std::vector<double> x_filtered, y_filtered;

    for (int i = 0; i < funccount*funccount; ++i) {
        if (std::sqrt(pow(x[i]-a,2) + pow(y[i]-b,2)) <= r) {
            x_filtered.push_back(x[i]);
            y_filtered.push_back(y[i]);
        }
    }


    std::cout<<x_filtered.size()<<'\t'<<x.size()<<'\t'<<x_filtered.size()/(1.0*x.size())<<"\n";


    return {x_filtered, y_filtered};
}
int main(int argc, char* argv[]) {
    int N=10;
    //std::cin>>N;
    using namespace Eigen;
    double a=1e-6;
    double l=a/10.0;
    std::cin>>a>>l;

    double x1,y1;
    std::cin>>x1>>y1;


   // CoordGenerator<double> genr(0.0,a);
    auto coords=triangularGrid(l,a,x1,y1);
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

    auto x=transform(t, [&a,&x1](auto t) { return a*cos(t)+x1; });
    auto y=transform(t, [&a,&y1](auto t) { return a*sin(t)+y1; });
    ax->scatter(x,y);
    hold(off);



    //ax->ezpolar(ss.str());

    //std::cout<<"\n\n\n";

    save("result.png");


}