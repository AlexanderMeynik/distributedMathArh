
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
    std::array<std::vector<double>,2>coordinates;
    coordinates[0]={0.0,5*l};
    coordinates[1]={0.0,5*l};
    double limit=0.01*l;
    int i=0;
    int N=coordinates[0].size();
    std::ofstream out("movemovemove/results.txt");

    Dipoles< double> d(N,coordinates);
    d.solve_();

    MeshProcessor<double> mmesh;
    d.getFullFunction();
    mmesh.generateMeshes(d.getIfunction());
    auto prevMesh=mmesh.getMeshdec();
    while(i<20)//(coordinates[0]-coordinates[1]).norm()>limit)
    {

        d.setNewCoordinates(coordinates);
        d.solve_();
        auto solut2=d.getSolution_();

        d.getFullFunction();
        mmesh.generateMeshes(d.getIfunction());

        //printToFile<double>(N, coordinates, d, dirname,i,2);
        //когда разницы между ними почти не будет оставновка
        //функция для подсчёта нормы от разницы 2 мешей принимет vector<vector<T>>&
        mmesh.printDec(out);
        std::vector<std::vector<double>> t1=prevMesh[2];
        std::vector<std::vector<double>> t2=mmesh.getMeshdec()[2];
        out<<getMeshDiffNorm(t1,t2)<<"\n\n\n\n\n";
        ++i;

        coordinates[1][0]=coordinates[1][0]-l/5;
        coordinates[1][1]=coordinates[1][1]-l/5;


    }
    out.close();
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