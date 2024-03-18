#include <iostream>
#include "Dipoles.h"
#include <vector>
#include <algorithm>
#include <numeric>
#include <ostream>
#include <fstream>
#include <iomanip>
#include <filesystem>
using Eigen::Vector2d;




const std::size_t maxPrecision = std::numeric_limits<double>::digits;

void printToFile(int N, vector<Eigen::Vector<long double, 2>> &a, dipoles::Dipoles<long double> &d, string& basicString,int id);


std::vector<std::vector<Eigen::Vector<long double,2>>> parseConf(std::string &filename)
{
    std::ifstream in(filename);
    char c=in.get();
    assert(c=='C');
    int Nconf;
    in>>Nconf;
    std::vector<std::vector<Eigen::Vector<long double,2>>>avec(Nconf);
    std::vector<int>Nvec(Nconf);
    for (int j = 0; j < Nconf; ++j) {
        int N;
        in>>N;
        Nvec[j]=N;
        avec[j]=std::vector<Eigen::Vector<long double,2>>(N,{0,0});
        for (int i = 0; i < N; ++i) {
            //in>>a[i][0];
            in>>avec[j][i][0];
        }
        c=in.get();
        if(c!='\n')
        {
            if(c=='\r'&&in.peek()!='\n') {
                errno = -1;
                std::cout << "Errno=" << errno;
            }
        }

        for (int i = 0; i < N; ++i) {
            //in>>a[i][1];
            in>>avec[j][i][1];
        }
    }
    return avec;
}

using  namespace dipoles;
const double  l=1E-7;
int main(int argc, char* argv[]) {
    std::ios_base::sync_with_stdio(false);
    std::string filename="config.txt";
    if(argc==2)
    {
        filename=argv[1];
    }
    auto avec= parseConf(filename);
    std::string dirname=filename.erase(filename.find('.'));
    std::filesystem::create_directory(dirname);
    for (int i = 0; i < avec.size(); ++i) {
        std::vector<Eigen::Vector<long double,2>>a=avec[i];
        int N=a.size();
        Dipoles<long double> d(N,a);
        d.solve();
        auto solut=d.getSolution();
        //Aox-0,Aoy-1
        //Box-2,Boy-3
        std::array<std::vector<long double>,4> coefss={std::vector<long double>(N,0),std::vector<long double>(N,0),std::vector<long double>(N,0),std::vector<long double>(N,0)};

        for (int i = 0; i < N; ++i) {
            coefss[0][i]=solut.coeffRef(2*i);//Ax
            coefss[1][i]=solut.coeffRef(2*N+2*i);//Bx
            coefss[2][i]=solut.coeffRef(2*i+1);//Ay
            coefss[3][i]=solut.coeffRef(2*N+2*i+1);//By
            std::cout<<"A"<<i<<"x = "<<solut.coeffRef(2*i)<<", B"<<i<<"x = "<<solut.coeffRef(2*N+2*i)<<"\n";
            std::cout<<"A"<<i<<"y = "<<solut.coeffRef(2*i+1)<<", B"<<i<<"y = "<<solut.coeffRef(2*N+2*i+1)<<"\n";
        }
        std::cout<<solut;
        //std::cout << "Hello, World!" << std::endl;
        printToFile(N, a, d, dirname,i);
    }
    /*for (int j = 0; j < 1; ++j) {
        int N=2;
        //std::cin>>N;


        std::vector<Eigen::Vector<long double,2>> a(N,{0,0});

      double step=l/((N)-1);
        double accumul=0;for(auto & elem:a)
        {
            elem(0)=accumul;
            //elem(1)=accumul;
            accumul+=step;
            //elem(1)=accumul-step;
        }

        Dipoles<long double> d(N,a);
        d.solve();
        auto solut=d.getSolution();
        //Aox-0,Aoy-1
        //Box-2,Boy-3
        std::array<std::vector<long double>,4> coefss={std::vector<long double>(N,0),std::vector<long double>(N,0),std::vector<long double>(N,0),std::vector<long double>(N,0)};
        for (int i = 0; i < N; ++i) {
            coefss[0][i]=solut.coeffRef(2*i);//Ax
            coefss[1][i]=solut.coeffRef(2*N+2*i);//Bx
            coefss[2][i]=solut.coeffRef(2*i+1);//Ay
            coefss[3][i]=solut.coeffRef(2*N+2*i+1);//By
            std::cout<<"A"<<i<<"x = "<<solut.coeffRef(2*i)<<", B"<<i<<"x = "<<solut.coeffRef(2*N+2*i)<<"\n";
            std::cout<<"A"<<i<<"y = "<<solut.coeffRef(2*i+1)<<", B"<<i<<"y = "<<solut.coeffRef(2*N+2*i+1)<<"\n";
        }
        std::cout<<solut;
        //std::cout << "Hello, World!" << std::endl;
        printToFile(N,a,d);




    }*/

    return 0;
}

void
printToFile(int N, vector<Eigen::Vector<long double, 2>> &a, dipoles::Dipoles<long double> &d, string &basicString,int id) {
    auto solut=d.getSolution();
    std::ofstream out(basicString+"/out"+std::to_string(N)+"_"+std::to_string(/*a[N-1][0]/l*/id)+"_.txt");
    Eigen::IOFormat CleanFmt(Eigen::StreamPrecision, 0, "\t", "\n", "", "");
    out<<"Матрица\n"<<d.getMatrixx().format(CleanFmt)<<"\n\n";
    out<<"Координаты диполей\n";
    for_each(a.begin(),a.end(),[&out](Eigen::Vector<long double,2>& n) { out << n(0) << '\t'<<n(1)<<"\n"; });
    out<<"\n\n";
    out<<"Правая часть\n"<<d.getRightPart().format(CleanFmt)<<"\n\n";

    out<<"Вектор решения\n"<<solut.format(CleanFmt)<<"\n\n";
    out<<"Коеффициенты по номеру уравнения\n";
    auto newas=d.getMatrixx()*solut-d.getRightPart();
    std::cout<<"\n\n\n"<<newas<<"\n Norm="<<newas.norm()<<"\n\n";


    for (int i = 0; i < N; ++i) {
        out<<"A"<<i+1<<"x = "<<solut.coeffRef(2*i)<<", B"<<i+1<<"x = "<<solut.coeffRef(2*N+2*i)<<"\n";
        out<<"A"<<i+1<<"y = "<<solut.coeffRef(2*i+1)<<", B"<<i+1<<"y = "<<solut.coeffRef(2*N+2*i+1)<<"\n";
    }

    /*for (int i = 0; i < N; ++i) {
            out<<solut.coeffRef(2*i)<<"\t"<<solut.coeffRef(2*N+2*i)<<"\t";
            out<<solut.coeffRef(2*i+1)<<"\t"<<solut.coeffRef(2*N+2*i+1)<<"\n";
        }*/
    out.close();
}
