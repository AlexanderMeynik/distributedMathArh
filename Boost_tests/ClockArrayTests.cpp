#include "../application/common/lib.h"
#include <boost/test/unit_test.hpp>
#include "../application/common/printUtils.h"
#include "../application/parallelUtils/OpenmpParallelClock.h"
#include <iomanip>
#include <omp.h>
#include <chrono>
#include <thread>
#include "../application/parallelUtils/clockArray.h"
#include "../application/math_core/TestRunner.h"

BOOST_AUTO_TEST_SUITE(clockTest)

    template<size_t size, typename COUNTED>
    using clock1 = clockArray<size, double, COUNTED>;

    constexpr double sleep_sec = 0.1;
    constexpr size_t iterations = 12;
    constexpr double tolerance = 1;

    BOOST_AUTO_TEST_CASE(test_array) {
        size_t N = 100;
        double aRange = 1e-6;

        TestRunner ts(N, N, aRange);
        clock1<3, TestRunner> cl1(ts);

        // If generateGeneralized needs to be called as well, ensure it’s called with proper parameters
        using GenFuncType = std::function<std::array<std::vector<double>, 2>(size_t, double, double)>;
        GenFuncType genFunc = generators::uniform_real<double>;
        ts.generateGeneralized(genFunc, N, 0.0, aRange * sqrt(2));

        // Perform timed call to solve function
        cl1.perform_withTimeCalc<1, &TestRunner::solve>();
        cl1.perform_withTimeCalc<2,&TestRunner::generateFunction>();

        // Print the metrics
        cl1.printMetrics(std::cout);
    }

BOOST_AUTO_TEST_SUITE_END()
