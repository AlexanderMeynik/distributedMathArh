#include "../../application/common/lib.h"
#include <chrono>

#include "../../application/math_core/math_core.h"

#include "../GoogleCommon.h"
#include <algorithm>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

constexpr double tool = std::numeric_limits<decltype(tool)>::epsilon();

using namespace myconceps;//todo разобраться почему  у нас это дело включается без заголовка
//https://github.com/google/googletest/blob/main/docs/advanced.md#type-parameterized-tests
//тут описана возможность запускать етсты с разными типами
template<typename T>
decltype(auto) get_Default_Configuration() {
    MeshProcessor<T> sample;
    return sample.export_conf();
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

class IsSymmetricTestSuite : public testing::TestWithParam<int> {};

TEST_P(IsSymmetricTestSuite, CheckGenratedMatrixes)
{
    const int size = 2;

    Eigen::MatrixXd mat = Eigen::MatrixXd::Random(size, size);

    Eigen::MatrixXd symMat = (mat + mat.transpose()) / 2;

    ASSERT_TRUE(dipoles::isSymmetric(symMat));
}

INSTANTIATE_TEST_SUITE_P(Matrixes, IsSymmetricTestSuite, testing::Values(2,4,10,100,200,400,800),
                         testing::PrintToStringParamName());
//во 2 примере есть гайд на ток как именуются тесты
// https://www.sandordargo.com/blog/2019/04/24/parameterized-testing-with-gtest
//https://google.github.io/googletest/advanced.html#value-parameterized-tests


TEST(Dipoles,test_solve_result_in_zero_nev)
{
    const int N= 2;
    CoordGenerator<double> genr(0,1e-6);

    auto coord= genr.generateCoordinates2(N);
    dipoles::Dipoles<double> dipolearr(N,coord);
    auto solution=dipolearr.solve3();

    EXPECT_TRUE(solution.size()==4*N);
   // std::cout<<solution<<"\n\n\n\n\n";
  //  std::cout<<dipolearr.getMatrixx()<<"\n\n\n\n\n";

    auto nev=dipolearr.getMatrixx()*solution-dipolearr.getRightPart2();
    //std::cout<<nev;
    {
        EXPECT_NEAR(nev.norm(),0,10e-4);
    }

}


TEST(Dipoles, test_right_part_nev_solve_impl) {


    for (int N = 10; N < 200; N *= 4) {
        for (int i = 0; i < 1; ++i) {
            SCOPED_TRACE("Perform comparison test for N = " + std::to_string(N) + " attempt №" + std::to_string(i));
            CoordGenerator<double> genr(0, 1e-6);

            auto coord = genr.generateCoordinates2(N);
            dipoles::Dipoles<double> dipolearr(N, coord);
            //auto solution=dipolearr.solve2();
            auto rsol = dipolearr.solve3();

            //compare_collections(solution,rsol);
            // EXPECT_NEAR((solution-rsol).norm(),0,tool);
        }

    }


}


template<typename Type>
using DynVector = Eigen::Matrix<Type, Eigen::Dynamic, 1>;
std::string res_dir_path = "../res/";
std::string filename = res_dir_path.append("config.txt");
string subdir = filename.substr(0, filename.rfind('.')) + "data7_25";

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



namespace uu {
    TEST(verification, test_on_10_basik_conf_matrixes) {
        std::ios_base::sync_with_stdio(false);
        auto avec = parseConf2<double, DynVector>(filename);

        std::ifstream in1(subdir + "/matrixes.txt");
        for (int i = 0; i < avec.size(); ++i) {
            auto NN = 0;
            in1 >> NN;
            EXPECT_EQ(NN, avec[i].size() / 2);
            Parser<MatrixXd> pp1(NN);
            in1 >> pp1;


            dipoles::Dipoles<double> dd(avec[i].size() / 2, avec[i]);
            compare_matrices(dd.getMatrixx(), pp1.vals_, i, 1e-5);

        }

    }

    TEST(verification,
         test_on_10_basik_conf_solutions) {
        std::ios_base::sync_with_stdio(false);


        auto avec = getConfSize(filename);

        std::ifstream in1(subdir + "/solutions.txt");
        std::ifstream in2(subdir + "/matrixes.txt");
        for (int i = 0; i < avec; ++i) {
            auto NN = 0;
            in1 >> NN;

            Parser<MatrixXd> pmatrix(NN);
            in2 >> NN;
            in2 >> pmatrix;

            dipoles::Dipoles<double> dd;
            dd.loadFromMatrix(pmatrix.vals_);

            Parser<DynVector<double>> pp1(NN);
            in1 >> pp1;


            auto sol = dd.solve3();
            compare_collections(pp1.vals_, sol, i, tool);
        }
    }

    TEST(verification, test_on_10_basik_conf_meshes) {
        std::ios_base::sync_with_stdio(false);


        auto avec = parseConf2<double, vector>(filename);
        auto conf = get_Default_Configuration<double>();


        std::ifstream in1(subdir + "/meshes.txt");
        std::ifstream in2(subdir + "/solutions.txt");
        in1 >> conf.second[0] >> conf.second[1];
        Parser<MeshProcessor<double>> pp1;

        pp1.vals_.importConf(conf, true);
        for (int i = 0; i < avec.size(); ++i) {
            auto NN = 0;
            in1 >> NN;
            EXPECT_EQ(NN, avec[i].size() / 2);

            in1 >> pp1;
            in2 >> NN;

            Parser<vector<double>> ppsol(NN);
            in2 >> ppsol;


            dipoles::Dipoles<double> dd;//это тут не нужно
            dd.getFullFunction_(avec[i], ppsol.vals_);//todo превартить в статческий метод

            MeshProcessor<double> mm2;
            mm2.importConf(conf, true);
            mm2.generateNoInt(dd.getI2function());

            compare_matrices(pp1.vals_.getMeshdec()[2], mm2.getMeshdec()[2], i, 1e-4);
        }
        in1.close();
        in2.close();
    }

}

using namespace uu;


int main(int argc, char **argv) {//todo cmake+gtestmain
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);


    return RUN_ALL_TESTS();
}