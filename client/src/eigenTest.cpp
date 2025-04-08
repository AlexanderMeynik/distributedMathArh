
#include <random>


#include "parallelUtils/chronoClock.h"
#include "../include/plotingUtils.h"

auto aRange = 1.0e-6;

using chronoClock::gClk;

int main(int argc, char *argv[]) {
    std::vector<double> a = {0, 1, 6, 1, 0, -6};
    plotCoordinates("result.png", 1, a);

}