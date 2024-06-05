//
// Created by Lenovo on 05.06.2024.
//

#ifndef DIPLOM_GENERATOR_H
#define DIPLOM_GENERATOR_H
#include <array>
#include <vector>
#include <random>
#include <functional>
template<typename T>
class Generator {
public:
    virtual std::array<std::vector<T>, 2> generate() = 0;

   // Generator(Generator &) = delete;

   // Generator(Generator &&) = delete;

   // Generator &operator=(Generator &) = delete;

    //Generator &operator=(Generator &&) = delete;
};



template<typename T>//todo напоминает по вещам лямбды, но не совсем
class GausGenerator: public Generator<T>{
public:
    GausGenerator(T mean, T stddev,size_t N) : mean_(mean), stddev_(sqrt(2) * stddev) {
        distribution_ = std::normal_distribution<T>(mean_, stddev_);
        N_=N;
    }

    std::array<std::vector<T>, 2> generate() override {
        if (!N_) {
            return std::array<std::vector<T>, 2>();
        }
        std::array<std::vector<T>, 2> res;
        res[0] = std::vector<T>(N_, 0);
        res[1] = std::vector<T>(N_, 0);
        std::function<T()> generetor = [&]() { return distribution_(rng_); };
        //std::cout<<generetor()<<"\t"<<distribution_(rng_)<<"\n";
        std::generate(res[0].begin(), res[0].end(), generetor);
        std::generate(res[1].begin(), res[1].end(), generetor);
        return res;
    }



private:
    T mean_;
    T stddev_;
    std::random_device rd_;
    std::mt19937 rng_ = std::mt19937(rd_());
    std::normal_distribution<T> distribution_;
    size_t N_;
};

template<typename T>//todo grid generator with features
class TriangGenerator: public Generator<T>{
public:

    TriangGenerator( T a, T b, T l, T r, bool center) :  a(a), b(b), l(l), r(r), center(center) {}

    std::array<std::vector<T>, 2> generate() override {
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

private:
    static constexpr T c1 = 1 / 2.0;
    static constexpr T c2 = T(0.86602540378443864676372317075293618347140262690519031402790348972596650845440001854057309337862428783781307070770335151498497254749947623940582775604718682426404661595115279103398741005054233746163251);
    T a = 0;
    T b = 0;
    T l;
    T r;
    bool center = false;

};


#endif //DIPLOM_GENERATOR_H
