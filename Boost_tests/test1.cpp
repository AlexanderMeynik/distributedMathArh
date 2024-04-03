#include "../application/lib.h"
#include <chrono>
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN  // in only one cpp file
#include <boost/test/unit_test.hpp>
#include "../application/MeshProcessor.h"
#include "../application/Dipoles.h"
#include <algorithm>
//https://stackoverflow.com/questions/6759560/boosttest-and-mocking-framework
#define BOOST_TEST_MODULE example
BOOST_AUTO_TEST_SUITE( example )

BOOST_AUTO_TEST_CASE( free_test_function )
/* Compare with void free_test_function() */
{
    auto res= integrate<double>([](double x)->double{return 2*x;},0,2);
    BOOST_TEST(res==4);
}


BOOST_AUTO_TEST_CASE( test_linpace )
/* Compare with void free_test_function() */
{
    auto res1=matplot::linspace(0,10,20);
    auto my_res1= myLinspace<double>(0.0,10.0,20);

    BOOST_TEST(res1==my_res1);
    //BOOST_CHECK_EQUAL_COLLECTIONS(res1.begin(),res1.end(),res2.b)
}

BOOST_AUTO_TEST_CASE( test_meshfunc )
/* Compare with void free_test_function() */
{
    auto mesh1=matplot::meshgrid(matplot::linspace(1.0, 10.0, 2), matplot::linspace(0.0, 10.0, 3));
    auto mesh2= mymeshGrid(matplot::linspace(1.0, 10.0, 2), matplot::linspace(0.0, 10.0, 3));
    for (int i = 0; i < mesh1.first.size(); ++i) {
        BOOST_CHECK_EQUAL_COLLECTIONS(mesh1.first[i].begin(), mesh1.first[i].end(), mesh2.first[i].begin(), mesh2.first[i].end());
    }
    for (int i = 0; i < mesh1.second.size(); ++i) {
        BOOST_CHECK_EQUAL_COLLECTIONS(mesh1.second[i].begin(), mesh1.second[i].end(), mesh2.second[i].begin(), mesh2.second[i].end());

    }

    //BOOST_CHECK_EQUAL_COLLECTIONS(mesh1.begin(),mesh1.end(),res2.b)
}

BOOST_AUTO_TEST_CASE(testdot)
{
    Eigen::Vector2d init={1,2};
    auto ss=init*2;
    Eigen::Vector2d init2={2,4};
    BOOST_CHECK_EQUAL_COLLECTIONS(ss.begin(), ss.end(),
                                  init2.begin(), init2.end());
}

BOOST_AUTO_TEST_CASE(testcwise)
{
    Eigen::Vector2d init={1,2};
    Eigen::Vector2d mm={3,4};
    auto ss=init.cwiseProduct(mm);
    Eigen::Vector2d init2={3,8};
    BOOST_CHECK_EQUAL_COLLECTIONS(ss.begin(), ss.end(),
                                  init2.begin(), init2.end());
}


namespace utf = boost::unit_test;

BOOST_AUTO_TEST_CASE( test_e_impement1 ,* utf::tolerance(pow(10,-12)))
/* Compare with void free_test_function() */
{
    const double  l=1E-7;
    std::array<std::vector<double>,2>coordinates;
    coordinates[0]={0.0};
    coordinates[1]={0.0};
    using dipoles::Dipoles;
    Dipoles<double> d(coordinates[0].size(),coordinates);
    d.solve_();
    auto sol=d.getSolution_();
    d.getFullFunction();
    auto f1=d.getIfunction();
    auto f2=d.getI2function();

    MeshProcessor<double> mesh;
    mesh.generateMeshes(f1);
    auto mesh1=mesh.getMeshdec();
    mesh.generateNoInt(f2);
    auto mesh2=mesh.getMeshdec();
    auto mesh3=mesh2;
    for (int i = 0; i < mesh1[2].size(); ++i) {
        std::cout<<"theta = "<<M_PI*i/12.0<<'\n';
        for (int k = 0; k < mesh1[2][i].size(); ++k) {

            mesh3[2][i][k]=mesh1[2][i][k]-mesh2[2][i][k];
            BOOST_TEST(mesh1[2][i][k]==mesh2[2][i][k]);
        }
    }
}

