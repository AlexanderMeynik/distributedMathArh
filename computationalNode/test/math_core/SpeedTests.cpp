
#include <chrono>


#include "common/lib.h"
/*#include "computationalLib/math_core/math_core.h"*/
#include "computationalLib/math_core/Dipoles.h"
#include "computationalLib/math_core/MeshProcessor2.h"
#include "computationalLib/math_core/MeshProcessor.h"
#include "../GoogleCommon.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>



using namespace myConcepts;
using namespace testCommon;
using namespace Eigen;


template<typename T>
decltype(auto) get_Default_Configuration() {
    meshStorage::MeshProcessor2 sample;
    return sample.export_conf();
}

static inline double C1=0;
static inline double C2=0;
auto dipole1Function(double theta, double phi)
{
    2*M_PI*pow(params::omega,3)*
    (pow(params::omega*sin(theta)/params::c,2)*
        (pow(C1,4)+pow(C2,4)+2*pow(C1*C2,2))+
    (C1*C1+C2*C2)-
    (pow(sin(theta),2)*(C1*C1+C2*C2)/2.));
}

TEST(transformations, reinterpret_vector_test) {
    auto N = 20;
    CoordGenerator<double> genr(0, 1e-6);
    auto EigenVec = genr.generateCoordinates2(N);
    auto arr2vec = reinterpretVector(EigenVec);
    EXPECT_TRUE(EigenVec.size() == 2 * arr2vec[0].size() && EigenVec.size() == 2 * N);
    for (int i = 0; i < N; ++i) {
        SCOPED_TRACE("Checked index " + std::to_string(i) + '\n');
        EXPECT_NEAR(EigenVec[i], arr2vec[0][i], tool);
        EXPECT_NEAR(EigenVec[i + N], arr2vec[1][i], tool);
    }

}

class IsSymmetricTestSuite : public testing::TestWithParam<int> {
};

TEST_P(IsSymmetricTestSuite, CheckGenratedMatrixes) {
    const int size = 2;

    Eigen::MatrixXd mat = Eigen::MatrixXd::Random(size, size);

    Eigen::MatrixXd symMat = (mat + mat.transpose()) / 2;

    ASSERT_TRUE(dipoles::isSymmetric(symMat));
}

INSTANTIATE_TEST_SUITE_P(Matrixes, IsSymmetricTestSuite, testing::Values(2, 4, 10, 100, 200, 400, 800),
                         testing::PrintToStringParamName());
//во 2 примере есть гайд на ток как именуются тесты
// https://www.sandordargo.com/blog/2019/04/24/parameterized-testing-with-gtest
//https://google.github.io/googletest/advanced.html#value-parameterized-tests


TEST(Dipoles, test_solve_result_in_zero_nev) {
    const int N = 2;
    CoordGenerator<double> genr(0, 1e-6);

    auto coord = genr.generateCoordinates2(N);
    dipoles::Dipoles dipolearr(N, coord);
    auto solution = dipolearr.solve<dipoles::EigenVec>();

    EXPECT_TRUE(solution.size() == 4 * N);
    // std::cout<<solution<<"\n\n\n\n\n";
    //  std::cout<<dipolearr.getMatrixx()<<"\n\n\n\n\n";

    auto nev = dipolearr.getMatrixx() * solution - dipolearr.getRightPart();
    //std::cout<<nev;
    {
        EXPECT_NEAR(nev.norm(), 0, 10e-4);
    }

}


TEST(Dipoles, test_right_part_nev_solve_impl) {


    for (int N = 10; N < 200; N *= 4) {
        for (int i = 0; i < 1; ++i) {
            SCOPED_TRACE("Perform comparison test for N = " + std::to_string(N) + " attempt №" + std::to_string(i));
            CoordGenerator<double> genr(0, 1e-6);

            auto coord = genr.generateCoordinates2(N);
            dipoles::Dipoles dipolearr(N, coord);
            //auto solution=dipolearr.solve2();
            auto rsol = dipolearr.solve<dipoles::EigenVec>();
            //todo solve 3
            //compare_collections(solution,rsol);
            // EXPECT_NEAR((solution-rsol).norm(),0,tool);
        }

    }


}


