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
using dipoles::Dipoles;

int main(int argc, char* argv[]) {
    int N=5;
    int Nsym=1000;
    if(argc==3)
    {
        char* end;
        N=std::strtol(argv[1],&end,10);
        Nsym=std::strtol(argv[2],&end,10);
    }
    std::string dirname="experiment_N="+std::to_string(N)+
            "_Nsym="+std::to_string(Nsym)+"/";
    if(!std::filesystem::exists(dirname)) {
        std::filesystem::create_directory(dirname);
    }

    CoordGenerator<double> genr(0,1e-6);
    std::vector<array<vector<double>, 2>> coordinates(Nsym);
    for (int i = 0; i < Nsym; ++i) {
        coordinates[i]=genr.generateCoordinates(N);
    }
    Dipoles<double>dipoles1(N,coordinates[0]);

    MeshProcessor<double> mesh=MeshProcessor<double>();
    auto result = mesh.getMeshGliff();
    Eigen::IOFormat CleanFmt(Eigen::StreamPrecision, 0, "\t", "\n", "", "");
    //#pragma omp parallel for private(dipoles1,mesh),default(shared)
    for (int i = 0; i < Nsym; ++i) {
        std::ofstream out1(dirname+"sim_i="+std::to_string(i)+".txt");
        dipoles1.setNewCoordinates(coordinates[i]);
        dipoles1.solve_();
        dipoles1.getFullFunction();
        mesh.generateNoInt(dipoles1.getI2function());
        auto mesht=mesh.getMeshdec()[2];
        //#pragma omp critical
        {
            addMesh(result, mesht);
        }
        out1<<"Итерация симуляции i = "<<i<<"\n\n";
        dipoles1.printCoordinates(out1);
        out1<<"\n";

        dipoles1.printSolutionFormat1(out1);
        out1<<"\n";


        out1<<"\n";
        mesh.printDec(out1);

        out1.close();
        mesh.plotSpherical(dirname+"sim_i="+std::to_string(i)+".png");
        std::string name=dirname+"coord_i="+std::to_string(i)+".png";
        dipoles1.plotCoordinates(name);

    }

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