BOOST_AUTO_TEST_CASE( test_e_impementation2 ,* utf::tolerance(pow(10,-12)))
{
    const double  l=1E-7;
    std::array<std::vector<double>,2>coordinates;
    coordinates[0]={0.0,l};
    coordinates[1]={0.0,l};
    using dipoles::Dipoles;
    Dipoles<double> d(coordinates[0].size(),coordinates);
    d.solve_();
    auto sol=d.getSolution_();
    d.getFullFunction();
    auto f1=d.getIfunction();
    auto f2=d.getI2function();

    MeshProcessor<double> mesh;
    mesh.generateMeshes(f1);
    auto mesh1=mesh.getMeshdec();
    mesh.generateNoInt(f2);
    auto mesh2=mesh.getMeshdec();
    auto mesh3=mesh2;
        for (int i = 0; i < mesh1[2].size(); ++i) {
            std::cout<<"theta = "<<M_PI*i/12.0<<'\n';
            for (int k = 0; k < mesh1[2][i].size(); ++k) {

                mesh3[2][i][k]=mesh1[2][i][k]-mesh2[2][i][k];
                BOOST_TEST(mesh1[2][i][k]==mesh2[2][i][k]);
            }
        }
}

namespace utf = boost::unit_test;
BOOST_AUTO_TEST_CASE( test_e_impementation3 ,* utf::tolerance(pow(10,-12)))
{
    const double  l=1E-7;
    std::array<std::vector<double>,2>coordinates;
    coordinates[0]={0.0,0.5*l,l};
    coordinates[1]={0.0,0.5*l,l};
    using dipoles::Dipoles;
    Dipoles<double> d(coordinates[0].size(),coordinates);
    d.solve_();
    auto sol=d.getSolution_();
    d.getFullFunction();
    auto f1=d.getIfunction();
    auto f2=d.getI2function();

    MeshProcessor<double> mesh;
    mesh.generateMeshes(f1);
    auto mesh1=mesh.getMeshdec();
    mesh.generateNoInt(f2);
    auto mesh2=mesh.getMeshdec();
    auto mesh3=mesh2;

    for (int i = 0; i < mesh1[2].size(); ++i) {
        std::cout<<"theta = "<<M_PI*i/12.0<<'\n';
        for (int k = 0; k < mesh1[2][i].size(); ++k) {

            mesh3[2][i][k]=mesh1[2][i][k]-mesh2[2][i][k];
            BOOST_TEST(mesh1[2][i][k]==mesh2[2][i][k]);
        }
    }



}


BOOST_AUTO_TEST_CASE( test_e_impementation4 ,* utf::tolerance(pow(10,-12)))
{
    const double  l=1E-7;
    std::array<std::vector<double>,2>coordinates;
    coordinates[0]={0.0,0.5*l,0.0,0.5*l,l};
    coordinates[1]={0.0,0.0,0.5*l,0.5*l,l};
    using dipoles::Dipoles;
    Dipoles<double> d(coordinates[0].size(),coordinates);
    d.solve_();
    auto sol=d.getSolution_();
    d.getFullFunction();
    auto f1=d.getIfunction();
    auto f2=d.getI2function();

    MeshProcessor<double> mesh;
    mesh.generateMeshes(f1);
    auto mesh1=mesh.getMeshdec();
    mesh.generateNoInt(f2);
    auto mesh2=mesh.getMeshdec();
    auto mesh3=mesh2;

    for (int i = 0; i < mesh1[2].size(); ++i) {
        std::cout<<"theta = "<<M_PI*i/12.0<<'\n';
        for (int k = 0; k < mesh1[2][i].size(); ++k) {
            mesh3[2][i][k]=mesh1[2][i][k]-mesh2[2][i][k];
            BOOST_TEST(mesh1[2][i][k]==mesh2[2][i][k]);
        }
    }

}

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

BOOST_AUTO_TEST_SUITE_END()
