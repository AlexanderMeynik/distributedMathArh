#include "../../application/common/lib.h"
#include <chrono>

#include "../../application/math_core/MeshProcessor.h"
#include "../../application/math_core/Dipoles.h"
#include <algorithm>
#include <omp.h>
#include <gtest/gtest.h>

TEST(perfomance_tests,get_calculation_times)
{
    //todo т.к. мы переделали все интерфесы надо будет сделать все эти тесты заново
    ASSERT_EQ(1,2);
}


/*
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
            auto solution=dipoles1.solve_();
            dipoles1.getFullFunction(coordinates[i],solution);
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
        const int N=1;
        const int Nsym=100;
        auto t_prev = std::chrono::high_resolution_clock::now();
        auto t_curr = std::chrono::high_resolution_clock::now();

        std::string dirname="results/experiment2_N="+std::to_string(N)+
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
            auto sol=dipolearr[i].solve_();
            dipolearr[i].getFullFunction(coordinates[i],sol);
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
        out3<<"Время выполнения программы: "<<elapsed_time_ms[2]<<'\n';
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
    static const boost::array< double, 14 > NS{ 1,2,4,5,8,10,20,40,50,100,200,400,500,1000};
    BOOST_DATA_TEST_CASE( free_test_function,NS)
    {
        const int N= sample;
        CoordGenerator<double> genr(0,1e-6);

        auto coord= genr.generateCoordinates(N);
        dipoles::Dipoles<double> dipolearr(N,coord);
        auto solution=dipolearr.solve_();
        dipolearr.getFullFunction(coord,solution);
        const int mag=7;
        const int mag2=5;
        unsigned const int Ns[mag2]={15,31,41,51,61};
        unsigned const int maxSizes[5]={5,7,10,12,15};
        double tols[mag]={1e-5,1e-7,1e-10,1e-12,1e-15,1e-17,1e-20};
        double resarr[mag*mag2];
        double timearr[mag*mag2];
        auto func=dipolearr.getIfunction();
        double rr=2*M_PI/pow(10,15);

        for (int i=0;i<mag;i++) {
            double stime=omp_get_wtime();
            resarr[i]= integrateFunctionBy1Val<double,15>(func,M_PI/12,M_PI/12,0,rr,5,tols[i]);
            timearr[i] = omp_get_wtime()-stime;
        }
        for (int i=0;i<mag;i++) {
            double stime=omp_get_wtime();
            resarr[i+mag]= integrateFunctionBy1Val<double,31>(func,M_PI/12,M_PI/12,0,rr,5,tols[i]);
            timearr[i+mag] = omp_get_wtime()-stime;
        }
        for (int i=0;i<mag;i++) {
            double stime=omp_get_wtime();
            resarr[i+2*mag]= integrateFunctionBy1Val<double,41>(func,M_PI/12,M_PI/12,0,rr,5,tols[i]);
            timearr[i+2*mag] = omp_get_wtime()-stime;
        }
        for (int i=0;i<mag;i++) {
            double stime=omp_get_wtime();

            resarr[i+3*mag]= integrateFunctionBy1Val<double,51>(func,M_PI/12,M_PI/12,0,rr,5,tols[i]);
            timearr[i+3*mag] = omp_get_wtime()-stime;
        }
        for (int i=0;i<mag;i++) {
            double stime=omp_get_wtime();
            resarr[i+4*mag]= integrateFunctionBy1Val<double,61>(func,M_PI/12,M_PI/12,0,rr,5,tols[i]);
            timearr[i+4*mag] = omp_get_wtime()-stime;
        }
        std::cout<<"Tols\t";
        for(auto &elem:tols)
        {
            std::cout<<elem<<"\t";
        }
        std::cout<<"\n";

        for (int i = 0; i < mag2; ++i) {
            std::cout<<Ns[i]<<"\t";
            for (int j = 0; j < mag; ++j) {
                std::cout<<timearr[j+mag*i]*10000*61*25<<"\t";
            }
            std::cout<<"\n";
        }
        std::cout<<"\n";
        std::cout<<"\n";

        for (int i = 0; i < mag2; ++i) {
            std::cout<<Ns[i]<<"\t";
            for (int j = 0; j < mag; ++j) {
                std::cout<<resarr[j+mag*i]<<"\t";
            }
            std::cout<<"\n";
        }



        //BOOST_TEST(res==4);
    }


    static const boost::array< double, 6 > DATA{ 1,10,100,1000,2000,4000};
    BOOST_DATA_TEST_CASE( Foo, DATA )
{
    CoordGenerator<double> genr(0,1e-6);
    double N=sample;
    auto coord= genr.generateCoordinates(N);
    double stime=omp_get_wtime();
    dipoles::Dipoles<double> dipolearr(N,coord);
    double ftime = omp_get_wtime()-stime;
    double ttemp=ftime;
    stime=omp_get_wtime();
    dipolearr.solve_();
    ftime = omp_get_wtime()-stime;
    std::cout<<ttemp<<"\t"<<ftime<<"\t"<<N<<"\n";
}



BOOST_AUTO_TEST_SUITE_END()*/