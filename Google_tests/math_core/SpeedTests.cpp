#include "../../application/common/lib.h"
#include <chrono>

#include "../../application/math_core/MeshProcessor.h"
#include "../../application/math_core/Dipoles.h"
#include "../GoogleCommon.h"
#include <algorithm>
#include <omp.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <eigen3/Eigen/Dense>
#include <filesystem>
#include <type_traits>
#include <concepts>
constexpr double tool =std::numeric_limits<decltype(tool)>::epsilon();

//todo обобщить при помощи рекурсивных шаблонов
/*void
compare_collections(const Eigen::Vector<double, -1> &solution, const Eigen::Vector<double, -1> &solution2) {
    EXPECT_TRUE(solution.size()==solution2.size());
    auto ss=solution2.size();
    for (int i = 0; i < ss; ++i) {
        SCOPED_TRACE("Checked index "+std::to_string(ss)+'\n');
        EXPECT_NEAR(solution[i],solution2[i],tool);
    }
}*/

using namespace myconceps;

/*
template<typename... Args, template<typename...> typename Container>
void
compare_collections2(const Container<Args...> &solution, const Container<Args...> &solution2,int ii) {
    EXPECT_TRUE(solution.size()==solution2.size());
    auto ss=solution2.size();
    for (int i = 0; i < ss; ++i) {

        auto ss1=solution2[i].size();
        EXPECT_TRUE(solution[i].size()==solution2[i].size());
        for (int j = 0; j < ss1; ++j) {
            SCOPED_TRACE("iteration "+std::to_string(ii)+'\n');
            SCOPED_TRACE("Checked index "+std::to_string(ss)+'\n');
            SCOPED_TRACE("Checked index "+std::to_string(ss1)+'\n');
            EXPECT_NEAR(solution[i][j],solution2[i][j],tool);
        }
    }
}*/




/*
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


TEST(Dipoles,test_solve_result_in_zero_nev)
{
    const int N= 2;
    CoordGenerator<double> genr(0,1e-6);

    auto coord= genr.generateCoordinates2(N);
    dipoles::Dipoles<double> dipolearr(N,coord);
    auto solution=dipolearr.solve3();

    EXPECT_TRUE(solution.size()==4*N);
    std::cout<<solution<<"\n\n\n\n\n";
    std::cout<<dipolearr.getMatrixx()<<"\n\n\n\n\n";

    auto nev=dipolearr.getMatrixx()*solution-dipolearr.getRightPart2();
    std::cout<<nev;
    {
        EXPECT_NEAR(nev.norm(),0,10e-4);
    }

}*/

//todo достать старые тесты на занчения и всё перепроверить внимательно
//todo поменять команды для парсинга
//todo raw biffers initalizrion for eigen vectors https://eigen.tuxfamily.org/dox/group__TutorialMapClass.html
/*TEST(Dipoles,test_right_part_nev_solve_impl)
{


    for (int N = 10; N < 200; N*=4) {
        for (int i = 0; i < 1; ++i) {
            SCOPED_TRACE("Perform comparison test for N = "+std::to_string(N)+" attempt №"+std::to_string(i));
            CoordGenerator<double> genr(0,1e-6);

            auto coord= genr.generateCoordinates2(N);
            dipoles::Dipoles<double> dipolearr(N,coord);
            //auto solution=dipolearr.solve2();
            auto rsol=dipolearr.solve3();

            //compare_collections(solution,rsol);
           // EXPECT_NEAR((solution-rsol).norm(),0,tool);
        }

    }


}

*/

template <typename Type>
using DynVector = Eigen::Matrix<Type,Eigen::Dynamic,1>;
/*
TEST(verification,test_on_10_basik_conf)
{
    std::ios_base::sync_with_stdio(false);
    int verbose=2;
    std::string filename="config.txt";



    auto avec= parseConf2<double,vector>(filename);
    std::string dirname=filename.erase(filename.find('.'));
    auto subdir=dirname+"data";
    std::filesystem::create_directory(subdir);
    std::ofstream out1(subdir+"/solutions");
    std::ofstream out(subdir+"/matrixes");
    std::ofstream out2(subdir+"/meshes");
    for (int i = 0; i < avec.size(); ++i) {

        out<<avec[i].size()/2<<'\n';
        out1<<avec[i].size()/2<<"\n";
        out2<<avec[i].size()/2<<"\n";
        dipoles::Dipoles<double> dd(avec[i].size()/2,avec[i]);
        auto solution=dd.solve4();
        out<<dd.getMatrixx()<<"\n";

        //printSolutionFormat1(out,solution);
       // out<<"\n\n";
        printSolution(out1,solution);
        //std::vector<double> solvec(solution.begin(),solution.end());
        dd.getFullFunction3(avec[i],solution);
        auto func=dd.getI2function();

        MeshProcessor<double> meshProcessor;
        meshProcessor.generateNoInt(func);
        //auto mesh1=meshProcessor.getMeshsph();
        //auto mesh2=meshProcessor.getMeshdec();
        meshProcessor.printDec(out2);
        meshProcessor.plotSpherical(subdir+"/plot"+std::to_string(i)+".png");

        //todo read data
        //start from Matrix
        //then solution format 1
        //then solution format 2
        //a last we'll check the mesh for function
        //function to ignore all text



    }//todo сделать простой парсер, чтобы сопоставлять данные

}*/
TEST(verification,test_on_10_basik_conf_matrixes)
{
    std::ios_base::sync_with_stdio(false);
    std::string filename="config.txt";
    auto avec= parseConf2<double,DynVector>(filename);
    std::string dirname=filename.erase(filename.find('.'));
    auto subdir=dirname+"data";
    std::ifstream in1(subdir+"/matrixes");
    for (int i = 0; i < avec.size(); ++i) {
        auto NN=0;
        in1>>NN;//todo удалить N оттуда
        EXPECT_EQ(NN,avec[i].size()/2);
        Parser<MatrixXd> pp1(NN);
        in1>>pp1;


        dipoles::Dipoles<double> dd(avec[i].size()/2,avec[i]);
        compare_matrices(dd.getMatrixx(),pp1.vals_,i,1e-5);

    }

}

