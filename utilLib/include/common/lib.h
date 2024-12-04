

#ifndef MAGISTER1_LIB_H
#define MAGISTER1_LIB_H


#include <vector>

#include <iomanip>
#include <filesystem>
#include <random>
#include <map>
#include <cassert>
#include <eigen3/Eigen/Core>

//todo delete some things
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

/*
template<typename T>
class CoordGenerator {
public:
    CoordGenerator(T mean, T stddev) : mean_(mean), stddev_(sqrt(2) * stddev) {
        distribution_ = std::normal_distribution<T>(mean_, stddev_);
    }

    [[deprecated("use generateCoordinates2")]]std::array<std::vector<T>, 2> generateCoordinates(size_t N) {
        if (!N) {
            return std::array<std::vector<T>, 2>();
        }
        std::array<std::vector<T>, 2> res;
        res[0] = std::vector<T>(N, 0);
        res[1] = std::vector<T>(N, 0);
        std::function<T()> generetor = [&]() { return distribution_(rng_); };
        //std::cout<<generetor()<<"\t"<<distribution_(rng_)<<"\n";
        std::generate(res[0].begin(), res[0].end(), generetor);
        std::generate(res[1].begin(), res[1].end(), generetor);
        return res;
    }




    //todo remake
    Eigen::Vector<T, Eigen::Dynamic> generateCoordinates2(size_t N) {
        if (!N) {
            return Eigen::Vector<T, Eigen::Dynamic>();
        }
        Eigen::Vector<T, Eigen::Dynamic> res(2 * N);

        std::function<T()> generetor = [&]() { return distribution_(rng_); };

        std::generate(res.begin(), res.end(), generetor);
        return res;
    }


    std::array<std::vector<T>, 2> triangularGrid(T l, T r, T a = 0, T b = 0, bool center = false) {

        T k = c2 / c1;

        T D = sqrt(k * k * r * r / (k * k + 1));//здесь мы находим x координаты для точек пересечения
        T x_t[2] = {-D, D};
        T y_t[2];
        for (int i = 0; i < 2; ++i) {
            y_t[i] = -(x_t[i]) / k;//подставляем их в исходный код
        }
        T c_t[2];

        for (int i = 0; i < 2; ++i) {
            c_t[i] = y_t[i] - x_t[i] * k;
        }
        int l_t[2];

        for (int i = 0; i < 2; ++i) {
            l_t[i] = -(c_t[i]) / (k * l);
        }


        //todo 2 круг симметричная фигура-> можно подсчитать результаты в 1 четверти
        //а что будет если поместить треугольники так, чробы центр центрального был в центре круга
        if (center) {
            l_t[0]--;
            //l_t[1];
        }
        int funccount = std::abs(l_t[1] - l_t[0]) + 1;
        std::vector<T> x1 = myLinspace(l_t[0] * l, l_t[1] * l, funccount);
        std::vector<T> y1(funccount, 0);

        std::vector<T> x(funccount * funccount, 0);
        std::vector<T> y(funccount * funccount, 0);

        for (int i = 0; i < funccount; ++i) {
            for (int j = 0; j < funccount; ++j) {

                x[j + i * funccount] = x1[i] / 2 + x1[j] / 2 + (c1 / c2) * (y1[i] + y1[j]) / 2;
                y[j + i * funccount] = (-c2 / c1) * x[j + i * funccount] + (c2 / c1) * x1[i] - y1[i];

                x[j + i * funccount] += a;
                y[j + i * funccount] += b;

            }
        }

        if (center) {
            for (int i = 0; i < funccount; ++i) {
                for (int j = 0; j < funccount; ++j) {

                    x[j + i * funccount] += l / 2.0;
                    y[j + i * funccount] += l * c2 / 2.0;

                }
            }
        }


        std::vector<T> x_filtered, y_filtered;

        for (int i = 0; i < funccount * funccount; ++i) {
            if (std::sqrt(pow(x[i] - a, 2) + pow(y[i] - b, 2)) <= r) {
                x_filtered.push_back(x[i]);
                y_filtered.push_back(y[i]);
            }
        }
        //std::cout<<x_filtered.size()<<'\t'<<x.size()<<'\t'<<x_filtered.size()/(1.0*x.size())<<"\n";
        return {x_filtered, y_filtered};
    }

    CoordGenerator(CoordGenerator &) = delete;

    CoordGenerator(CoordGenerator &&) = delete;

    CoordGenerator &operator=(CoordGenerator &) = delete;

    CoordGenerator &operator=(CoordGenerator &&) = delete;

private:
    T mean_;
    T stddev_;
    std::random_device rd_;
    std::mt19937 rng_ = std::mt19937(rd_());
    std::normal_distribution<T> distribution_;
    long double c1 = 1 / 2.0;
    long double c2 = sqrt(3) / 2.0;
};*/

enum class state_t {
    openmp_new,
    new_,
    openmp_old,
    old,
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


#endif //MAGISTER1_LIB_H
