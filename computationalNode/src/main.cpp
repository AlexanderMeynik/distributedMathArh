#include <iostream>

#include <vector>
#include <algorithm>
#include <numeric>
#include <ostream>
#include <fstream>
#include <iomanip>
#include <filesystem>

#include <boost/math/quadrature/gauss_kronrod.hpp>
#include <matplot/matplot.h>
#include "common/typeCasts.h"
#include "computationalLib/math_core/Dipoles.h"

const std::size_t maxPrecision = std::numeric_limits<double>::digits;


using namespace dipoles;
const double l = 1E-7;

int main(int argc, char *argv[]) {
    std::ios_base::sync_with_stdio(false);
    int verbose = 2;
    std::string filename = "config.txt";
    if (argc == 2) {
        filename = argv[1];
    }
    if (argc == 3) {
        char *end;
        verbose = std::strtol(argv[2], &end, 10);
    }
    auto avec = parseConf2<double>(filename);
    std::string dirname = filename.erase(filename.find('.'));
    std::filesystem::create_directory(dirname);
    for (int i = 0; i < avec.size(); ++i) {
        std::array<std::vector<double>, 2> a = avec[i];
        int N = a[0].size();
        Dipoles d(N, a);

        auto solut2 = d.solve3();//todo?
        //printToFile<double>(N, a, d, dirname,i,verbose);
    }

    return 0;
}








