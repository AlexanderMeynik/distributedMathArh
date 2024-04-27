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



    std::array<std::vector<T>,2> triangularGrid(T l,T r,T a=0,T b=0,bool center=false)
    {

        T k=c2/c1;

        T D=sqrt(k*k*r*r/(k*k+1));//здесь мы находим x координаты для точек пересечения
        T x_t[2]={-D,D};
        T y_t[2];
        for (int i = 0; i < 2; ++i) {
            y_t[i]=-(x_t[i])/k;//подставляем их в исходный код
        }
        T c_t[2];

        for (int i = 0; i < 2; ++i) {
            c_t[i]=y_t[i]-x_t[i]*k;
        }
        int l_t[2];

        for (int i = 0; i < 2; ++i) {
            l_t[i]=-(c_t[i])/(k*l);
        }


        //todo 2 круг симметричная фигура-> можно подсчитать результаты в 1 четверти
        //а что будет если поместить треугольники так, чробы центр центрального был в центре круга
        if(center)
        {
            l_t[0]--;
            //l_t[1];
        }
        int funccount=std::abs(l_t[1]-l_t[0])+1;
        std::vector<T> x1= myLinspace(l_t[0]*l,l_t[1]*l,funccount);
        std::vector<T> y1(funccount,0);

        std::vector<T> x(funccount*funccount,0);
        std::vector<T> y(funccount*funccount,0);

        for (int i = 0; i < funccount; ++i) {
            for (int j = 0; j < funccount; ++j) {

                x[j+i*funccount]=x1[i]/2+x1[j]/2+(c1/c2)*(y1[i]+y1[j])/2;
                y[j+i*funccount]=(-c2/c1)*x[j+i*funccount]+(c2/c1)*x1[i]-y1[i];

                x[j+i*funccount]+=a;
                y[j+i*funccount]+=b;

            }
        }

        if(center)
        {
            for (int i = 0; i < funccount; ++i) {
                for (int j = 0; j < funccount; ++j) {

                    x[j+i*funccount]+=l/2.0;
                    y[j+i*funccount]+=l*c2/2.0;

                }
            }
        }




        std::vector<T> x_filtered, y_filtered;

        for (int i = 0; i < funccount*funccount; ++i) {
            if (std::sqrt(pow(x[i]-a,2) + pow(y[i]-b,2)) <= r) {
                x_filtered.push_back(x[i]);
                y_filtered.push_back(y[i]);
            }
        }
        //std::cout<<x_filtered.size()<<'\t'<<x.size()<<'\t'<<x_filtered.size()/(1.0*x.size())<<"\n";
        return {x_filtered, y_filtered};
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
    long double c1= 1/2.0;
   long double c2= sqrt(3)/2.0;
};

enum class state_t {
    openmp_new,
    new_,
    openmp_old,
    old,
    print_
};


const static std::map<state_t, std::string> stateToString = {
        { state_t::openmp_new,"openmp_new" },
        { state_t::new_,"new"},
        { state_t::openmp_old,"openmp_old"},
        { state_t::old,"old"},
        { state_t::print_,"print"},
};
const static std::map<std::string,state_t >  stringToState = {
        { "openmp_new",state_t::openmp_new },
        { "new",state_t::new_},
        { "openmp_old",state_t::openmp_old},
        { "old",state_t::old},
        { "print",state_t::print_},
};


/*template<class T>
void
printToFile(int N, array<std::vector<T>, 2> &a, dipoles::Dipoles<T> &d, string &basicString, int id, int verboseLevel) {
    auto solut=d.getSolution_();
    std::ofstream out(basicString+"/out"+std::to_string(N)+"_"+std::to_string(id)+"_.txt");
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
    d.getFullFunction(a,d.getSolution_());
    mesh.generateNoInt(d.getI2function());

    mesh.printDec(out);
    mesh.plotSpherical(basicString+"/out"+std::to_string(N)+"_"+std::to_string(id)+"_.png");


    out.close();
}*/


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
