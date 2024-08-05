#include "../../application/common/lib.h"
#include <chrono>

#include "../../application/math_core/MeshProcessor.h"
#include "../../application/math_core/Dipoles.h"
#include <algorithm>
#include <omp.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <eigen3/Eigen/Dense>


constexpr double tool =std::numeric_limits<decltype(tool)>::epsilon();

//todo обобщить при помощи рекурсивных шаблонов
void
compare_collections(const Eigen::Vector<double, -1> &solution, const Eigen::Vector<double, -1> &solution2) {
    EXPECT_TRUE(solution.size()==solution2.size());
    auto ss=solution2.size();
    for (int i = 0; i < ss; ++i) {
        SCOPED_TRACE("Checked index "+std::to_string(ss)+'\n');
        EXPECT_NEAR(solution[i],solution2[i],tool);
    }
}
TEST(Dipoles, test_solve2_implementation)
{
    const int N= 10;
    CoordGenerator<double> genr(0,1e-6);

    auto coord= genr.generateCoordinates(N);
    dipoles::Dipoles<double> dipolearr(N,coord);
    auto solution=dipolearr.solve2();
    auto solution2=dipolearr.solve_();

    EXPECT_TRUE(solution.size()==2*solution2[0].size());
    auto ss=solution2[0].size();
    for (int i = 0; i < ss; ++i) {
        SCOPED_TRACE("Checked index "+std::to_string(ss)+'\n');
        EXPECT_NEAR(solution[i],solution2[0][i],tool);
        EXPECT_NEAR(solution[ss+i],solution2[1][i],tool);
    }

    //auto nevyazk=(dipolearr.getMatrixx()*coord-solution)


}
TEST(transformations, reinterpret_vector_test)
{
    auto N=20;
    CoordGenerator<double> genr(0,1e-6);
    auto EigenVec=genr.generateCoordinates2(N);
    auto arr2vec= reinterpretVector(EigenVec);
    EXPECT_TRUE(EigenVec.size()==2*arr2vec[0].size()&&EigenVec.size()==2*N);
    for (int i = 0; i < N; ++i) {
        SCOPED_TRACE("Checked index "+std::to_string(i)+'\n');
        EXPECT_NEAR(EigenVec[i],arr2vec[0][i],tool);
        EXPECT_NEAR(EigenVec[i+N],arr2vec[1][i],tool);
    }

}

TEST(Dipoles, test_init_vec_impl)
{
    const int N= 10;
    CoordGenerator<double> genr(0,1e-6);

    auto coord= genr.generateCoordinates2(N);
    dipoles::Dipoles<double> dipolearr(N,coord);
    auto solution=dipolearr.solve2();


    auto coord2= reinterpretVector(coord);
    dipoles::Dipoles<double> dipolearr2(N,coord);
    auto solution2=dipolearr.solve2();


    compare_collections(solution,solution2);
    //auto nevyazk=(dipolearr.getMatrixx()*coord-solution)


}

TEST(Dipoles,test_solve_result_in_zero_nev)
{
    const int N= 100;
    CoordGenerator<double> genr(0,1e-6);

    auto coord= genr.generateCoordinates2(N);
    dipoles::Dipoles<double> dipolearr(N,coord);
    auto solution=dipolearr.solve2();

    EXPECT_TRUE(solution.size()==4*N);
    auto nev=dipolearr.getMatrixx()*solution-dipolearr.getRightPart2();

    {
        EXPECT_NEAR(nev.norm(),0,10e-4);
    }

}


TEST(Dipoles,test_right_part_nev_solve_impl)
{


    for (int N = 10; N < 200; N*=4) {
        for (int i = 0; i < 10; ++i) {
            SCOPED_TRACE("Perform comparison test for N = "+std::to_string(N)+" attempt №"+std::to_string(i));
            CoordGenerator<double> genr(0,1e-6);

            auto coord= genr.generateCoordinates2(N);
            dipoles::Dipoles<double> dipolearr(N,coord);
            auto solution=dipolearr.solve2();
            auto rsol=dipolearr.solve3();

            compare_collections(solution,rsol);
            EXPECT_NEAR((solution-rsol).norm(),0,tool);
        }

    }


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

int main(int argc, char **argv)
{//todo cmake+gtestmain
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);


    return RUN_ALL_TESTS();
}