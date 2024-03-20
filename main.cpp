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

template<typename T>
struct scientificNumberType
{
    explicit scientificNumberType(T number, int decimalPlaces) : number(number), decimalPlaces(decimalPlaces) {}

    T number;
    int decimalPlaces;
};

template<typename T>
scientificNumberType<T> scientificNumber(T t, int decimalPlaces)
{
    return scientificNumberType<T>(t, decimalPlaces);
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const scientificNumberType<T>& n)
{
    double numberDouble = n.number;
    char c=(n.number>0)?'+':'-';

    int eToThe = 0;
    for(; numberDouble > 9; ++eToThe)
    {
        numberDouble /= 10;
    }

    // memorize old state
    std::ios oldState(nullptr);
    oldState.copyfmt(os);

    os << std::fixed << std::setprecision(n.decimalPlaces) << numberDouble << "e"<<c << eToThe;

    // restore state
    os.copyfmt(oldState);

    return os;
}
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

const std::size_t maxPrecision = std::numeric_limits<double>::digits;
template<class T>
void
printToFile(int N, vector<Eigen::Vector<T, 2>> &a, dipoles::Dipoles<T> &d, string &basicString,int id,int verboseLevel);



template<class T>
std::vector<std::vector<Eigen::Vector<T,2>>> parseConf(std::string &filename);

using  namespace dipoles;
const double  l=1E-7;
int main(int argc, char* argv[]) {
    std::ios_base::sync_with_stdio(false);
    int verbose=2;
    std::string filename="config.txt";
    if(argc==2)
    {
        filename=argv[1];
    }
    if(argc==3)
    {
        char* end;
        verbose=std::strtol(argv[2],&end,10);
    }
    auto avec= parseConf<double>(filename);
    std::string dirname=filename.erase(filename.find('.'));
    std::filesystem::create_directory(dirname);
    for (int i = 0; i < avec.size(); ++i) {
        std::vector<Eigen::Vector<double,2>>a=avec[i];
        int N=a.size();
        Dipoles< double> d(N,a);
        d.solve_();
        auto solut2=d.getSolution_();
        printToFile<double>(N, a, d, dirname,i,verbose);
    }

    return 0;
}



template<class T>
void printToFile(int N, vector<Eigen::Vector<T, 2>> &a, Dipoles <T> &d, string &basicString, int id, int verboseLevel) {
    auto solut=d.getSolution_();
    std::ofstream out(basicString+"/out"+std::to_string(N)+"_"+std::to_string(/*a[N-1][0]/l*/id)+"_.txt");
    Eigen::IOFormat CleanFmt(Eigen::StreamPrecision, 0, "\t", "\n", "", "");
    if(verboseLevel>=3) {
        out << "Матрица\n" << d.getMatrixx().format(CleanFmt) << "\n\n";
    }
    out<<"Координаты диполей\n";
    for_each(a.begin(),a.end(),[&out](Eigen::Vector<T,2>& n) { out << n(0) << '\t'<<n(1)<<"\n"; });
    out<<"\n\n";
    if(verboseLevel>=2) {
        out << "Правая часть\n" << d.getRightPart()[0].format(CleanFmt) << '\n' << d.getRightPart()[1].format(CleanFmt)
            << "\n\n";

        out << "Вектор решения\n" << solut[0].format(CleanFmt) << '\n' << solut[1].format(CleanFmt) << "\n\n";
    }
    out<<"Коеффициенты по номеру уравнения\n";
    /*auto newas=d.getMatrixx()*solut-d.getRightPart();
    std::cout<<"\n\n\n"<<newas<<"\n Norm="<<newas.norm()<<"\n\n";*/


    for (int i = 0; i < N; ++i) {
        out<<"A"<<i+1<<"x = "<<solut[0].coeffRef(2*i)<<", B"<<i+1<<"x = "<<solut[1].coeffRef(2*i)<<"\n";
        out<<"A"<<i+1<<"y = "<<solut[0].coeffRef(2*i+1)<<", B"<<i+1<<"y = "<<solut[1].coeffRef(2*i+1)<<"\n";
    }
    std::array<std::vector<T>,2> coord={std::vector<T>(N),std::vector<T>(N)};
    std::array<std::vector<T>,2> coefs={std::vector<T>(2*N),std::vector<T>(2*N)};

    for (int i = 0; i < N; ++i) {
        coord[0][i]=a[i][0];
        coord[1][i]=a[i][1];
    }
    for (int i = 0; i < 2*N; ++i) {
        coefs[0][i]=solut[0][i];
        coefs[1][i]=solut[1][i];
        //coord[1][i]=a[i][1];
    }
    const T omega=pow(10,15);//todo константа вынести
    const T rr=2*M_PI/omega;//todo константа вынести
    //todo вынести высление функции в отдельнкю библиотеку
    std::function<T(T,T,T)>ff2=getFullFunction<T>(coefs,coord);
    T step=M_PI/12;//todo константа вынести
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

    using namespace matplot;
    auto [phi, theta] = meshgrid(linspace(0, M_PI*2, 25), linspace(0, M_PI_2, 7));
    auto Z = transform(phi, theta, [&ff2,&rr](double x, double y) {
        return integrateFunctionBy1Val<double>(ff2,y,x,0,rr);
    });
    surf(phi, theta, Z);
    view(213,22);
    //show();
    save(basicString+"/out"+std::to_string(N)+"_"+std::to_string(/*a[N-1][0]/l*/id)+"_.png");


    /*for (int i = 0; i < N; ++i) {
            out<<solut.coeffRef(2*i)<<"\t"<<solut.coeffRef(2*N+2*i)<<"\t";
            out<<solut.coeffRef(2*i+1)<<"\t"<<solut.coeffRef(2*N+2*i+1)<<"\n";
        }*/
    out.close();
}

template<class T>
vector<std::vector<Eigen::Vector<T, 2>>> parseConf(string &filename) {
    std::ifstream in(filename);
    char c=in.get();
    assert(c=='C');
    int Nconf;
    in>>Nconf;
    std::vector<std::vector<Eigen::Vector<T,2>>>avec(Nconf);
    std::vector<int>Nvec(Nconf);
    for (int j = 0; j < Nconf; ++j) {

        int N;
        in>>N;
        Nvec[j]=N;
        avec[j]=std::vector<Eigen::Vector<T,2>>(N,{0,0});

        if(in.peek()=='l')
        {
            in.get();
            T lim[4];
            in>>lim[0]>>lim[1]>>lim[2]>>lim[3];
            T step[2]={(lim[1]-lim[0])/(N-1),(lim[3]-lim[2])/(N-1)};
            for (int i = 0; i < N; ++i) {

                avec[j][i][0]=lim[0];
                avec[j][i][1]=lim[2];
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
                    avec[j][i*N2+k][0]=lim[0];
                    avec[j][i*N2+k][1]=lim[2];
                    lim[2]+=step[1];
                }
                lim[2]=start[1];
                lim[0]+=step[0];
            }
        }
        else{
            for (int i = 0; i < N; ++i) {
                //in>>a[i][0];
                in >> avec[j][i][0];
            }
            c = in.get();
            if (c != '\n') {
                if (c == '\r' && in.peek() != '\n') {
                    errno = -1;
                    std::cout << "Errno=" << errno;
                }
            }

            for (int i = 0; i < N; ++i) {
                //in>>a[i][1];
                in >> avec[j][i][1];
            }
        }
    }
    return avec;
}


