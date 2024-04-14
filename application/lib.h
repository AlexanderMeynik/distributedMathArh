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
#include <random>
using Eigen::Vector2d;
using std::function,std::pair,std::vector,std::array;
using std::string;
template<class T>
std::vector<std::array<std::vector<T>,2>> parseConf(string &filename);

template<class T>
void printToFile(int N, std::array<std::vector<T>,2> &a, dipoles::Dipoles <T> &d, string &basicString, int id, int verboseLevel);

template<class T>
void generateMeshes(int N,int id, dipoles::Dipoles<T> & d,std::ostream&out,std::string &basicString);


template <typename T>
class CoordGenerator
{
public:
    CoordGenerator(T mean,T stddev):mean_(mean),stddev_(sqrt(2)*stddev)
    {
        distribution_=std::normal_distribution<T>(mean_, stddev_);
    }
    std::array<std::vector<T>,2> generateCoordinates(size_t N)
    {
        if(!N)
        {
            return  std::array<std::vector<T>,2>();
        }
        std::array<std::vector<T>,2>res;
        res[0]=std::vector<T>(N,0);
        res[1]=std::vector<T>(N,0);
        std::function<T()> generetor=[&](){return distribution_(rng_);};
        //std::cout<<generetor()<<"\t"<<distribution_(rng_)<<"\n";
        std::generate(res[0].begin(), res[0].end(), generetor);
        std::generate(res[1].begin(), res[1].end(), generetor);
        return res;
    }

    CoordGenerator(CoordGenerator&)=delete;
    CoordGenerator(CoordGenerator&&)=delete;
    CoordGenerator&operator=(CoordGenerator&)=delete;
    CoordGenerator&operator=(CoordGenerator&&)=delete;
private:
    T mean_;
    T stddev_;
    std::random_device rd_;
    std::mt19937 rng_=std::mt19937(rd_());
    std::normal_distribution<T> distribution_;
};

template<class T>
void
printToFile(int N, array<std::vector<T>, 2> &a, dipoles::Dipoles<T> &d, string &basicString, int id, int verboseLevel) {
    auto solut=d.getSolution_();
    std::ofstream out(basicString+"/out"+std::to_string(N)+"_"+std::to_string(/*a[N-1][0]/l*/id)+"_.txt");
    Eigen::IOFormat CleanFmt(Eigen::StreamPrecision, 0, "\t", "\n", "", "");
    if(verboseLevel>=3) {
        d.printMatrix(out,CleanFmt);
    }

    d.printCoordinates(out);
    out<<"\n\n";
    if(verboseLevel>=2) {

        d.printRightPart(out,CleanFmt);
        d.printSolution(out,CleanFmt);
    }
    out<<"Коеффициенты по номеру уравнения\n";



    for (int i = 0; i < N; ++i) {
        out<<"A"<<i+1<<"x = "<<solut[0].coeffRef(2*i)<<", B"<<i+1<<"x = "<<solut[1].coeffRef(2*i)<<"\n";
        out<<"A"<<i+1<<"y = "<<solut[0].coeffRef(2*i+1)<<", B"<<i+1<<"y = "<<solut[1].coeffRef(2*i+1)<<"\n";
    }

    MeshProcessor<T> mesh;
    d.getFullFunction();
    mesh.generateNoInt(d.getI2function());

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
