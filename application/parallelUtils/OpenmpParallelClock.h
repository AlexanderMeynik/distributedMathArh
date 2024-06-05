//
// Created by Lenovo on 23.04.2024.
//
#include <omp.h>
#include <array>
#include <ranges>
#include <limits>
#include <algorithm>
#include <numeric>
#include <list>
#include <cmath>
#ifndef DIPLOM_OPENMPPARRALELCLOCK_H
#define DIPLOM_OPENMPPARRALELCLOCK_H

class OpenmpParallelClock {
public:
    OpenmpParallelClock();
    double getTime() {
        double sumValueA = 0.0;
        int num=0;
        for (auto elme:timers) {
            if(std::abs(elme)>std::numeric_limits<double>::epsilon()) {
                sumValueA += elme;
                num++;
            }
        }
        return sumValueA  / (1.0*num);
    }

    template<typename T,double init>
    T aggregate(std::function<T(T,T)>&& aggr)
    {
        //подумать https://jscheiny.github.io/Streams/
       return std::accumulate(timers.begin(), timers.end(), init, aggr);
    }
    void tik();
    void tak();
private:
    static bool inInParallel();
    std::vector<double> timers;
    std::vector<double> startIngTimers;
};




#endif //DIPLOM_OPENMPPARRALELCLOCK_H
