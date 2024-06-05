#include "OpenmpParallelClock.h"


void OpenmpParallelClock::tak() {
    size_t id=omp_get_thread_num();
    timers[id]+=omp_get_wtime()-startIngTimers[id];
}

void OpenmpParallelClock::tik() {
    startIngTimers[omp_get_thread_num()]=omp_get_wtime();
}



bool OpenmpParallelClock::inInParallel() {
    return omp_get_thread_num()!=0;
}

OpenmpParallelClock::OpenmpParallelClock() {
    timers=std::vector<double>(omp_get_max_threads(),0.0);
    startIngTimers=std::vector<double>(omp_get_max_threads(),0.0);
}
