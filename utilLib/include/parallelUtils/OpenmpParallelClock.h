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
#include <iostream>
#include <cmath>

#ifndef DIPLOM_OPENMPPARRALELCLOCK_H
#define DIPLOM_OPENMPPARRALELCLOCK_H

namespace timing {
    template<typename T>
    struct value_func_pair {
        T value;
        std::function<T(T, T)> f;
    };
    template<typename T>
    value_func_pair<T> max{-1.0, [](T a, T b) { return std::max(a, b); }};

    template<typename T, T(*timeGetter)(), int(*threadNumGetter)()>
    class OpenmpParallelClock {
    public:
        //todo почитать про parameter forwarding и сделать более общей
        explicit OpenmpParallelClock(const std::string &name = "anon") {
            timers = std::vector<T>(omp_get_max_threads(), T(0.0));
            startIngTimers = std::vector<T>(omp_get_max_threads(), T(0.0));
            name_ = name;
        }

        T getTime() {
            T sumValueA = 0.0;
            int num = 0;
            for (auto elme: timers) {
                if (std::abs(elme) > std::numeric_limits<T>::epsilon()) {
                    sumValueA += elme;
                    num++;
                }
            }
            return sumValueA / (1.0 * num);
        }

        template<typename Tp, typename BinaryOperation>
        Tp aggregate(Tp init,
                     BinaryOperation binary_op) {
            return std::accumulate(timers.begin(), timers.end(), init, binary_op);
        }

        template<typename Tp>
        Tp aggregate() {
            return std::accumulate(timers.begin(), timers.end(), max<Tp>.value, max<Tp>.f);
        }

        void tak() {
            size_t id = (*threadNumGetter)();
            timers[id] += (*timeGetter)() - startIngTimers[id];
        }

        void tik() {
            startIngTimers[(*threadNumGetter)()] = (*timeGetter)();
        }

        friend std::ostream &operator<<(std::ostream &out, OpenmpParallelClock<T, timeGetter, threadNumGetter> &clk) {
            out << clk.name_ << '\t' << clk.template aggregate<T>();
            return out;
        }

        template<class StringType>
        friend StringType to_string(std::ostream &out, OpenmpParallelClock<T, timeGetter, threadNumGetter> &clk) {
            std::basic_stringstream<StringType> ss;
            ss << clk.name_ << '\t' << clk.template aggregate<T>();//type deduction?
            return ss.str();
        }

    private:
        bool inInParallel() {
            return omp_get_thread_num() != 0;
        }

        std::vector<T> timers;
        std::vector<T> startIngTimers;
        std::string name_;
    };
}


#endif //DIPLOM_OPENMPPARRALELCLOCK_H
