//
// Created by Lenovo on 23.04.2024.
//
#include <omp.h>
#include <array>
#include <ranges>
#include <limits>
#include <algorithm>
#include <numeric>

#ifndef DIPLOM_OPENMPPARRALELCLOCK_H
#define DIPLOM_OPENMPPARRALELCLOCK_H

class OpenmpParallelClock {
public:
    OpenmpParallelClock();
    double getTime();
    void tik();
    void tak();
private:
    static bool inInParallel();
    std::vector<double> timers;
    std::vector<double> startIngTimers;
};




#endif //DIPLOM_OPENMPPARRALELCLOCK_H
