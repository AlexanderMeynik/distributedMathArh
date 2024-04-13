#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include <eigen3/Eigen/Dense>
#include <random>
#include <omp.h>
/*
int main(int argc, char* argv[]) {
 double alpha=1;
    int NN=10;
    std::random_device rd;  // Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> dis(0, 1);
    std::vector<Eigen::Vector<double,2>> array(NN);

}*/

#include <iostream>
#include <random>
#include <cmath>
#include <fstream>
#include "lib.h"
#include "Dipoles.h"
#include "MeshProcessor.h"
// Функция для генерации нормально распределенного случайного числа средствами преобразования Бокса-Мюллера
std::string getString(const std::string &dirname,std::string &&name, int i, std::string &&end) {
    return dirname + name+"_i"+ to_string(i) +"."+ end;
}

using dipoles::Dipoles;

int main(int argc, char* argv[]) {
    int N=5;
    int Nsym=1000;
    double aRange=1e-6;
    bool print=false;
    if(argc>=3)
    {
        char* end;
        N=std::strtol(argv[1],&end,10);
        Nsym=std::strtol(argv[2],&end,10);
    }
    if(argc>=4)
    {
        char* end;
        aRange=std::strtod(argv[3],&end);
    }

    if(argc==5)
    {
        char* end;
        print=strtol(argv[4],&end,10);
    }
    //todo добавить класс test runner
    //его задачи - инциализирвоать все компоненты
    //данный класс внутри будет хранить структуру с названием теста и его параметрами
    //в стуркутре теста можно задать статусы

    std::stringstream ss;
    ss<<aRange<<".csv";
    std::string aStr = ss.str(); // filename = 1e+16.csv
    aStr.erase(std::remove(aStr.begin(), aStr.end(), '+'), aStr.end());// removing the '+' sign
    std::replace(aStr.begin(), aStr.end(), '-','_');//, aStr.end());// removing the '-' sign
    //std::cout<<aStr; // 1e16.csv
    std::string dirname="results/experiment_N="+std::to_string(N)+
            "_Nsym="+std::to_string(Nsym)+"_a="+aStr+"_print="+std::to_string(print)+"/";
    if(!std::filesystem::exists(dirname)) {
        std::filesystem::create_directory(dirname);
    }

    CoordGenerator<double> genr(0,aRange);
    std::vector<array<vector<double>, 2>> coordinates(Nsym);
    for (int i = 0; i < Nsym; ++i) {
        coordinates[i]=genr.generateCoordinates(N);
    }
    Dipoles<double>dipoles1(N,coordinates[0]);

    MeshProcessor<double> mesh=MeshProcessor<double>();
    auto result = mesh.getMeshGliff();
    Eigen::IOFormat CleanFmt(Eigen::StreamPrecision, 0, "\t", "\n", "", "");
    double stime = omp_get_wtime();
    if(!print) {
        #pragma omp parallel for private(dipoles1,mesh),default(shared)
        for (int i = 0; i < Nsym; ++i) {
            dipoles1.setNewCoordinates(coordinates[i]);
            dipoles1.solve_();
            dipoles1.getFullFunction();
            mesh.generateNoInt(dipoles1.getI2function());
            auto mesht = mesh.getMeshdec()[2];
            #pragma omp critical
            {
                addMesh(result, mesht);
            }
        }
    }
    else
    {
        for (int i = 0; i < Nsym; ++i) {
            std::ofstream out1(getString(dirname,"sim", i, "txt"));
            dipoles1.setNewCoordinates(coordinates[i]);
            dipoles1.solve_();
            dipoles1.getFullFunction();
            mesh.generateNoInt(dipoles1.getI2function());
            auto mesht = mesh.getMeshdec()[2];
            {
                addMesh(result, mesht);
            }
            out1 << "Итерация симуляции i = " << i << "\n\n";
            dipoles1.printCoordinates(out1);
            out1 << "\n";

            dipoles1.printSolutionFormat1(out1);
            out1 << "\n";


            out1 << "\n";
            mesh.printDec(out1);

            out1.close();
            mesh.plotSpherical(getString(dirname,"sim", i, "png"));
            //std::string name = dirname + "coord_i=" + std::to_string(i) + ".png";
            dipoles1.plotCoordinates(getString(dirname,"coord", i, "png"));

        }
    }

    double resulting_time = omp_get_wtime()-stime;
    std::cout<<"Execution_time = "<<resulting_time<<"\tN = "<<N<<"\n";

    for (int i = 0; i < result.size(); ++i) {
        for (int j = 0; j < result[0].size(); ++j) {
            result[i][j]/=Nsym;
        }
    }
    std::ofstream out1(dirname+"avg.txt");
    out1<<"Значение  целевой функции усреднённой по "<<Nsym<<" симуляциям "
    <<"для конфигураций, состоящих из "<< N<<" диполей\n";
    mesh.setMesh3(result);
    mesh.printDec(out1);
    mesh.plotSpherical(dirname+"avg.png");
    out1.close();

    return 0;
}


