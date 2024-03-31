#include "../application/lib.h"
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN  // in only one cpp file
#include <boost/test/unit_test.hpp>
#include "../application/MeshProcessor.h"
#include "../application/Dipoles.h"
#include <algorithm>
//https://stackoverflow.com/questions/6759560/boosttest-and-mocking-framework

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


