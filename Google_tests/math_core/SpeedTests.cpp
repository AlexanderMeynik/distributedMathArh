#include "../../application/common/lib.h"
#include <chrono>

#include "../../application/math_core/math_core.h"

#include "../GoogleCommon.h"
#include <algorithm>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

constexpr double tool =std::numeric_limits<decltype(tool)>::epsilon();

using namespace myconceps;

template<typename T>
decltype(auto) get_Default_Configuration()
{
    MeshProcessor<T> sample;
    return sample.export_conf();
}
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
std::string res_dir_path="../res/";
std::string filename=res_dir_path.append("config.txt");
string subdir=filename.substr(0, filename.rfind('.')) + "data7_25";
#include <filesystem>
//todo перместить это тметов генераторы(тесты, которые прост осоздаёт данные)
/*
TEST(verification,test_on_10_basik_conf)
{

    std::ios_base::sync_with_stdio(false);
    auto avec= parseConf2<double,vector>(filename);



    std::filesystem::create_directory(subdir);
    std::ofstream out1(subdir+"/solutions.txt");
    std::ofstream out(subdir+"/matrixes.txt");
    std::ofstream out2(subdir+"/meshes.txt");
    auto conf=get_Default_Configuration<double>();

    //conf.second[0]*=4;
    //conf.second[1]*=4;
    out2<<scientificNumber(conf.second[0])<<'\t'<<scientificNumber(conf.second[1])<<'\n';
    for (int i = 0; i < avec.size(); ++i) {

        out<<avec[i].size()/2<<'\n';
        out1<<avec[i].size()/2<<"\n";
        out2<<avec[i].size()/2<<"\n";
        dipoles::Dipoles<double> dd(avec[i].size()/2,avec[i]);
        auto solution=dd.solve4();
        out<<dd.getMatrixx()<<"\n";


        printSolution(out1,solution);

        dd.getFullFunction3(avec[i],solution);
        auto func=dd.getI2function();

        MeshProcessor<double> meshProcessor;
        meshProcessor.importConf(conf, true);
        meshProcessor.generateNoInt(func);

        meshProcessor.printDec(out2);
        meshProcessor.plotSpherical(subdir+"/plot"+std::to_string(i)+".png");




    }

}*/



namespace uu{
TEST(verification, test_on_10_basik_conf_matrixes) {
    std::ios_base::sync_with_stdio(false);
    auto avec = parseConf2<double, DynVector>(filename);

    std::ifstream in1(subdir + "/matrixes.txt");
    for (int i = 0; i < avec.size(); ++i) {
        auto NN = 0;
        in1 >> NN;//todo удалить N оттуда
        EXPECT_EQ(NN, avec[i].size() / 2);
        Parser<MatrixXd> pp1(NN);
        in1 >> pp1;


        dipoles::Dipoles<double> dd(avec[i].size() / 2, avec[i]);
        compare_matrices(dd.getMatrixx(), pp1.vals_, i, 1e-5);

    }

}

TEST(verification, test_on_10_basik_conf_solutions) {//todo изолировать(убрать все предыдущие этапы(читаем тут матрицу)
    std::ios_base::sync_with_stdio(false);


    auto avec = parseConf2<double, DynVector>(filename);

    std::ifstream in1(subdir + "/solutions.txt");
    for (int i = 0; i < avec.size(); ++i) {
        auto NN = 0;
        in1 >> NN;//todo удалить N оттуда
        EXPECT_EQ(NN, avec[i].size() / 2);

        Parser<DynVector<double>> pp1(NN);
        in1 >> pp1;


        dipoles::Dipoles<double> dd(avec[i].size() / 2, avec[i]);
        auto sol = dd.solve3();
        compare_collections(pp1.vals_, sol, i, tool);
    }
}

TEST(verification, test_on_10_basik_conf_meshes) {//todo считываем решение
    std::ios_base::sync_with_stdio(false);


    auto avec = parseConf2<double, DynVector>(filename);
    auto conf = get_Default_Configuration<double>();


    std::ifstream in1(subdir + "/meshes.txt");
    in1 >> conf.second[0] >> conf.second[1];
    Parser<MeshProcessor<double>> pp1;

    pp1.vals_.importConf(conf, true);
    for (int i = 0; i < avec.size(); ++i) {
        auto NN = 0;
        in1 >> NN;//todo удалить N оттуда
        EXPECT_EQ(NN, avec[i].size() / 2);


        in1 >> pp1;

        dipoles::Dipoles<double> dd(avec[i].size() / 2, avec[i]);
        auto sol = dd.solve3();
        dd.getFullFunction(avec[i], sol);

        MeshProcessor<double> mm2;
        mm2.importConf(conf, true);
        mm2.generateNoInt(dd.getI2function());

        compare_matrices(pp1.vals_.getMeshdec()[2], mm2.getMeshdec()[2], i, 1e-4);
    }
}

}

using namespace uu;


int main(int argc, char **argv)
{//todo cmake+gtestmain
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);


    return RUN_ALL_TESTS();
}