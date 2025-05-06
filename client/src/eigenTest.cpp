
#include <random>

#include "../include/plotingUtils.h"

auto aRange = 1.0e-6;

int main(int argc, char *argv[]) {
  std::vector<double> a = {0, 1, 6, 1, 0, -6};
  PlotCoordinates("result.png", 1, a);

}