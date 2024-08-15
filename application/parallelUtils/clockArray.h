//
// Created by Lenovo on 05.06.2024.
//

#ifndef DIPLOM_CLOCKARRAY_H
#define DIPLOM_CLOCKARRAY_H

#include <iostream>
#include <cstddef>
#include <array>
#include <functional>
#include "OpenmpParallelClock.h"

template<typename ...Args>
struct functable {
    std::function<void(Args...)> func;
    const char *name;
};
//todo может мы его и уберём
template<size_t size, typename Tr, typename COUNTED>
class clockArray {
public:
    using CLOCK = timing::OpenmpParallelClock<Tr, &omp_get_wtime, &omp_get_thread_num>;

    explicit clockArray(COUNTED &cm) : cmd(&cm) {
        for (int i = 0; i < size; ++i) {
            clocks_[i] = CLOCK(std::to_string(i));
        }
    };

    template<size_t INDEX/*,typename ... Args*/, void (COUNTED::*action)(/*Args ...*/)>
    void perform_withTimeCalc(/*Args ... args*/) {
        static_assert(INDEX < size);
        clocks_[INDEX].tik();
        (*cmd.*action)(/*args...*/);
        clocks_[INDEX].tak();
    };

    void printMetrics(std::ostream &out) {
        for (int i = 0; i < size; ++i) {
            out/*<<clock_names.find(i)->second<<'\t'*/<< clocks_[i] << "\n";
        }
        //std::cout<<N_.value()<<'\t'<<Nsym_.value()<<'\n';
    }

private:
    std::array<CLOCK, size> clocks_;  // Array of clock objects
    COUNTED *cmd;  // Pointer to the command object
};


#endif //DIPLOM_CLOCKARRAY_H
