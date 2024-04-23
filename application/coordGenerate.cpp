#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include <eigen3/Eigen/Dense>
#include <random>
#include <map>
#include <omp.h>
#include "OpenmpParallelClock.h"
/*
int main(int argc, char* argv[]) {
 double alpha=1;
    int NN=10;
    std::random_device rd;  // Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> dis(0, 1);
    std::vector<Eigen::Vector<double,2>> array(NN);

}*/

enum class state_t {
    openmp_new,
    new_,
    openmp_old,
    old,
    print_
};

typedef std::map<state_t, std::string> statemap_t;

statemap_t state1 = {
        { state_t::openmp_new,"openmp_new" },
        { state_t::new_,"new"},
        { state_t::openmp_old,"openmp_old"},
        { state_t::old,"old"},
        { state_t::print_,"print"},
};
typedef std::map<std::string,state_t > statemap_r;
statemap_r state2 = {
        { "openmp_new",state_t::openmp_new },
        { "new",state_t::new_},
        { "openmp_old",state_t::openmp_old},
        { "old",state_t::old},
        { "print",state_t::print_},
};


#include <iostream>
#include <random>
#include <cmath>
#include <fstream>
#include "lib.h"
#include "Dipoles.h"
#include "MeshProcessor.h"
// Функция для генерации нормально распределенного случайного числа средствами преобразования Бокса-Мюллера
std::string getString(const std::string &dirname,std::string &&name, int i, std::string &&end) {
    return dirname + name+"_i"+ std::to_string(i) +"."+ end;
}

using dipoles::Dipoles;

