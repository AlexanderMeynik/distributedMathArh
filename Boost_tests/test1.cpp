#include "../application/common/lib.h"
#include <chrono>
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN  // in only one cpp file
#include <boost/test/unit_test.hpp>
#include "../application/math_core/MeshProcessor.h"
#include "../application/math_core/Dipoles.h"
#include <algorithm>
//https://stackoverflow.com/questions/6759560/boosttest-and-mocking-framework
#define BOOST_TEST_MODULE example
BOOST_AUTO_TEST_SUITE( example )
namespace utf = boost::unit_test;
BOOST_AUTO_TEST_CASE( free_test_function,* utf::tolerance(pow(10,-12)) )
/* Compare with void free_test_function() */
{
    //auto res= integrate<double,61>([](double x)->double{return 2*x;},0,2);
    //unsigned const int array[5]={15,31,41,51,61};
    const int mag=7;
    const int mag2=5;
    unsigned const int Ns[5]={15,31,41,51,61};
    unsigned const int maxSizes[5]={5,7,10,12,15};
    double tols[mag]={1e-5,1e-7,1e-10,1e-12,1e-15,1e-17,1e-20};
    double resarr[mag*mag2];
    for (int i=0;i<mag;i++) {
        resarr[i]= integrate<double,15>([](double x)->double{return 2*x;},0,2,5,tols[i]);
    }
    for (int i=0;i<mag;i++) {
        resarr[i+mag]= integrate<double,31>([](double x)->double{return 2*x;},0,2,5,tols[i]);
    }
    for (int i=0;i<mag;i++) {
        resarr[i+2*mag]= integrate<double,41>([](double x)->double{return 2*x;},0,2,5,tols[i]);
    }
    for (int i=0;i<mag;i++) {
        resarr[i+3*mag]= integrate<double,51>([](double x)->double{return 2*x;},0,2,5,tols[i]);
    }
    for (int i=0;i<mag;i++) {
        resarr[i+4*mag]= integrate<double,51>([](double x)->double{return 2*x;},0,2,5,tols[i]);
    }
    for (int i = 0; i < mag2; ++i) {

        for (int j = 0; j < mag; ++j) {
            std::cout<<Ns[i]<<"\t"<<tols[j]<<"\t"<<resarr[i*mag+j]-4<<"\n";
            BOOST_TEST(resarr[i*mag+j]==4);
        }
    }
    //BOOST_TEST(res==4);
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




BOOST_AUTO_TEST_CASE( test_e_impement1 ,* utf::tolerance(pow(10,-12)))
/* Compare with void free_test_function() */
{
    const double  l=1E-7;
    std::array<std::vector<double>,2>coordinates;
    coordinates[0]={0.0};
    coordinates[1]={0.0};
    using dipoles::Dipoles;
    Dipoles<double> d(coordinates[0].size(),coordinates);
    auto sol=d.solve_();
    d.getFullFunction(coordinates,sol);
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
    auto sol=d.solve_();
    d.getFullFunction(coordinates,sol);
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


BOOST_AUTO_TEST_CASE( test_e_impementation3 ,* utf::tolerance(pow(10,-12)))
{
    const double  l=1E-7;
    std::array<std::vector<double>,2>coordinates;
    coordinates[0]={0.0,0.5*l,l};
    coordinates[1]={0.0,0.5*l,l};
    using dipoles::Dipoles;
    Dipoles<double> d(coordinates[0].size(),coordinates);
    auto sol=d.solve_();
    d.getFullFunction(coordinates,sol);
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
    auto sol=d.solve_();
    d.getFullFunction(coordinates,sol);
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


BOOST_AUTO_TEST_SUITE_END()
