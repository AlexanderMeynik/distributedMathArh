//
// Created by Lenovo on 25.03.2024.
//

#ifndef MAGISTER1_LIB_H
#define MAGISTER1_LIB_H


#include <iostream>
#include "Dipoles.h"
#include <vector>
#include <algorithm>
#include <numeric>
#include <ostream>
#include <fstream>
#include <iomanip>
#include <filesystem>
#include <boost/math/quadrature/gauss_kronrod.hpp>
#include <matplot/matplot.h>
#include "MeshProcessor.h"

using Eigen::Vector2d;
/*template<class T>
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
T integrateFunctionBy1Val(std::function<T(T, T, T)>&ff, T theta, T phi,T left,T right)
{
    std::function<T(T)> tt=[&theta,&phi,&ff](T t){return ff(theta,phi,t);};
    return integrate(tt,left,right);
}*/

template<class T>
std::vector<std::array<std::vector<T>,2>> parseConf(string &filename);

template<class T>
void printToFile(int N, std::array<std::vector<T>,2> &a, dipoles::Dipoles <T> &d, string &basicString, int id, int verboseLevel);

template<class T>
void generateMeshes(int N,int id, dipoles::Dipoles<T> & d,std::ostream&out,std::string &basicString);



/*template<class T>
void generateMeshes(int N, int id, dipoles::Dipoles<T> &d, ostream &out, string &basicString) {
    using namespace matplot;

    const T omega=pow(10,15);//todo константа вынести
    const T rr=2*M_PI/omega;//todo константа вынести
    const T step=M_PI/12;//todo константа вынести
    d.getFullFunction();
    std::function<T(T,T,T)>ff2=d.getIfunction();


    out<<"Функция I(phi,th)\n";
    out<<"phi\\th\t\t";
    for (T theta = 0; theta < M_PI/2; theta+=step) {
        out<<scientificNumber(theta,5)<<"\t";
    }
    out<<"\n";
    for (T phi = 0; phi < M_PI*2; phi+=step) {
        out<<scientificNumber(phi,5)<<"\t";
        for (T theta = 0; theta < M_PI/2; theta+=step) {
            out<<scientificNumber(integrateFunctionBy1Val(ff2,theta,phi,0.0,rr),5)<<"\t";
        }
        out<<"\n";
    }


    auto [phi, theta] = meshgrid(linspace(0, M_PI*2, 25), linspace(0, M_PI_2, 7));
    auto func = transform(phi, theta, [&ff2,&rr](double x, double y) {
        return integrateFunctionBy1Val<double>(ff2,y,x,0,rr);
    });

    auto x=phi;
    auto y=theta;
    auto z=func;
    for (int i = 0; i < x.size(); ++i) {
        for (int j = 0; j < x[0].size(); ++j) {
            x[i][j]=func[i][j]*sin(theta[i][j])*cos(phi[i][j]);
            y[i][j]=func[i][j]*sin(theta[i][j])*sin(phi[i][j]);
            z[i][j]=func[i][j]*cos(theta[i][j]);
        }
    }

    auto ax=gca();
    ax->surf(x, y, z);//-> view(213,22)->xlim({-40,40})->ylim({-40,40});
    //surf(x, y, z);
    view(213,22);
    xlim({-40,40});
    ylim({-40,40});
    matplot::save(basicString+"/out"+std::to_string(N)+"_"+std::to_string(id)+"_.png");
}*/


