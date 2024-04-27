#include "../application/lib.h"

#include <boost/test/unit_test.hpp>
#include "../application/TestRunner.h"
#include <boost/test/parameterized_test.hpp>
#include <iomanip>
#include <boost/array.hpp>
#include <boost/test/data/test_case.hpp>

#define BOOST_TEST_MODULE testRunnerTest

#define CHECK_CLOSE_COLLECTION(aa, bb, tolerance) { \
    using std::distance; \
    using std::begin; \
    using std::end; \
    auto a = begin(aa), ae = end(aa); \
    auto b = begin(bb); \
    BOOST_REQUIRE_EQUAL(distance(a, ae), distance(b, end(bb))); \
    for(; a != ae; ++a, ++b) { \
        BOOST_CHECK_CLOSE(*a, *b, tolerance); \
    } \
}
//https://stackoverflow.com/questions/3999644/how-to-compare-vectors-with-boost-test/17503436#17503436
BOOST_AUTO_TEST_SUITE( printUtils )
    constexpr double aRange=1e-7;

    //bool operator==(const T& a, const U& b);

    void CheckSolutions(std::vector<TestRunner::solution >&sol1,std::vector<TestRunner::solution >&sol2)
    {   //sol1[0][0].begin();
        for (int i = 0; i < sol1.size(); ++i) {
            for (int  dim = 0; dim < 2; ++dim) {
                BOOST_TEST_REQUIRE(sol1[i][dim]==sol2[i][dim],boost::test_tools::per_element() );
                //CHECK_CLOSE_COLLECTION(sol1[i][dim],sol2[i][dim],pow(10,-12));
                //BOOST_CHECK_EQUAL_COLLECTIONS(sol1[i][dim].cbegin(),sol1[i][dim].cend(),
                //                              sol2[i][dim].cbegin(),sol2[i][dim].cend());
            }
        }

    }


    BOOST_AUTO_TEST_CASE( test_1_dipole_solve,*boost::unit_test::tolerance(pow(10,-12)))
    {   int N=1;
        int Nsym=1;
        TestRunner testRunner(N,Nsym,aRange,"","",state_t::openmp_new);
        //std::vector<std::array<std::vector<FloatType>,2>>
        auto&CoordRef= testRunner.getCoordRef();
        CoordRef[0][0]=std::vector<FloatType>(N,0.0);
        CoordRef[0][1]=std::vector<FloatType>(N,0.0);

        testRunner.solve();
        std::vector<TestRunner::solution >mySol;
        mySol.resize(Nsym);
        mySol[0][0].resize(2);
        mySol[0][1].resize(2);
        mySol[0][0]<<0,-1e-22;
        mySol[0][1]<<1e-22,0;
        testRunner.generateFunction();
        auto baseSol= testRunner.getSolRef();
        CheckSolutions(baseSol,mySol);
    }

    array<vector<FloatType>, 2>parse(std::initializer_list<FloatType>&&one,std::initializer_list<FloatType>&&two)
    {
        return {std::vector<FloatType>(one),std::vector<FloatType>(two)};
    }

    array<Eigen::Vector<FloatType, -1>, 2>parse2(std::initializer_list<FloatType>&&one,std::initializer_list<FloatType>&&two)
    {
        Eigen::Vector<double, -1> temp{one};
        Eigen::Vector<double, -1> temp2{two};
        return {temp,temp2};
    }

    BOOST_AUTO_TEST_CASE( testAll2DipoleConfs,*boost::unit_test::tolerance(pow(10,-12)))
    {
        int N=2;
        int Nsym=14;
        auto ss={0.0,2.0};
        TestRunner testRunner(N,Nsym,aRange,"","",state_t::openmp_new);
        auto&CoordRef= testRunner.getCoordRef();
        CoordRef[0]=parse({0.0,1e-7},{0.0,0.0});
        CoordRef[1]=parse({-5e-8,5e-8},{0.0,0.0});
        CoordRef[2]=parse({0.0,0.0},{0.0,1e-7});
        CoordRef[3]=parse({0.0,0.0},{-5e-8,5e-8});
        CoordRef[4]=parse({6.07E-08,1.31E-07},{9.07E-08,1.61E-07});
        CoordRef[5]=parse({-6.07E-08,-1.31E-07},{-9.07E-08,-1.61E-07});
        CoordRef[6]=parse({4.33E-09,1.87E-08},{-7.90E-08,-1.78E-07});
        CoordRef[7]=parse({-4.33E-09,-1.87E-08},{7.90E-08,1.78E-07});
        CoordRef[8]=parse({0.0,0.0},{-3e-8,7e-8});
        CoordRef[9]=parse({-1.0,-1.0},{-7e-8,3e-8});
        CoordRef[10]=parse({1.0,1.0},{-3e-8,7e-8});
        CoordRef[11]=parse({0.0,0.0},{-7e-8,3e-8});
        CoordRef[12]=parse({1.87E-08,4.33E-09},{-1.78E-07,-7.90E-08});
        CoordRef[13]=parse({-1.87E-08,-4.33E-09},{1.78E-07,7.90E-08});

        testRunner.solve();
        std::vector<TestRunner::solution >mySol;
        mySol.resize(Nsym);
        mySol[0][0].resize(2);
        mySol[0][1].resize(2);
        Eigen::Vector<double, -1> temp{{0.0, -1e-22}};
        mySol[0]= parse2({0.0, -1e-22},{0.0, -1e-22});
        testRunner.generateFunction();
        auto baseSol= testRunner.getSolRef();

        CheckSolutions(baseSol,mySol);
    }

BOOST_AUTO_TEST_SUITE_END()