TEST(verification,test_on_10_basik_conf_solutions)
{
    std::ios_base::sync_with_stdio(false);
    std::string filename="config.txt";



    auto avec= parseConf2<double,DynVector>(filename);
    std::string dirname=filename.erase(filename.find('.'));
    auto subdir=dirname+"data";
    std::ifstream in1(subdir+"/solutions");
    for (int i = 0; i < avec.size(); ++i) {
        auto NN=0;
        in1>>NN;//todo удалить N оттуда
        EXPECT_EQ(NN,avec[i].size()/2);

        Parser<DynVector<double>> pp1(NN);
        in1>>pp1;


        dipoles::Dipoles<double> dd(avec[i].size()/2,avec[i]);
        auto sol=dd.solve3();
        compare_collections(pp1.vals_,sol,i,tool);
    }
}

TEST(verification,test_on_10_basik_conf_meshes)
{
    std::ios_base::sync_with_stdio(false);
    std::string filename="config.txt";



    auto avec= parseConf2<double,DynVector>(filename);
    std::string dirname=filename.erase(filename.find('.'));
    auto subdir=dirname+"data";
    std::ifstream in1(subdir+"/meshes");
    for (int i = 0; i < avec.size(); ++i) {
        auto NN=0;
        in1>>NN;//todo удалить N оттуда
        EXPECT_EQ(NN,avec[i].size()/2);

        Parser<MeshProcessor<double>> pp1(NN);
        in1>>pp1;


        dipoles::Dipoles<double> dd(avec[i].size()/2,avec[i]);
        auto sol=dd.solve3();
        dd.getFullFunction(avec[i],sol);

        MeshProcessor<double>mm2;
        mm2.generateNoInt(dd.getI2function());


        compare_matrices(pp1.vals_.getMeshdec()[2],mm2.getMeshdec()[2],i,1e-4);
    }
}



/*TEST(verification,test_on_10_basik_conf2)
{
    std::ios_base::sync_with_stdio(false);
    int verbose=2;
    std::string filename="config.txt";



    auto avec= parseConf2<double,DynVector>(filename);
    std::string dirname=filename.erase(filename.find('.'));
    auto subdir=dirname+"osas";
    //std::filesystem::create_directory(subdir);
    for (int i = 0; i < avec.size(); ++i) {
        std::cout<<i<<'\n';
        std::ifstream in(subdir+"/data"+std::to_string(i)+".txt");
        auto NN=0;
        in>>NN;
        EXPECT_EQ(NN,avec[i].size()/2);

        //DynVector<double> solution1= parseFrom_file<double>(in,2*NN);

        Parser<MatrixXd> pp1(NN);
        in>>pp1;

        Parser<DynVector<double>> pp(NN);
        in>>pp;

        Parser<MeshProcessor<double>> pp3(NN);
        in>>pp3;


        dipoles::Dipoles<double> dd(avec[i].size()/2,avec[i]);
        EXPECT_TRUE(!(dd.getMatrixx()-pp1.vals_).any());
        DynVector<double> solution=dd.solve3();
        //pp.vals_.size();
        compare_collections(pp.vals_,solution,tool);
        //ff[0];
        //auto diff=pp.vals_==solution;

        //std::vector<double> solvec(solution.begin(),solution.end());
        dd.getFullFunction(avec[i],solution);
        auto func=dd.getI2function();

        MeshProcessor<double> meshProcessor;
        meshProcessor.generateNoInt(func);
        compare_matrices(pp3.vals_.getMeshdec()[2],meshProcessor.getMeshdec()[2],i);

    }

}*/



int main(int argc, char **argv)
{//todo cmake+gtestmain
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);


    return RUN_ALL_TESTS();
}