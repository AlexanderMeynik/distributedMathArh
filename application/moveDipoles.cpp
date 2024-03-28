
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
double eps=0.01;//todo метод печати матрицы в диполи
//todo сравнивать штуки
int main(int argc, char* argv[]) {
    std::ios_base::sync_with_stdio(false);
    std::string dirname="movemovemove/";
    std::filesystem::create_directory(dirname);
    std::array<std::vector<double>,2>coordinates;
    coordinates[0]={0.0,5*l};
    coordinates[1]={0.0,5*l};
    double limit=0.01*l;
    int i=0;
    int N=coordinates[0].size();
    std::ofstream out(dirname+"results.txt");

    Dipoles< double> d(N,coordinates);
    d.solve_();

    MeshProcessor<double> mmesh;
    d.getFullFunction();
    mmesh.generateMeshes(d.getIfunction());
    auto prevMesh=mmesh.getMeshdec();
    prevMesh[2][0][0]=10000000000;
    double multip=1;
    while(i<30)//(coordinates[0]-coordinates[1]).norm()>limit)
    {
        std::ofstream  fout(dirname+"out"+std::to_string(N)+"_"+std::to_string(i)+".txt");
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
        auto res=getMeshDiffNorm(t1,t2);
        out<<res<<"\n\n\n\n\n";
        fout<<coordinates[0][0]<<"\t"<<coordinates[1][0]<<"\n";
        fout<<coordinates[0][1]<<"\t"<<coordinates[1][1]<<"\n";
        fout<<i<<"\t"<<res<<"\n";
        mmesh.plotSpherical(dirname+"out"+std::to_string(N)+"_"+std::to_string(i)+".png");
        ++i;
        fout<<'\n'<<solut2[0]<<"\n"<<solut2[1]<<"\n\n";


        mmesh.printDec(fout);

        coordinates[0][1]=coordinates[0][1]+multip*l;
        coordinates[1][1]=coordinates[1][1]+multip*l;

        prevMesh=mmesh.getMeshdec();
        if(i%4)
        {
            multip*=2;
        }
        if(i==29)
        {
            show();
        }

        fout.close();

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