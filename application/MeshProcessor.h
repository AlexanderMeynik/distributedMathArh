//
// Created by Lenovo on 28.03.2024.
//

#ifndef DIPLOM_MESHPROCESSOR_H
#define DIPLOM_MESHPROCESSOR_H

#include <matplot/matplot.h>
#include <ostream>
#include <vector>
#include <boost/math/quadrature/gauss_kronrod.hpp>
#include "printUtils.h"


template<class T, unsigned N>
T integrate(std::function<T(T)> &f1, T left, T right, unsigned int max_depth = 5, T tol = 1e-20) {
    T error;
    T Q = boost::math::quadrature::gauss_kronrod<T, N>::integrate(f1, left, right, max_depth, tol, &error);
    return Q;
}

template<class T, unsigned N>
T integrate(const std::function<T(T)> &&f1, T left, T right, unsigned int max_depth = 5, T tol = 1e-20) {
    T error;
    double Q = boost::math::quadrature::gauss_kronrod<T, N>::integrate(f1, left, right, max_depth, tol, &error);
    return Q;
}

template<class T, unsigned N>
T integrateFunctionBy1Val(const std::function<T(T, T, T)> &ff, T theta, T phi, T left, T right,
                          unsigned int max_depth = 5, T tol = 1e-20) {
    std::function<T(T)> tt = [&theta, &phi, &ff](T t) { return ff(theta, phi, t); };
    return integrate<T, N>(tt, left, right, max_depth, tol);
}

template<class T>
T getMeshDiffNorm(std::vector<std::vector<T>> &mesh1, std::vector<std::vector<T>> &mesh2);


template<typename T>
void addMesh(std::vector<std::vector<T>> &a, const std::vector<std::vector<T>> &b);

template<typename T>
void addMesh(std::vector<std::vector<T>> &a, const std::vector<std::vector<T>> &b) {
    size_t size2 = a[0].size();
    for (int i = 0; i < a.size(); ++i) {
        for (int j = 0; j < size2; ++j) {
            a[i][j] += b[i][j];
        }
    }
}


using namespace matplot;

template<typename T>
std::pair<std::vector<std::vector<T>>, std::vector<std::vector<T>>> mymeshGrid(std::vector<T> &&a, std::vector<T> &&b);


template<typename T>
std::vector<T> myLinspace(T lower_bound, T upper_bound, size_t n, bool end = true);

template<typename T>
std::vector<T> myLinspace(T lower_bound, T upper_bound, size_t n, bool end) {
    std::vector<T> result(n, T());

    T div = (end) ? (n - 1) : n;
    T step = (upper_bound - lower_bound) / div;

    for (int i = 0; i < n; ++i) {
        result[i] = i * step + lower_bound;
    }
    return result;
}


template<typename T>
std::pair<std::vector<std::vector<T>>, std::vector<std::vector<T>>> mymeshGrid(std::vector<T> &&a, std::vector<T> &&b) {
    std::vector<std::vector<T>> x_mesh(b.size(), std::vector<T>(a.size()));
    std::vector<std::vector<T>> y_mesh(b.size(), std::vector<T>(a.size()));
    for (size_t i = 0; i < b.size(); ++i) {
        x_mesh[i] = a;
        for (size_t j = 0; j < a.size(); ++j) {
            y_mesh[i][j] = b[i];
        }
    }

    return std::make_pair(x_mesh, y_mesh);
}

template<typename T>
class MeshProcessor {
public:
    MeshProcessor() {
        initCoordMeshes();
    }

    void setSteps(std::pair<T, T> &elems) {
        steps[0] = elems.first;
        steps[1] = elems.second;
    }

    const std::array<std::vector<std::vector<T>>, 3> &getMeshdec() const {
        return meshdec;
    }

    std::vector<std::vector<T>> getMeshGliff() {
        return std::vector<std::vector<T>>(nums[1], std::vector<T>(nums[0], 0.0));
    }

    const std::array<std::vector<std::vector<T>>, 3> &getMeshsph() const {
        return meshsph;
    }

    void generateMeshes(const std::function<T(T, T, T)> &func);

    void generateNoInt(const std::function<T(T, T)> &func);

    void plotSpherical(std::string filename);

    void printDec(std::ostream &out);

    void setMesh3(std::vector<std::vector<T>> &val);

private:
    void initCoordMeshes();

    void sphericalTransformation();

    std::array<std::vector<std::vector<T>>, 3> meshdec;
    std::array<std::vector<std::vector<T>>, 3> meshsph;

