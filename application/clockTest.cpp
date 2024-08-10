#include "parallelUtils/OpenmpParallelClock.h"
#include <unistd.h>
#include <iostream>

int main(int argc, char *argv[]) {
    OpenmpParallelClock clock1;

    auto timers = std::vector<double>(omp_get_max_threads(), 0.0);
    auto startIngTimers = std::vector<double>(omp_get_max_threads(), 0.0);
    int sleep_sec = 1;
    {
        //#pragma omp parallel private(sleep_sec,clock1) num_threads(1) default(none)
        {
#pragma omp parallel for shared(sleep_sec) num_threads(omp_get_max_threads())
            for (int i = 0; i < omp_get_max_threads() * 2; ++i) {
                int id = omp_get_thread_num();
                startIngTimers[id] = omp_get_wtime();
                clock1.tik();
                // #pragma omp critical
                {
                    printf("Thread %d is sleeping for 1 second\n", omp_get_thread_num());
                    sleep(sleep_sec); // Sleep for 1 second
                }
                clock1.tak();
                timers[id] += omp_get_wtime() - startIngTimers[id];
            }
        }
    }
    double a = clock1.getTime();
    std::cout << clock1.getTime() << "\t" << a;

}