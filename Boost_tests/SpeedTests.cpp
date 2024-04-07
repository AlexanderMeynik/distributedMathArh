#include "../application/lib.h"
#include <chrono>
#include <boost/test/unit_test.hpp>
#include "../application/MeshProcessor.h"
#include "../application/Dipoles.h"
#include <algorithm>
#include <omp.h>
BOOST_AUTO_TEST_SUITE( speedTest )
namespace utf = boost::unit_test;


BOOST_AUTO_TEST_CASE( speed_of_implementations ,* utf::tolerance(pow(10,-12)))
{


        auto t_prev = std::chrono::high_resolution_clock::now();
        auto t_curr = std::chrono::high_resolution_clock::now();
        double elapsed_time_ms[2] ={0.0,0.0};// std::chrono::duration<double, std::milli>(t_end-t_start).count();
        int N=2;
        int Nsym=100;
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
        dipoles::Dipoles<double>dipoles1(N,coordinates[0]);

        MeshProcessor<double> mesh=MeshProcessor<double>();
        auto result = mesh.getMeshGliff();
        auto resultInt = mesh.getMeshGliff();
        Eigen::IOFormat CleanFmt(Eigen::StreamPrecision, 0, "\t", "\n", "", "");
        for (int i = 0; i < Nsym; ++i) {
            //std::ofstream out1(dirname+"sim_i="+std::to_string(i)+".txt");
            dipoles1.setNewCoordinates(coordinates[i]);
            dipoles1.solve_();
            dipoles1.getFullFunction();
            t_prev = std::chrono::high_resolution_clock::now();
            mesh.generateNoInt(dipoles1.getI2function());
            t_curr = std::chrono::high_resolution_clock::now();
            elapsed_time_ms[0]+=std::chrono::duration<double, std::milli>(t_curr-t_prev).count();
            auto mesht=mesh.getMeshdec()[2];
            {
                addMesh(result, mesht);
            }
            t_prev = std::chrono::high_resolution_clock::now();
            mesh.generateMeshes(dipoles1.getIfunction());
            t_curr = std::chrono::high_resolution_clock::now();
            elapsed_time_ms[1]+=std::chrono::duration<double, std::milli>(t_curr-t_prev).count();
            mesht=mesh.getMeshdec()[2];
            {
                addMesh(resultInt, mesht);
            }
        }

        for (int i = 0; i < result.size(); ++i) {
            for (int j = 0; j < result[0].size(); ++j) {
                result[i][j]/=Nsym;
                resultInt[i][j]/=Nsym;
            }
        }
        std::ofstream out1(dirname+"avg.txt");
        out1<<"Значение  целевой функции усреднённой по "<<Nsym<<" симуляциям "
            <<"для конфигураций, состоящих из "<< N<<" диполей\n";
        out1<<"Время выполнения программы: "<<elapsed_time_ms[0]<<'\n';
        mesh.setMesh3(result);
        mesh.printDec(out1);
        mesh.plotSpherical(dirname+"avg.png");
        out1.close();


        std::ofstream out2(dirname+"avgINT.txt");
        out2<<"Значение  целевой функции усреднённой по "<<Nsym<<" симуляциям "
            <<"для конфигураций, состоящих из "<< N<<" диполей\n";
        out2<<"Время выполнения программы: "<<elapsed_time_ms[1]<<'\n';
        mesh.setMesh3(resultInt);
        mesh.printDec(out2);
        mesh.plotSpherical(dirname+"avgINT.png");

        out2.close();

        std::cout<<elapsed_time_ms[1]/elapsed_time_ms[0]<<"\n";

        auto mesh3=mesh.getMeshdec()[2];
        for (int i = 0; i < resultInt.size(); ++i) {
            std::cout<<"theta = "<<M_PI*i/12.0<<'\n';
            for (int k = 0; k < resultInt[i].size(); ++k) {
                mesh3[i][k]=resultInt[i][k]-result[i][k];
                BOOST_TEST(resultInt[i][k]==result[i][k]);
            }
        }

}



    BOOST_AUTO_TEST_CASE( ompTest ,* utf::tolerance(pow(10,-12)))
    {

        double elapsed_time_ms[3] ={0.0,0.0,0.0};// std::chrono::duration<double, std::milli>(t_end-t_start).count();
        const int N=8;
        const int Nsym=100;
        auto t_prev = std::chrono::high_resolution_clock::now();
        auto t_curr = std::chrono::high_resolution_clock::now();

        std::string dirname="experiment2_N="+std::to_string(N)+
                            "_Nsym="+std::to_string(Nsym)+"/";
        if(!std::filesystem::exists(dirname)) {
            std::filesystem::create_directory(dirname);
        }

        CoordGenerator<double> genr(0,1e-6);
        std::vector<array<vector<double>, 2>> coordinates(Nsym);
        dipoles::Dipoles<double> dipolearr[Nsym];
        for (int i = 0; i < Nsym; ++i) {
            coordinates[i]=genr.generateCoordinates(N);
            dipolearr[i].setNewCoordinates(coordinates[i]);
            dipolearr[i].solve_();
            dipolearr[i].getFullFunction();
        }

        MeshProcessor<double> mesh=MeshProcessor<double>();

        auto resultInt = mesh.getMeshGliff();
        auto result = mesh.getMeshGliff();
        auto resultPar = mesh.getMeshGliff();
        Eigen::IOFormat CleanFmt(Eigen::StreamPrecision, 0, "\t", "\n", "", "");

        double ftime = omp_get_wtime();
        for (int i = 0; i < Nsym; ++i) {
            //std::ofstream out1(dirname+"sim_i="+std::to_string(i)+".txt");

            mesh.generateMeshes(dipolearr[i].getIfunction());

            auto mesht=mesh.getMeshdec()[2];
            {
                addMesh(resultInt, mesht);
            }

        }
        elapsed_time_ms[0]=omp_get_wtime()-ftime;

        ftime = omp_get_wtime();
        for (int i = 0; i < Nsym; ++i) {
            mesh.generateNoInt(dipolearr[i].getI2function());

            auto mesht=mesh.getMeshdec()[2];
            {
                addMesh(result, mesht);
            }

        }
        //t_curr = std::chrono::high_resolution_clock::now();
        elapsed_time_ms[1]=omp_get_wtime()-ftime;
        ftime = omp_get_wtime();
        #pragma omp parallel for private(mesh),shared(dipolearr,resultPar,Nsym,t_prev,t_curr,elapsed_time_ms),default(none)
        for (int i = 0; i < Nsym; ++i) {
            mesh.generateNoInt(dipolearr[i].getI2function());
            #pragma omp critical
            {
                auto mesht=mesh.getMeshdec()[2];
                addMesh(resultPar, mesht);
            }
        }
        t_curr = std::chrono::high_resolution_clock::now();
        elapsed_time_ms[2]=omp_get_wtime()-ftime;



        for (int i = 0; i < result.size(); ++i) {
            for (int j = 0; j < result[0].size(); ++j) {
                result[i][j]/=Nsym;
                resultInt[i][j]/=Nsym;
            }
        }
        std::ofstream out1(dirname+"avg.txt");
        out1<<"Значение  целевой функции усреднённой по "<<Nsym<<" симуляциям "
            <<"для конфигураций, состоящих из "<< N<<" диполей\n";
        out1<<"Время выполнения программы: "<<elapsed_time_ms[1]<<'\n';
        mesh.setMesh3(result);
        mesh.printDec(out1);
        mesh.plotSpherical(dirname+"avg.png");
        out1.close();


        std::ofstream out2(dirname+"avgINT.txt");
        out2<<"Значение  целевой функции усреднённой по "<<Nsym<<" симуляциям "
            <<"для конфигураций, состоящих из "<< N<<" диполей\n";
        out2<<"Время выполнения программы: "<<elapsed_time_ms[0]<<'\n';
        mesh.setMesh3(resultInt);
        mesh.printDec(out2);
        mesh.plotSpherical(dirname+"avgINT.png");

        out2.close();

        std::ofstream out3(dirname+"avgPAR.txt");
        out3<<"Значение  целевой функции усреднённой по "<<Nsym<<" симуляциям "
            <<"для конфигураций, состоящих из "<< N<<" диполей\n";
        out2<<"Время выполнения программы: "<<elapsed_time_ms[2]<<'\n';
        mesh.setMesh3(resultInt);
        mesh.printDec(out3);
        mesh.plotSpherical(dirname+"avgPAR.png");

        out2.close();

        std::cout<<elapsed_time_ms[0]<<'\t'<<elapsed_time_ms[1]<<'\t'<<elapsed_time_ms[2]<<"\n";
        std::cout<<elapsed_time_ms[0]/elapsed_time_ms[1]<<'\t'<<elapsed_time_ms[1]/elapsed_time_ms[2]<<"\n";

        auto mesh3=mesh.getMeshdec()[2];
        for (int i = 0; i < resultInt.size(); ++i) {
            std::cout<<"theta = "<<M_PI*i/12.0<<'\n';
            for (int k = 0; k < resultInt[i].size(); ++k) {
                mesh3[i][k]=resultInt[i][k]-result[i][k];
                BOOST_TEST(resultInt[i][k]==result[i][k]);
            }
        }

    }


BOOST_AUTO_TEST_SUITE_END()