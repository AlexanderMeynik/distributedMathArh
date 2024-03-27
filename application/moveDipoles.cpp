
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
#include "lib.h"
const std::size_t maxPrecision = std::numeric_limits<double>::digits;


using  namespace dipoles;
const double  l=1E-7;
double eps=0.01;
int main(int argc, char* argv[]) {
    std::ios_base::sync_with_stdio(false);
    std::string dirname="movemovemove";
    std::filesystem::create_directory(dirname);
    std::vector<Eigen::Vector<double,2>>coordinates={{0,0},{5*l,5*l}};
    double limit=0.01*l;
    int i=0;
    int N=coordinates.size();
    while(i<20)//(coordinates[0]-coordinates[1]).norm()>limit)
    {

        Dipoles< double> d(N,coordinates);
        d.solve_();
        auto solut2=d.getSolution_();
        printToFile<double>(N,coordinates,d,dirname,i,2);//надо сравнивать mesh
        //когда разницы между ними почти не будет оставновка
        //функция для подсчёта нормы от разницы 2 мешей принимет vector<vector<T>>&
        ++i;

        coordinates[1][0]=coordinates[1][0]-l/5;
        coordinates[1][1]=coordinates[1][1]-l/5;


    }
    /*for (int i = 0; i < avec.size(); ++i) {
        std::vector<Eigen::Vector<double,2>>a=avec[i];
        int N=a.size();
        Dipoles< double> d(N,a);
        d.solve_();
        auto solut2=d.getSolution_();
        printToFile<double>(N, a, d, dirname,i,verbose);
    }*/

    return 0;
}