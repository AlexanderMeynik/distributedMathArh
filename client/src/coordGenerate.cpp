#include <iostream>
#include <fstream>

#include <random>

#include <omp.h>




#include <filesystem>

#include "computationalLib/math_core/Dipoles.h"
#include "common/Generator.h"
#include "plotUtils/MeshCreator.h"
#include "common/Printers.h"
#include "../include/plotingUtils.h"
#include "common/sharedDeclarations.h"

std::string getString(const std::string &dirname, std::string &&name, int i, std::string &&end) {
    return dirname + name + "_i" + std::to_string(i) + "." + end;
}
using namespace shared;
using dipoles::Dipoles;
using printUtils::printCoordinates2,printUtils::printSolutionFormat1;
int main(int argc, char *argv[]) {
    int N = 5;
    int Nsym = 1000;
    double aRange = 1e-6;
    int print = false;
    state_t state = state_t::openmp_new;
    std::string subdirectory = "";
    if (argc >= 3) {
        char *end;
        N = std::strtol(argv[1], &end, 10);
        Nsym = std::strtol(argv[2], &end, 10);
    }
    if (argc >= 4) {
        char *end;
        aRange = std::strtod(argv[3], &end);
    }

    if (argc >= 5) {
        char *end;

        std::string mode = argv[4];

        state = stringToState.find(mode)->second;


        //print=strtol(argv[4],&end,10);
    }
    if (argc >= 6) {
        subdirectory = argv[5];
        subdirectory += '/';
    }


    std::stringstream ss;
    ss << aRange << ".csv";
    std::string aStr = ss.str(); // filename = 1e+16.csv
    aStr.erase(std::remove(aStr.begin(), aStr.end(), '+'), aStr.end());// removing the '+' sign
    std::replace(aStr.begin(), aStr.end(), '-', '_');//, aStr.end());// removing the '-' sign
    //std::cout<<aStr; // 1e16.csv
    std::string dirname = "results/" + subdirectory + "experiment_N=" + std::to_string(N) +
                          "_Nsym=" + std::to_string(Nsym) + "_a=" + aStr + "_mode=" +
                          stateToStr[static_cast<size_t>(state)] + "/";
    if (!std::filesystem::exists("results/")) {
        std::filesystem::create_directory("results/");
    }

    if (subdirectory.size() && !std::filesystem::exists("results/" + subdirectory)) {
        std::filesystem::create_directory("results/" + subdirectory);
    }

    if (!std::filesystem::exists(dirname)) {
        std::filesystem::create_directory(dirname);
    }

    /*CoordGenerator<double> genr(0, aRange);*/
    std::vector<std::vector<double>> coordinates(Nsym);
    for (int i = 0; i < Nsym; ++i) {
        coordinates[i] = generators::normal<std::vector>(N,0.0,aRange* sqrt(2))/*genr.generateCoordinates(N)*/;
    }
    Dipoles dipoles1(coordinates[0]);
    using meshStorage::MeshCreator;
    MeshCreator mesh = MeshCreator();
    mesh.constructMeshes();
    auto result = mesh.data[2];

    Eigen::IOFormat CleanFmt(Eigen::StreamPrecision, 0, "\t", "\n", "", "");
    std::vector<double> totalTime(omp_get_max_threads(), 0);
    std::vector<double> functionTime(omp_get_max_threads(), 0);
    std::vector<double> solveTime(omp_get_max_threads(), 0);

    double totalTimeS = 0;
    double functionTimeS = 0;
    double solveTimeS = 0;

    double stime = omp_get_wtime();

    switch (state) {
        case state_t::openmp_new:
            goto run_new_openmp;
            break;
        case state_t::new_:
            goto run_new_;
            break;
        case state_t::openmp_old:
            goto run_old_openmp;
            break;
        case state_t::old:
            goto run_old_;
            break;
        case state_t::print_:
            goto print_;
            break;
    }
    //if(!print) {
    {
        run_new_openmp:
        stime = omp_get_wtime();
        //solveTime= ;
#pragma omp parallel for firstprivate(dipoles1, mesh), default(shared)
        for (int i = 0; i < Nsym; ++i) {
            int tid = omp_get_thread_num();
            double stmep[2] = {omp_get_wtime(), 0};
            dipoles1.setNewCoordinates(coordinates[i]);
            auto solution = dipoles1.solve<dipoles::EigenVec>();
            dipoles1.getFullFunction_(coordinates[i], solution);
            stmep[1] = omp_get_wtime();
            solveTime[tid] += stmep[1] - stmep[0];


            double stmep2[2] = {omp_get_wtime(), 0};
            //stmep= omp_get_wtime();
            mesh.applyFunction(dipoles1.getI2function());
            auto mesht = mesh.data[2];
            stmep2[1] = omp_get_wtime();
            functionTime[tid] += stmep2[1] - stmep2[0];


#pragma omp critical
            {
                meshStorage::addMesh(result, mesht);
                //std::cout<<tid<<"\t"<<functionTime[tid]<<'\t'<<solveTime[tid]<<'\n';
            }
            totalTime[tid] += omp_get_wtime() - stmep[0];
        }

        solveTimeS = std::accumulate(solveTime.begin(), solveTime.end(), 0.0) / omp_get_max_threads();
        functionTimeS = std::accumulate(functionTime.begin(), functionTime.end(), 0.0) / omp_get_max_threads();
        totalTimeS = std::accumulate(totalTime.begin(), totalTime.end(), 0.0) / omp_get_max_threads();
        goto printtimes;
    }

    {
        run_new_:
        stime = omp_get_wtime();
        //solveTime= ;
        for (int i = 0; i < Nsym; ++i) {
            double stmep[2] = {omp_get_wtime(), 0};
            dipoles1.setNewCoordinates(coordinates[i]);
            auto solution = dipoles1.solve<dipoles::EigenVec>();
            dipoles1.getFullFunction_(coordinates[i], solution);
            stmep[1] = omp_get_wtime();
            solveTimeS += stmep[1] - stmep[0];

            double stmep2[2] = {omp_get_wtime(), 0};
            mesh.applyFunction(dipoles1.getI2function());
            auto mesht = mesh.data[2];
            stmep2[1] = omp_get_wtime();
            functionTimeS += stmep2[1] - stmep2[0];
            meshStorage::addMesh(result, mesht);
            totalTimeS += omp_get_wtime() - stmep[0];
        }
        goto printtimes;
    }

    {
        run_old_:
        stime = omp_get_wtime();
        //solveTime= ;
        for (int i = 0; i < Nsym; ++i) {
            double stmep[2] = {omp_get_wtime(), 0};
            dipoles1.setNewCoordinates(coordinates[i]);
            auto solution = dipoles1.solve<dipoles::EigenVec>();
            dipoles1.getFullFunction_(coordinates[i], solution);
            stmep[1] = omp_get_wtime();
            solveTimeS += stmep[1] - stmep[0];

            double stmep2[2] = {omp_get_wtime(), 0};
            mesh.applyIntegrate(dipoles1.getIfunction());
            auto mesht = mesh.data[2];
            stmep2[1] = omp_get_wtime();
            functionTimeS += stmep2[1] - stmep2[0];
            meshStorage::addMesh(result, mesht);
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
            double stmep[2] = {omp_get_wtime(), 0};
            dipoles1.setNewCoordinates(coordinates[i]);
            auto solution = dipoles1.solve<dipoles::EigenVec>();
            dipoles1.getFullFunction_(coordinates[i], solution);
            stmep[1] = omp_get_wtime();
            solveTime[tid] += stmep[1] - stmep[0];


            double stmep2[2] = {omp_get_wtime(), 0};
            //stmep= omp_get_wtime();
            mesh.applyIntegrate(dipoles1.getIfunction());
            auto mesht = mesh.data[2];
            stmep2[1] = omp_get_wtime();
            functionTime[tid] += stmep2[1] - stmep2[0];


#pragma omp critical
            {
                meshStorage::addMesh(result, mesht);
                //std::cout<<tid<<"\t"<<functionTime[tid]<<'\t'<<solveTime[tid]<<'\n';
            }
            totalTime[tid] += omp_get_wtime() - stmep[0];
        }

        solveTimeS = std::accumulate(solveTime.begin(), solveTime.end(), 0.0) / omp_get_max_threads();
        functionTimeS = std::accumulate(functionTime.begin(), functionTime.end(), 0.0) / omp_get_max_threads();
        totalTimeS = std::accumulate(totalTime.begin(), totalTime.end(), 0.0) / omp_get_max_threads();
        goto printtimes;
    }



    //}
    //else
    //{
    print_:
    for (int i = 0; i < Nsym; ++i) {
        std::ofstream out1(getString(dirname, "sim", i, "txt"));
        dipoles1.setNewCoordinates(coordinates[i]);
        auto solution = dipoles1.solve<dipoles::EigenVec>();
        //dipoles1.getFullFunction();
        dipoles1.getFullFunction_(coordinates[i], solution);
        mesh.applyFunction(dipoles1.getI2function());
        auto mesht = mesh.data[2];
        {
            meshStorage::addMesh(result, mesht);
        }
        out1 << "Итерация симуляции i = " << i << "\n\n";
        printCoordinates2(out1, coordinates[i]);
        out1 << "\n";

        printSolutionFormat1(out1, solution);
        out1 << "\n";


        out1 << "\n";

        printDec(mesh,out1);

        out1.close();
        mesh.plotAndSave(getString(dirname, "sim", i, "png"),plotFunction);
        plotCoordinates(getString(dirname, "coord", i, "png"), aRange, coordinates[i]);

    }
    goto printtimes;
    //}

    printtimes:
    double resulting_time = omp_get_wtime() - stime;
    //std::cout<<"Execution_time = "<<resulting_time<<"\tN = "<<N<<"\n";

    /*std::vector<double> times={std::accumulate(solveTime.begin(), solveTime.end(),0.0)/omp_get_max_threads(),//,[](double a,double b){return a+b;}),
                               std::accumulate(functionTime.begin(), functionTime.end(),0.0)/omp_get_max_threads(),
                               std::accumulate(totalTime.begin(), totalTime.end(),0.0)/omp_get_max_threads()
                               };*/
    std::cout << resulting_time << '\t' << solveTimeS
              << '\t' << functionTimeS << '\t' <<
              totalTimeS << '\t' << N << '\t';

    /*for (int i = 0; i < result.size(); ++i) {
        for (int j = 0; j < result[0].size(); ++j) {
            result[i][j] /= Nsym;
        }
    }*/
    result/=Nsym;
    std::ofstream out1(dirname + "avg.txt");
    out1 << "Значение  целевой функции усреднённой по " << Nsym << " симуляциям "
         << "для конфигураций, состоящих из " << N << " диполей\n";

    mesh.data[2]=result;
    printDec(mesh,out1);
    mesh.plotAndSave(dirname + "avg.png",plotFunction);
    out1.close();

    return 0;
}


