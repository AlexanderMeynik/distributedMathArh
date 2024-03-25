#include <boost/math/quadrature/gauss_kronrod.hpp>
#include <vector>
#include <eigen3/Eigen/Dense>
#include <matplot/matplot.h>
using namespace Eigen;
const double c = 3.0 * pow(10, 8);
const double yo = pow(10, 7);
const double omega = pow(10, 15);
const double omega0 = omega;
const  double a = 1;
const double eps = 1;
const double an=a;
const double rr=2*M_PI/omega;


using Eigen::Vector2d;
template<class T>
T integrate(std::function<T(T)> &f1, T left, T right) {
    T error;
    T Q = boost::math::quadrature::gauss_kronrod<T, 61>::integrate(f1, left, right, 5, 1e-20, &error);
    return Q;
}
template<class T>
T integrate(std::function<T(T)> &&f1, T left, T right) {
    T error;
    double Q = boost::math::quadrature::gauss_kronrod<T, 61>::integrate(f1, left, right, 5, 1e-20, &error);
    return Q;
}
template<class T>
std::function<T(T,T,T)> getFullFunction(std::array<std::vector<T>, 2>&coefs,std::array<std::vector<T>, 2>&coords)
{


    //T omeg=omega;//todo можно посчитать все величины заранее и потом прост оподставить
    return [&coords,&coefs](T theta,T phi,T t)
    {
        T omega=pow(10,15);
        T c=3*pow(10,8);
        int N=coords[0].size();
        T res=0;
        T s[2]={cos(phi),sin(phi)};
        T ress[3]={0,0,0};
        for (int i = 0; i < N; ++i) {
            T ri[2]={coords[0][i],coords[1][i]};
            T ys=(ri[1]*cos(phi)-ri[0]*sin(phi))*sin(theta);
            T t0=t-ys/c;
            T Ai[2]={coefs[0][2*i],coefs[0][2*i+1]};
            T Bi[2]={coefs[1][2*i],coefs[1][2*i+1]};


            T Di[2]={Ai[0]* cos(omega*t0)+Bi[0]* sin(omega*t0),
                     Ai[1]* cos(omega*t0)+Bi[1]* sin(omega*t0)};

            T vi[2]={omega*(Bi[0]* cos(omega*t0)-Ai[0]* sin(omega*t0))/c,
                     omega*(Bi[1]* cos(omega*t0)-Ai[1]* sin(omega))/c};
            T ai[2]={-pow(omega,2)*Di[0],-pow(omega,2)*Di[1]};

            T vsi= vi[0] * s[0] + vi[1] * s[1];
            T asi= ai[0] * s[0] + ai[1] * s[1];

            for (int coord = 0; coord < 2; ++coord) {
                T ttt= ai[coord]*(vsi*sin(theta)-1)+
                       s[coord]*asi* pow(sin(theta),2)-
                       vi[coord]*asi*sin(theta);
                ress[coord]+=ttt;
            }

            T t3= asi * sin(theta) * cos(theta);
            ress[2]+=t3;
        }
        for (T elem : ress) {
            res+=elem*elem;
        }
        return res;
    };

}
template<class T>
T integrateFunctionBy1Val(std::function<T(T, T, T)>&ff, T theta, T phi,T left,T right)
{
    //std::function<T(T)> tt=ff(theta,phi);
    std::function<T(T)> tt=[&theta,&phi,&ff](T t){return ff(theta,phi,t);};
    return integrate(tt,left,right);
}

int main(int argc, char *argv[]) {
    double error;
    //int N=2;
    std::array<std::vector<double>, 2> ccs;



   /* ccs[0] = {-4.02101e-24,
              -4.98918e-23,
              -4.02101e-24,
              -4.98918e-23};
    ccs[1] = {4.73471e-23,
              2.63096e-24,
              4.73471e-23,
              2.63096e-24};*/

    ccs[0]={0,
    -1e-22};
    ccs[1]={1e-22,
    0};
    std::array<std::vector<double>, 2> coords;
    /*coords[0]={0,	1e-07};
    coords[1]={0,	0};*/
    coords[0]={0};
    coords[1]={0};

    /*double left=0;
    double right=2*M_PI;*/
    std::function<double(double,double,double)>ff=[](double theta,double phi,double t){return theta*100+phi*1000+(t);};
    //std::cout<<integrate([](double x)->double {return sin(x)+x;},left,right);
    /*for (int i = 0; i < 10; ++i) {
        std::cout << integrateFunctionBy1Val(ff, i, i,0,1) << "\t" << i * 100 + i * 1000 + (1 / 2.0) << "\n";
    }*/



    std::function<double(double,double,double)>ff2=getFullFunction(ccs,coords);
    double step=M_PI/12;
    std::cout<<"phi\\th\t";
    for (double theta = 0; theta < M_PI/2; theta+=step) {
        std::cout<<theta<<" ";
    }
    std::cout<<"\n";
    for (double phi = 0; phi < M_PI*2; phi+=step) {
        std::cout<<phi<<" ";
        for (double theta = 0; theta < M_PI/2; theta+=step) {
            std::cout<<integrateFunctionBy1Val<double>(ff2,theta,phi,0,rr)<<" ";
        }
        std::cout<<"\n";
    }

    using namespace matplot;
    auto [phi, theta] = meshgrid(linspace(0, M_PI*2, 25), linspace(0, M_PI_2, 7));
    auto Z = transform(phi, theta, [&ff2](double x, double y) {
        return integrateFunctionBy1Val<double>(ff2,y,x,0,rr);
    });
    surf(phi, theta, Z);
    //"66,213"
    view(66, 213);
    show();

    view(213,22);
    show();


    //double Q = boost::math::quadrature::gauss_kronrod<double, 61>::integrate(f1, left, right, 5, 1e-20, &error);
    return 0;
}