template<typename Type>
using DynVector = Eigen::Matrix<Type, Eigen::Dynamic, 1>;
std::string res_dir_path = "../../../res/";
std::string filename = res_dir_path.append("config.txt");
string subdir = filename.substr(0, filename.rfind('.')) + "data7_25";//todo вот этот путь у нас теперь не верен




class DipolesVerificationTS : public ::testing::Test {
protected:

};

TEST_F(DipolesVerificationTS, test_on_10_basik_conf_matrixes) {
    std::ios_base::sync_with_stdio(false);
    auto avec = parseConf2<double, DynVector>(filename);

    std::ifstream in1(subdir + "/matrixes.txt");
    for (int i = 0; i < avec.size(); ++i) {
        auto NN = 0;
        in1 >> NN;
        EXPECT_EQ(NN, avec[i].size() / 2);
        Parser<MatrixXd> pp1(NN);
        in1 >> pp1;


        dipoles::Dipoles dd(avec[i].size() / 2, avec[i]);


        compare2dArrays(dd.getMatrixx(), pp1.vals_, double_comparator3, 1e20 / 10000);


    }

}

TEST_F(DipolesVerificationTS,
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

        dipoles::Dipoles dd;
        dd.loadFromMatrix(pmatrix.vals_);

        Parser<DynVector<double>> pp1(NN);
        in1 >> pp1;


        auto sol = dd.solve<dipoles::EigenVec>();
        compareArrays(pp1.vals_,sol,double_comparator2);//todo assert near
        //todo printer / parser must print all numbers during serializtion(wath gtest)
    }
}


static const  size_t dimCount=2;
class TestMeshImpl
{
public:
    TestMeshImpl(): dimensions({7,25}), limits({0, M_PI_2,0, M_PI * 2}),
                    data({std::valarray<FloatType>(dimensions[0]*dimensions[1]),
                          std::valarray<FloatType>(dimensions[0]*dimensions[1]),
                          std::valarray<FloatType>(dimensions[0]*dimensions[1])}){

    }

    void constructMeshes(const std::optional<std::array<size_t,2>> dimenstion=std::nullopt,
                         const std::optional<std::array<FloatType ,4>> limit=std::nullopt)
    {
        if(dimenstion.has_value())
        {
            this->dimensions=dimenstion.value();
        }
        if(limit.has_value())
        {
            this->limits=limit.value();
        }

        auto phi=meshStorage::myLinspace<std::valarray>(limits[2],limits[3],dimensions[1]);
        auto theta=meshStorage::myLinspace<std::valarray>(limits[0],limits[1],dimensions[0]);
        std::array<std::valarray<FloatType>,dimCount> coords;
        for (size_t i = 0; i <dimCount ; ++i) {
            coords[i]=meshStorage::myLinspace<std::valarray>(limits[2*i],limits[2*i+1],dimensions[i]);
        }

        auto rr=meshStorage::myMeshGrid(phi,theta);
        data[0]=rr[0];
        data[1]=rr[1];
        computeViews();
    }

    meshStorage::MeshProcessor2::meshArr<dimCount+1> sphericalTransformation() {
        meshStorage::MeshProcessor2::meshArr<dimCount+1> res;
        for (size_t i = 0; i < dimCount+1; ++i) {
            res[i]=data[i];
        }


        res[0] = this->data[2] * sin(this->data[1]) * cos(this->data[0]);
        res[1] = this->data[2] * sin(this->data[1]) * sin(this->data[0]);
        res[2] = this->data[2] * cos(this->data[1]);
        return res;

    }