template<class T>
void
printToFile(int N, array<std::vector<T>, 2> &a, dipoles::Dipoles<T> &d, string &basicString, int id, int verboseLevel) {
    auto solut=d.getSolution_();
    std::ofstream out(basicString+"/out"+std::to_string(N)+"_"+std::to_string(/*a[N-1][0]/l*/id)+"_.txt");
    Eigen::IOFormat CleanFmt(Eigen::StreamPrecision, 0, "\t", "\n", "", "");
    if(verboseLevel>=3) {
        //out << "Матрица\n" << d.getMatrixx().format(CleanFmt) << "\n\n";
        d.printMatrix(out,CleanFmt);
    }
    /*out<<"Координаты диполей\n";
    for (int i = 0; i < a[0].size(); ++i) {
        out << a[0][i] << '\t'<<a[1][i]<<"\n";
    }*/
    d.printCoordinates(out);
    //for_each(a.begin(),a.end(),[&out](Eigen::Vector<T,2>& n) { out << n(0) << '\t'<<n(1)<<"\n"; });
    out<<"\n\n";
    if(verboseLevel>=2) {
        /*out << "Правая часть\n" << d.getRightPart()[0].format(CleanFmt) << '\n' << d.getRightPart()[1].format(CleanFmt)
            << "\n\n";

        out << "Вектор решения\n" << solut[0].format(CleanFmt) << '\n' << solut[1].format(CleanFmt) << "\n\n";*/
        d.printRightPart(out,CleanFmt);
        d.printSolution(out,CleanFmt);
    }
    out<<"Коеффициенты по номеру уравнения\n";
    /*auto newas=d.getMatrixx()*solut-d.getRightPart();
    std::cout<<"\n\n\n"<<newas<<"\n Norm="<<newas.norm()<<"\n\n";*/


    for (int i = 0; i < N; ++i) {
        out<<"A"<<i+1<<"x = "<<solut[0].coeffRef(2*i)<<", B"<<i+1<<"x = "<<solut[1].coeffRef(2*i)<<"\n";
        out<<"A"<<i+1<<"y = "<<solut[0].coeffRef(2*i+1)<<", B"<<i+1<<"y = "<<solut[1].coeffRef(2*i+1)<<"\n";
    }

    //generateMeshes(N,id,d,out,basicString);
    MeshProcessor<T> mesh;
    d.getFullFunction();
    mesh.generateMeshes(d.getIfunction());

    mesh.printDec(out);
    mesh.plotSpherical(basicString+"/out"+std::to_string(N)+"_"+std::to_string(id)+"_.png");


    out.close();
}


template<class T>
vector<std::array<std::vector<T>, 2>> parseConf(string &filename) {
    std::ifstream in(filename);
    char c=in.get();
    assert(c=='C');
    int Nconf;
    in>>Nconf;
    std::vector<std::array<std::vector<T>,2>>avec(Nconf);
    std::vector<int>Nvec(Nconf);
    for (int j = 0; j < Nconf; ++j) {

        int N;
        in>>N;
        Nvec[j]=N;
        avec[j]=std::array<std::vector<T>,2>({std::vector<T>(N,0),std::vector<T>(N,0)});

        if(in.peek()=='l')
        {
            in.get();
            T lim[4];
            in>>lim[0]>>lim[1]>>lim[2]>>lim[3];
            T step[2]={(lim[1]-lim[0])/(N-1),(lim[3]-lim[2])/(N-1)};
            for (int i = 0; i < N; ++i) {

                avec[j][0][i]=lim[0];
                avec[j][1][i]=lim[2];
                lim[0]+=step[0];
                lim[2]+=step[1];
            }
        }
        else if(in.peek()=='g')
        {
            int N1=0;
            in.get();
            in>>N1;
            int N2=N/N1;
            T lim[4];
            in>>lim[0]>>lim[1]>>lim[2]>>lim[3];
            T start[2]={lim[0],lim[2]};
            T step[2]={(lim[1]-lim[0])/(N1-1),(lim[3]-lim[2])/(N2-1)};
            for (int i = 0; i < N1; ++i) {
                for (int k = 0; k < N2; ++k) {
                    avec[j][0][i*N2+k]=lim[0];
                    avec[j][1][i*N2+k]=lim[2];
                    lim[2]+=step[1];
                }
                lim[2]=start[1];
                lim[0]+=step[0];
            }
        }
        else{
            for (int i = 0; i < N; ++i) {
                in >> avec[j][0][i];
            }
            c = in.get();
            if (c != '\n') {
                if (c == '\r' && in.peek() != '\n') {
                    errno = -1;
                    std::cout << "Errno=" << errno;
                }
            }

            for (int i = 0; i < N; ++i) {
                in >> avec[j][1][i];
            }
        }
    }
    return avec;
}



#endif //MAGISTER1_LIB_H