    static constexpr T philims[2] = {0, M_PI * 2};
    static constexpr T thelims[2] = {0, M_PI_2};
    static constexpr T steps[2] = {M_PI / 12, M_PI / 120};
    static constexpr T nums[2] = {(philims[1] - philims[0]) / (steps[0]) + 1, (thelims[1] - thelims[0]) / (steps[1]) + 1};
    static constexpr const T omega = 1e15;
    static constexpr const T rr = 2 * M_PI / omega;
    static constexpr const T step = M_PI / 12;
};

template<typename T>
void MeshProcessor<T>::initCoordMeshes() {
    std::pair<std::vector<std::vector<T>>, std::vector<std::vector<T>>> meshgrid1 = mymeshGrid(
            myLinspace(philims[0], philims[1], nums[0]), myLinspace(thelims[0], thelims[1], nums[1]));
    meshdec[0] = meshgrid1.first;
    meshdec[1] = meshgrid1.second;
}

template<typename T>
void MeshProcessor<T>::setMesh3(std::vector<std::vector<T>> &val) {
    meshdec[2] = val;
    sphericalTransformation();
}


template<typename T>
void MeshProcessor<T>::printDec(std::ostream &out) {
    out << "Функция I(phi,th)\n";
    out << "phi\\th\t\t";
    for (T theta = thelims[0]; theta < thelims[1]; theta += steps[1]) {//todo не перепуктали ли мы аргументы
        out << scientificNumber(theta, 5) << "\t";
    }
    out << "\n";
    int i1 = 0;
    int i2;
    for (T phi = philims[0]; phi < philims[1]; phi += steps[0]) {
        out << scientificNumber(phi, 5) << "\t";
        i2 = 0;
        i1++;
        for (T theta = thelims[0]; theta < thelims[1]; theta += steps[1]) {
            out << scientificNumber(meshdec[2][i2][i1], 5) << "\t";
            i2++;
        }
        out << "\n";
    }
}


template<typename T>
void MeshProcessor<T>::plotSpherical(std::string filename) {
    auto ax = gca();
    ax->surf(meshsph[0], meshsph[1], meshsph[2])
            ->lighting(true).primary(0.8f).specular(0.2f);;//-> view(213,22)->xlim({-40,40})->ylim({-40,40});
    //surf(x, y, z);
    ax->view(213, 22);
    ax->xlim({-40, 40});
    ax->ylim({-40, 40});
    ax->zlim({0, 90});

    matplot::save(filename);
    //ax->clear();
    ax.reset();
}

template<typename T>
void MeshProcessor<T>::generateNoInt(const std::function<T(T, T)> &func) {
    T rr1 = this->rr;

    meshdec[2] = transform(meshdec[0], meshdec[1], func);
    sphericalTransformation();
}

template<typename T>
void MeshProcessor<T>::sphericalTransformation() {
    this->meshsph[0] = this->meshdec[0];
    this->meshsph[1] = this->meshdec[1];
    this->meshsph[2] = this->meshdec[2];
    for (int i = 0; i < this->meshsph[0].size(); ++i) {
        for (int j = 0; j < this->meshsph[0][0].size(); ++j) {
            this->meshsph[0][i][j] = this->meshsph[2][i][j] * sin(this->meshdec[1][i][j]) * cos(this->meshdec[0][i][j]);
            this->meshsph[1][i][j] = this->meshsph[2][i][j] * sin(this->meshdec[1][i][j]) * sin(this->meshdec[0][i][j]);
            this->meshsph[2][i][j] = this->meshsph[2][i][j] * cos(this->meshdec[1][i][j]);
        }
    }
}

template<typename T>
void MeshProcessor<T>::generateMeshes(const std::function<T(T, T, T)> &func) {

    T rr1 = this->rr;

    meshdec[2] = transform(meshdec[0], meshdec[1], [&func, &rr1](T x, T y) {
        return integrateFunctionBy1Val<T, 61>(func, y, x, 0, rr1);
    });

    sphericalTransformation();
}


template<class T>
T getMeshDiffNorm(std::vector<std::vector<T>> &mesh1, std::vector<std::vector<T>> &mesh2) {
    T res = 0;
    for (int i = 0; i < mesh1.size(); ++i) {
        for (int j = 0; j < mesh1[0].size(); ++j) {
            res += pow(mesh1[i][j] - mesh2[i][j], 2);
        }
    }
    return sqrt(res);
}


#endif //DIPLOM_MESHPROCESSOR_H