    void applyFunction(const dipoles::directionGraph&plot)
    {
        data[2]=meshStorage::computeFunction(data[0],data[1],plot/*[&plot](FloatType x,FloatType y){return plot(y,x);}*/);
        computeViews(2);
    }
std::array<meshStorage::mdSpanType,3> spans;
public:
    void computeViews(int val=-1)
    {
        if(val!=-1)
        {
            spans[val]=meshStorage::mdSpanType(&(data[val][0]),dimensions[0],
                                                               dimensions[1]);
            return;
        }
        for (size_t i = 0; i < spans.size(); ++i) {
            spans[i]=meshStorage::mdSpanType(&(data[i][0]),dimensions[0],
                                                          dimensions[1]);
        }
    }
    /*meshStorage::MeshProcessor2::mdSpanType;*/
    std::array<size_t,dimCount> dimensions;
    std::array<FloatType ,dimCount*2> limits;
    meshStorage::MeshProcessor2::meshArr<dimCount+1> data;
};



template<>
class Parser<TestMeshImpl> {
public:
    Parser() : vals_() {}

    Parser(int size) : vals_() {}

    friend std::istream &operator>>(std::istream &in, Parser &pp) {
        using namespace meshStorage;
        std::string dummy;
        std::getline(in, dummy);
        std::getline(in, dummy);
        std::getline(in, dummy);
        //pp.vals_=MeshProcessor<T>();

        //todo read coord meshes;
        auto n=pp.vals_.dimensions;

        pp.vals_.constructMeshes();

        meshStorageType m = pp.vals_.data[0];

        mdSpanType span=mdSpanType
                (&(m[0]),n[0],n[1]);

        for (int i = 0; i < span.extent(1); ++i) {
            FloatType temp = 0;
            in >> temp;
            for (int j = 0; j < span.extent(0); ++j) {
                FloatType val;
                in >> val;
                m[j*span.extent(1)+i]=val;

                /*span[std::array{j,i}] = val;*/
            }
        }

        /*auto ss=meshStorage::MeshProcessor2::transpose(span);*/

        pp.vals_.data[2]=m;
        return in;
    }

    TestMeshImpl vals_;

};

TEST_F(DipolesVerificationTS, test_on_10_basik_conf_meshes) {
    std::ios_base::sync_with_stdio(false);


    auto avec = parseConf2<double, vector>(filename);
    auto conf = get_Default_Configuration<double>();
//todo here we get default coniguration for meshes i think we must set as standart that we have only 2 nums

    std::ifstream in1(subdir + "/meshes.txt");
    std::ifstream in2(subdir + "/solutions.txt");
    in1 >> conf.second[0] >> conf.second[1];
    Parser<TestMeshImpl> pp1;

    for (int i = 0; i < avec.size(); ++i) {
        auto NN = 0;
        in1 >> NN;
        EXPECT_EQ(NN, avec[i].size() / 2);

        in1 >> pp1;
        in2 >> NN;

        Parser<vector<double>> ppsol(NN);
        in2 >> ppsol;


        dipoles::Dipoles dd;
        dd.getFullFunction_(avec[i], ppsol.vals_);//todo превартить в статческий метод


       MeshProcessor mm2;
        mm2.importConf(conf, true);
        mm2.generateNoInt(dd.getI2function());//todo попробовать руками всё сдлеать

        TestMeshImpl mm;
        mm.constructMeshes();
        mm.applyFunction(dd.getI2function());
        auto r2=meshStorage::unflatten(mm.data[2],mm.dimensions.data());

        auto ress=meshStorage::unflatten(pp1.vals_.spans[2]);
        //todo дело не в функции дело в меше
        compare2dArrays<true>(ress, r2, double_comparator3, 1e-3);
        compare2dArrays<true>(ress, mm2.getMeshdec()[2], double_comparator3, 1e-3);

    }
    in1.close();
    in2.close();
}


int main(int argc, char **argv) {//todo cmake+gtestmain
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);


    return RUN_ALL_TESTS();
}