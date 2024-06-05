#include "../application/common/lib.h"

#include <boost/test/unit_test.hpp>
#include "../application/math_core/TestRunner.h"
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
BOOST_AUTO_TEST_SUITE(testRunnerTest)
    constexpr double aRange = 1e-7;


    void CheckSolutions(std::vector<solution> &sol1,
                        std::vector<solution> &sol2) {   //sol1[0][0].begin();
        for (int i = 0; i < sol1.size(); ++i) {
            for (int dim = 0; dim < 2; ++dim) {
                BOOST_TEST_REQUIRE(sol1[i][dim] == sol2[i][dim], boost::test_tools::per_element());
               }
        }

    }


    BOOST_AUTO_TEST_CASE(test_1_dipole_solve, *boost::unit_test::tolerance(pow(10, -12)))
    {
        int N = 1;
        int Nsym = 1;
        TestRunner testRunner(N, Nsym, aRange, "", "", state_t::openmp_new);
        //std::vector<std::array<std::vector<FloatType>,2>>
        auto &CoordRef = testRunner.getCoordRef();
        CoordRef[0][0] = std::vector<FloatType>(N, 0.0);
        CoordRef[0][1] = std::vector<FloatType>(N, 0.0);

        testRunner.solve();
        std::vector<solution> mySol;
        mySol.resize(Nsym);
        mySol[0][0].resize(2);
        mySol[0][1].resize(2);
        mySol[0][0] << 0, -1e-22;
        mySol[0][1] << 1e-22, 0;
        testRunner.generateFunction();
        auto baseSol = testRunner.getSolRef();
        CheckSolutions(baseSol, mySol);
    }

    array<vector<FloatType>, 2> parse(std::initializer_list<FloatType> &&one, std::initializer_list<FloatType> &&two) {
        return {std::vector<FloatType>(one), std::vector<FloatType>(two)};
    }

    array<Eigen::Vector<FloatType, -1>, 2>
    parse2(std::initializer_list<FloatType> &&one, std::initializer_list<FloatType> &&two) {
        Eigen::Vector<double, -1> temp{one};
        Eigen::Vector<double, -1> temp2{two};
        return {temp, temp2};
    }


    array<Eigen::Vector<FloatType, -1>, 2> parse2(std::initializer_list<FloatType> &&ls) {
        int size = ls.size();

        if (!size % 2) {
            return {};
        }
        Eigen::Vector<double, -1> onel, oner;
        onel.resize(size / 2);
        oner.resize(size / 2);
        for (int i = 0; i < size / 2; ++i) {
            onel.coeffRef(i) = data(ls)[i];
            oner.coeffRef(i) = data(ls)[i + size / 2];
        }
        return {onel, oner};
    }

    BOOST_AUTO_TEST_CASE(testAll2DipoleConfs, *boost::unit_test::tolerance(1e-5))
    {
        int N = 2;

        int Nsym = 14;
        TestRunner testRunner(N, Nsym, aRange, "", "", state_t::openmp_new);
        auto &CoordRef = testRunner.getCoordRef();
        CoordRef[0] = parse({0.0, 1e-7}, {0.0, 0.0});
        CoordRef[1] = parse({-5e-8, 5e-8}, {0.0, 0.0});
        CoordRef[2] = parse({0.0, 0.0}, {0.0, 1e-7});
        CoordRef[3] = parse({0.0, 0.0}, {-5e-8, 5e-8});
        CoordRef[4] = parse({6.07E-08, 1.31E-07}, {9.07E-08, 1.61E-07});
        CoordRef[5] = parse({-6.07E-08, -1.31E-07}, {-9.07E-08, -1.61E-07});
        CoordRef[6] = parse({4.33E-09, 1.87E-08}, {-7.90E-08, -1.78E-07});
        CoordRef[7] = parse({-4.33E-09, -1.87E-08}, {7.90E-08, 1.78E-07});
        CoordRef[8] = parse({0.0, 0.0}, {-3e-8, 7e-8});
        CoordRef[9] = parse({-1.0, -1.0}, {-7e-8, 3e-8});
        CoordRef[10] = parse({1.0, 1.0}, {-3e-8, 7e-8});
        CoordRef[11] = parse({0.0, 0.0}, {-7e-8, 3e-8});
        CoordRef[12] = parse({1.87E-08, 4.33E-09}, {-1.78E-07, -7.90E-08});
        CoordRef[13] = parse({-1.87E-08, -4.33E-09}, {1.78E-07, 7.90E-08});

        testRunner.solve();
        std::vector<solution> mySol;
        mySol.resize(Nsym);

        mySol[0] = parse2(
                {-4.02101e-24, -4.98918e-23, -4.02101e-24, -4.98918e-23, 4.73471e-23, 2.63096e-24, 4.73471e-23,
                 2.63096e-24});
        mySol[1] = parse2(
                {-4.02101e-24, -4.98918e-23, -4.02101e-24, -4.98918e-23, 4.73471e-23, 2.63096e-24, 4.73471e-23,
                 2.63096e-24});
        mySol[2] = parse2({2.63096e-24, -4.73471e-23, 2.63096e-24, -4.73471e-23, 4.98918e-23, -4.02101e-24, 4.98918e-23,
                           -4.02101e-24});
        mySol[3] = parse2({2.63096e-24, -4.73471e-23, 2.63096e-24, -4.73471e-23, 4.98918e-23, -4.02101e-24, 4.98918e-23,
                           -4.02101e-24});
        mySol[4] = parse2({5.81294e-25, -5.19813e-23, 5.81294e-25, -5.19813e-23, 4.52138e-23, -1.99773e-24, 4.52138e-23,
                           -1.99773e-24});
        mySol[5] = parse2({5.81294e-25, -5.19813e-23, 5.81294e-25, -5.19813e-23, 4.52138e-23, -1.99773e-24, 4.52138e-23,
                           -1.99773e-24});
        mySol[6] = parse2({2.12958e-24, -4.64575e-23, 2.12958e-24, -4.64575e-23, 5.07842e-23, -3.51794e-24, 5.07842e-23,
                           -3.51794e-24});
        mySol[7] = parse2({2.12958e-24, -4.64575e-23, 2.12958e-24, -4.64575e-23, 5.07842e-23, -3.51794e-24, 5.07842e-23,
                           -3.51794e-24});
        mySol[8] = parse2({2.63096e-24, -4.73471e-23, 2.63096e-24, -4.73471e-23, 4.98918e-23, -4.02101e-24, 4.98918e-23,
                           -4.02101e-24});
        mySol[9] = parse2({2.63096e-24, -4.73471e-23, 2.63096e-24, -4.73471e-23, 4.98918e-23, -4.02101e-24, 4.98918e-23,
                           -4.02101e-24});
        mySol[10] = parse2(
                {2.63096e-24, -4.73471e-23, 2.63096e-24, -4.73471e-23, 4.98918e-23, -4.02101e-24, 4.98918e-23,
                 -4.02101e-24});
        mySol[11] = parse2(
                {2.63096e-24, -4.73471e-23, 2.63096e-24, -4.73471e-23, 4.98918e-23, -4.02101e-24, 4.98918e-23,
                 -4.02101e-24});
        mySol[12] = parse2(
                {2.12958e-24, -4.64575e-23, 2.12958e-24, -4.64575e-23, 5.07842e-23, -3.51794e-24, 5.07842e-23,
                 -3.51794e-24});
        mySol[13] = parse2(
                {2.12958e-24, -4.64575e-23, 2.12958e-24, -4.64575e-23, 5.07842e-23, -3.51794e-24, 5.07842e-23,
                 -3.51794e-24});
        testRunner.generateFunction();
        auto baseSol = testRunner.getSolRef();

        CheckSolutions(baseSol, mySol);
    }



    BOOST_AUTO_TEST_CASE(test_generate, *boost::unit_test::tolerance(pow(10, -12)))
    {
        size_t N = 2;
        int Nsym = 4;
        TestRunner testRunner(N, Nsym, aRange, "", "", state_t::openmp_new);
        testRunner.generateGeneralized(generators::Gaus<double>,N,0.0,aRange*sqrt(2));
        //testRunner.generateGaus();
        //std::vector<std::array<std::vector<FloatType>,2>>

        testRunner.solve();
        testRunner.generateFunction();
        auto baseSol = testRunner.getSolRef();
        BOOST_CHECK_EQUAL(baseSol[0][0].size(),N*2);
        BOOST_CHECK_EQUAL(baseSol.size(),Nsym);
        //todo check multip;
        //CheckSolutions(baseSol, mySol);
    }

BOOST_AUTO_TEST_SUITE_END()



