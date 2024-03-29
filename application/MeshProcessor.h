//
// Created by Lenovo on 28.03.2024.
//

#ifndef DIPLOM_MESHPROCESSOR_H
#define DIPLOM_MESHPROCESSOR_H
#include <matplot/matplot.h>
#include <ostream>
#include <vector>
#include <boost/math/quadrature/gauss_kronrod.hpp>
#include "printUtils.h"


template<class T>
T integrate(std::function<T(T)> &f1, T left, T right) {
    T error;
    T Q = boost::math::quadrature::gauss_kronrod<T, 61>::integrate(f1, left, right, 5, 1e-20, &error);
    return Q;
}
template<class T>
T integrate(const std::function<T(T)> &&f1, T left, T right) {
    T error;
    double Q = boost::math::quadrature::gauss_kronrod<T, 61>::integrate(f1, left, right, 5, 1e-20, &error);
    return Q;
}

template<class T>
T integrateFunctionBy1Val(const std::function<T(T, T, T)>&ff, T theta, T phi,T left,T right)
{
    std::function<T(T)> tt=[&theta,&phi,&ff](T t){return ff(theta,phi,t);};
    return integrate(tt,left,right);
}
template<class T>
T getMeshDiffNorm(std::vector<std::vector<T>>& mesh1,std::vector<std::vector<T>>& mesh2);




using namespace matplot;
template <typename T>
std::pair<std::vector<std::vector<T>>,std::vector<std::vector<T>>> mymeshGrid(std::vector<T>&&a,std::vector<T>&&b);



template <typename T>
std::vector<T> myLinspace(T lower_bound, T upper_bound, size_t n, bool end=true);

template<typename T>
std::vector<T> myLinspace(T lower_bound, T upper_bound, size_t n, bool end) {
    std::vector<T> result(n,T());

    T div=(end)?(n - 1):n;
    T step = (upper_bound - lower_bound) / div;

    for (int i = 0; i < n; ++i) {
        result[i]=i*step+lower_bound;
    }
    return result;
}


template<typename T>
std::pair<std::vector<std::vector<T>>, std::vector<std::vector<T>>> mymeshGrid(std::vector<T> &&a, std::vector<T> &&b) {
    std::vector<std::vector<T>> x_mesh(b.size(), std::vector<T>(a.size()));
    std::vector<std::vector<T>> y_mesh(b.size(), std::vector<T>(a.size()));
    for (size_t i = 0; i < b.size(); ++i) {
        x_mesh[i]=a;
        for (size_t j = 0; j < a.size(); ++j) {
            y_mesh[i][j] = b[i];
        }
    }

    return std::make_pair(x_mesh, y_mesh);
}

template <typename T>
class MeshProcessor {
public:
    const std::array<std::vector<std::vector<T>>, 3> &getMeshdec() const {
        return meshdec;
    }

    const std::array<std::vector<std::vector<T>>, 3> &getMeshsph() const {
        return meshsph;
    }
    void generateMeshes(const std::function<T(T,T,T)>&func);

    void plotSpherical(std::string filename);

    void printDec(std::ostream&out);

private:
    std::array<std::vector<std::vector<T>>,3>meshdec;
    std::array<std::vector<std::vector<T>>,3>meshsph;

    T philims[2]={0, M_PI*2};
    T thelims[2]={0, M_PI_2};
    T steps[2]={M_PI/12, M_PI/12};
    T nums[2]={(philims[1]-philims[0])/(steps[0])+1, (thelims[1]-thelims[0])/(steps[1])+1};
    const T omega=pow(10,15);//todo константа вынести
    const T rr=2*M_PI/omega;//todo константа вынести
    const T step=M_PI/12;//todo константа вынести
};

template<typename T>
void MeshProcessor<T>::printDec(std::ostream &out) {
    out<<"Функция I(phi,th)\n";
    out<<"phi\\th\t\t";
    for (T theta = thelims[0]; theta < thelims[1]; theta+=steps[1]) {//todo не перепуктали ли мы аргументы
        out<<scientificNumber(theta,5)<<"\t";
    }
    out<<"\n";
    int i1=0;
    int i2;
    for (T phi = philims[0]; phi < philims[1]; phi+=steps[0]) {
        out<<scientificNumber(phi,5)<<"\t";
        i2=0;
        i1++;
        for (T theta = thelims[0]; theta < thelims[1]; theta+=steps[1]) {
            //out<<scientificNumber(integrateFunctionBy1Val(ff2,theta,phi,0.0,rr),5)<<"\t";
            //T val=
            out<<scientificNumber(meshdec[2][i2][i1],5)<<"\t";
            i2++;
        }
        out<<"\n";
    }
}


template<typename T>
void MeshProcessor<T>::plotSpherical(std::string filename) {
    auto ax=gca();
    ax->surf(meshsph[0], meshsph[1], meshsph[2]);//-> view(213,22)->xlim({-40,40})->ylim({-40,40});
    //surf(x, y, z);
    view(213,22);
    xlim({-40,40});
    ylim({-40,40});
    matplot::save(filename);
}

template<typename T>
void MeshProcessor<T>::generateMeshes(const std::function<T(T, T, T)> &func) {
    int num1 =(philims[1]-philims[0])/step;
    std::pair<std::vector<std::vector<T>>, std::vector<std::vector<T>>> meshgrid1=meshgrid(linspace(philims[0], philims[1],nums[0]),
                                                                                          linspace(thelims[0], thelims[1], nums[1]));
    meshdec[0]=meshgrid1.first;
    meshdec[1]=meshgrid1.second;
    T rr1=this->rr;
    meshdec[2] = transform(meshdec[0], meshdec[1], [&func,&rr1](T x, T y) {
        return integrateFunctionBy1Val<T>(func,y,x,0,rr1);
    });
    meshsph[0]=meshdec[0];
    meshsph[1]=meshdec[1];
    meshsph[2]=meshdec[2];
    //auto x=phi;
    //auto y=theta;
   // auto z=func;
    for (int i = 0; i < meshsph[0].size(); ++i) {
        for (int j = 0; j < meshsph[0][0].size(); ++j) {
            meshsph[0][i][j]=meshsph[2][i][j]*sin(meshdec[1][i][j])*cos(meshdec[0][i][j]);
            meshsph[1][i][j]=meshsph[2][i][j]*sin(meshdec[1][i][j])*sin(meshdec[0][i][j]);
            meshsph[2][i][j]=meshsph[2][i][j]*cos(meshdec[1][i][j]);
        }
    }


}


template<class T>
T getMeshDiffNorm(std::vector<std::vector<T>>& mesh1,std::vector<std::vector<T>>& mesh2)
{
    T res=0;
    for (int i = 0; i < mesh1.size(); ++i) {
        for (int j = 0; j < mesh1[0].size(); ++j) {
            res+=pow(mesh1[i][j]-mesh2[i][j],2);
        }
    }
    return sqrt(res);
}



#endif //DIPLOM_MESHPROCESSOR_H
