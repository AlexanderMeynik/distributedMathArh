

#ifndef MAGISTER1_LIB_H
#define MAGISTER1_LIB_H


#include <vector>

#include <iomanip>
#include <filesystem>
#include <random>
#include <map>
#include <cassert>
#include <eigen3/Eigen/Core>

//todo where do we use this file
// do we actually need it and where
using std::function, std::pair, std::vector, std::array;
using std::string;


template<class T>
std::array<std::vector<T>, 2> reinterpretVector(Eigen::Vector<T, Eigen::Dynamic> &xi) {
    auto N = xi.size() / 2;
    if (!N) {
        return std::array<std::vector<T>, 2>();
    }
    std::array<std::vector<T>, 2> res;
    res[0] = std::vector<T>(N, 0);
    res[1] = std::vector<T>(N, 0);

    for (int i = 0; i < N; ++i) {
        res[0][i] = xi[i];
        res[1][i] = xi[i + N];
    }
    return res;
}

enum class state_t {
    new_,
    old,
    openmp_new,
    openmp_old,
    print_
};


const static std::map<state_t, std::string> stateToString = {
        {state_t::openmp_new, "openmp_new"},
        {state_t::new_,       "new"},
        {state_t::openmp_old, "openmp_old"},
        {state_t::old,        "old"},
        {state_t::print_,     "print"},
};
const static std::map<std::string, state_t> stringToState = {
        {"openmp_new", state_t::openmp_new},
        {"new",        state_t::new_},
        {"openmp_old", state_t::openmp_old},
        {"old",        state_t::old},
        {"print",      state_t::print_},
};
std::ostream& operator<<  (std::ostream &out,const state_t&st);

#endif //MAGISTER1_LIB_H