int main(int argc, char* argv[]) {
    int N=5;
    int Nsym=1000;
    double aRange=1e-6;
    int print=false;
    state_t state=state_t::openmp_new;
    std::string subdirectory="";
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

    if(argc>=5)
    {
        char* end;

        std::string mode=argv[4];

        state=state2.find(mode)->second;


        //print=strtol(argv[4],&end,10);
    }
    if(argc>=6)
    {
        subdirectory=argv[5];
        subdirectory+='/';
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
    std::string dirname="results/"+subdirectory+"experiment_N="+std::to_string(N)+
            "_Nsym="+std::to_string(Nsym)+"_a="+aStr+"_mode="+state1.find(state)->second+"/";
    if(!std::filesystem::exists("results/"))
    {
        std::filesystem::create_directory("results/");
    }

    if(subdirectory.size()&&!std::filesystem::exists("results/"+subdirectory))
    {
        std::filesystem::create_directory("results/"+subdirectory);
    }

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
    std::vector<double>totalTime(omp_get_max_threads(),0);
    std::vector<double>functionTime(omp_get_max_threads(),0);
    std::vector<double>solveTime(omp_get_max_threads(),0);

    double totalTimeS=0;
    double functionTimeS=0;
    double solveTimeS=0;

    double stime = omp_get_wtime();

    switch (state) {
        case state_t::openmp_new: goto run_new_openmp; break;
        case state_t::new_: goto run_new_; break;
        case state_t::openmp_old: goto run_old_openmp; break;
        case state_t::old: goto run_old_;break;
        case state_t::print_: goto print_; break;
    }
    //if(!print) {
    {
        run_new_openmp:
        stime = omp_get_wtime();
        //solveTime= ;
        #pragma omp parallel for private(dipoles1, mesh), default(shared)
        for (int i = 0; i < Nsym; ++i) {
            int tid = omp_get_thread_num();
            double stmep[2] = {omp_get_wtime(), 0};//todo создать библиотеку timeUtils и вынести это туда
            dipoles1.setNewCoordinates(coordinates[i]);
            dipoles1.solve_();
            dipoles1.getFullFunction();
            stmep[1] = omp_get_wtime();
            solveTime[tid] += stmep[1] - stmep[0];


            double stmep2[2] = {omp_get_wtime(), 0};
            //stmep= omp_get_wtime();
            mesh.generateNoInt(dipoles1.getI2function());
            auto mesht = mesh.getMeshdec()[2];
            stmep2[1] = omp_get_wtime();
            functionTime[tid] += stmep2[1] - stmep2[0];


            #pragma omp critical
            {
                addMesh(result, mesht);
                //std::cout<<tid<<"\t"<<functionTime[tid]<<'\t'<<solveTime[tid]<<'\n';
            }
            totalTime[tid] += omp_get_wtime() - stmep[0];
        }

        solveTimeS=std::accumulate(solveTime.begin(), solveTime.end(),0.0)/omp_get_max_threads();
        functionTimeS=std::accumulate(functionTime.begin(), functionTime.end(),0.0)/omp_get_max_threads();
        totalTimeS=std::accumulate(totalTime.begin(), totalTime.end(),0.0)/omp_get_max_threads();
        goto printtimes;
    }

    {
        run_new_:
        stime = omp_get_wtime();
        //solveTime= ;
        for (int i = 0; i < Nsym; ++i) {
            double stmep[2] = {omp_get_wtime(), 0};//todo создать библиотеку timeUtils и вынести это туда
            dipoles1.setNewCoordinates(coordinates[i]);
            dipoles1.solve_();
            dipoles1.getFullFunction();
            stmep[1] = omp_get_wtime();
            solveTimeS += stmep[1] - stmep[0];

            double stmep2[2] = {omp_get_wtime(), 0};
            mesh.generateNoInt(dipoles1.getI2function());
            auto mesht = mesh.getMeshdec()[2];
            stmep2[1] = omp_get_wtime();
            functionTimeS += stmep2[1] - stmep2[0];
            addMesh(result, mesht);
            totalTimeS += omp_get_wtime() - stmep[0];
        }
        goto printtimes;
    }

    {
        run_old_:
        stime = omp_get_wtime();
        //solveTime= ;
        for (int i = 0; i < Nsym; ++i) {
            double stmep[2] = {omp_get_wtime(), 0};//todo создать библиотеку timeUtils и вынести это туда
            dipoles1.setNewCoordinates(coordinates[i]);
            dipoles1.solve_();
            dipoles1.getFullFunction();
            stmep[1] = omp_get_wtime();
            solveTimeS += stmep[1] - stmep[0];

            double stmep2[2] = {omp_get_wtime(), 0};
            mesh.generateMeshes(dipoles1.getIfunction());
            auto mesht = mesh.getMeshdec()[2];
            stmep2[1] = omp_get_wtime();
            functionTimeS += stmep2[1] - stmep2[0];
            addMesh(result, mesht);
            totalTimeS += omp_get_wtime() - stmep[0];
        }
        goto printtimes;
    }



    {
        run_old_openmp:
        stime = omp_get_wtime();
        //solveTime= ;
        #pragma omp parallel for private(dipoles1, mesh), default(shared)
        for (int i = 0; i < Nsym; ++i) {
            int tid = omp_get_thread_num();
            double stmep[2] = {omp_get_wtime(), 0};//todo создать библиотеку timeUtils и вынести это туда
            dipoles1.setNewCoordinates(coordinates[i]);
            dipoles1.solve_();
            dipoles1.getFullFunction();
            stmep[1] = omp_get_wtime();
            solveTime[tid] += stmep[1] - stmep[0];


            double stmep2[2] = {omp_get_wtime(), 0};
            //stmep= omp_get_wtime();
            mesh.generateMeshes(dipoles1.getIfunction());
            auto mesht = mesh.getMeshdec()[2];
            stmep2[1] = omp_get_wtime();
            functionTime[tid] += stmep2[1] - stmep2[0];


            #pragma omp critical
            {
                addMesh(result, mesht);
                //std::cout<<tid<<"\t"<<functionTime[tid]<<'\t'<<solveTime[tid]<<'\n';
            }
            totalTime[tid] += omp_get_wtime() - stmep[0];
        }

        solveTimeS=std::accumulate(solveTime.begin(), solveTime.end(),0.0)/omp_get_max_threads();
        functionTimeS=std::accumulate(functionTime.begin(), functionTime.end(),0.0)/omp_get_max_threads();
        totalTimeS=std::accumulate(totalTime.begin(), totalTime.end(),0.0)/omp_get_max_threads();
        goto printtimes;
    }



    //}
    //else
    //{
        print_:for (int i = 0; i < Nsym; ++i) {
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
            dipoles1.plotCoordinates(getString(dirname,"coord", i, "png"),aRange);

        }
    goto printtimes;
    //}

    printtimes:double resulting_time = omp_get_wtime()-stime;
    //std::cout<<"Execution_time = "<<resulting_time<<"\tN = "<<N<<"\n";

    /*std::vector<double> times={std::accumulate(solveTime.begin(), solveTime.end(),0.0)/omp_get_max_threads(),//,[](double a,double b){return a+b;}),
                               std::accumulate(functionTime.begin(), functionTime.end(),0.0)/omp_get_max_threads(),
                               std::accumulate(totalTime.begin(), totalTime.end(),0.0)/omp_get_max_threads()
                               };*/
    std::cout<<resulting_time<<'\t'<<solveTimeS
    <<'\t'<<functionTimeS<<'\t'<<
                          totalTimeS<<'\t'<<N<<'\t';

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


