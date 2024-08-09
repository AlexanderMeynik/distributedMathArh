
#ifndef DIPLOM_MESHPROCESSOR_H
#define DIPLOM_MESHPROCESSOR_H

#include <matplot/matplot.h>
#include <ostream>
#include <vector>
#include <boost/math/quadrature/gauss_kronrod.hpp>
#include "../common/printUtils.h"
#include "const.h"

#include "../common/Parsers.h"
#include "../common/Printers.h"
//todo получше организовать фаловую иерарзию
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


#include <ranges>
#include <concepts>
#include <type_traits>
template<typename T>
concept ElementIterable = requires(std::ranges::range_value_t<T> x)
{
    x.begin();          // must have `x.begin()`
    x.end();            // and `x.end()`
};
template<typename T, template<typename >typename CONT>
concept has_size = requires( CONT<T> t)
{
    { t.size() } -> std::convertible_to<std::size_t>;
};

template<typename T, template<typename >typename CONT>
concept has_brackets= requires( CONT<T> t,size_t i)
{
    { t[i] } -> std::convertible_to<T&>;
};

template<typename T, typename U>
concept has_elemet = requires( T t,int i)
{
    { t[i]} -> std::convertible_to<U&>;
};

template<typename T,typename U, template<typename >typename CONT>
requires has_size<U,CONT>&&has_brackets<U,CONT>&&has_elemet<U,T>
CONT<U> applyFunctionToVVD(const CONT<U>& a, const CONT<U>& b, const std::function<T(T, T)>& func) {
    size_t rows = a.size();
    size_t cols = a[0].size();

    CONT<U> result(rows, std::vector<double>(cols));

    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            result[i][j] = func(a[i][j], b[i][j]);
        }
    }

    return result;
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

    std::pair<std::array<size_t,2 >,std::array<T,2>> export_conf()
    {
        return {{this->nums[0],this->nums[1]},{this->steps[0],this->steps[1]}};
    }

    void importConf(std::pair<std::array<size_t,2 >,std::array<T,2>>&conf,bool precompute=false)
    {
        if(precompute)
        {
            this->setSteps(conf.second);
            return;
        }
        this->nums[0]=conf.first[0];
        this->nums[1]=conf.first[1];
        this->steps[0]=conf.second[0];
        this->steps[1]=conf.second[1];
        initCoordMeshes();
    }

    void setSteps(std::array<T , 2 > &elems) {
        steps[0] = elems[0];
        steps[1] = elems[1];
        nums[0] = (philims[1] - philims[0]) / (steps[0]) + 1;
        nums[1]=(thelims[1] - thelims[0]) / (steps[1]) + 1;//y=(a2-a1)/x+1
        initCoordMeshes();
        //y-1=(a2-a1)/x
        //x=(a2-a1)/(y-1)
    }

    void setNums(std::array<size_t , 2 > &elems) {
        nums[0]=elems[0];
        nums[1]=elems[1];
        steps[0]=(philims[1] - philims[0])/(nums[0]-1);
        steps[1]=(thelims[1] -thelims[0])/(nums[1]-1);
        initCoordMeshes();
    }

    const std::array<std::vector<std::vector<T>>, 3> &getMeshdec() const {
        return meshdec;
    }

    std::vector<std::vector<T>> getMeshGliff() {
        return std::vector<std::vector<T>>(this->nums[1], std::vector<T>(this->nums[0], 0.0));
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

    T philims[2] = {0, M_PI * 2};
    T thelims[2] = {0, M_PI_2};
    T steps[2] = {M_PI / 12, M_PI / 12};
    size_t nums[2] = {static_cast<size_t >((philims[1] - philims[0]) / (steps[0])) + 1, static_cast<size_t >((thelims[1] - thelims[0]) / (steps[1])) + 1};
    static constexpr const T rr = 2 * M_PI / params<T>::omega;
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
    for (T theta = thelims[0]; theta < thelims[1]; theta += steps[1]) {
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
            ->lighting(true).primary(0.8f).specular(0.2f);//-> view(213,22)->xlim({-40,40})->ylim({-40,40});
    //surf(x, y, z);
    ax->view(213, 22);
    ax->xlim({-40, 40});
    ax->ylim({-40, 40});
    ax->zlim({0, 90});

    matplot::save(filename);
    ax.reset();
}

template<typename T>
void MeshProcessor<T>::generateNoInt(const std::function<T(T, T)> &func) {
    T rr1 = this->rr;

    //meshdec[2] = transform(meshdec[0], meshdec[1], func);
    meshdec[2]= applyFunctionToVVD(meshdec[0], meshdec[1], func);
   // std::transform(meshdec[0].begin(), meshdec[0].end(), meshdec[1].begin(), meshdec[1].end(),std::back_inserter(meshdec[2]),func);
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
    meshdec[2]= applyFunctionToVVD(meshdec[0], meshdec[1], [&func, &rr1](T x, T y) {
        return integrateFunctionBy1Val<T, 61>(func, y, x, 0, rr1);
    });
    /*meshdec[2] = transform(meshdec[0], meshdec[1], [&func, &rr1](T x, T y) {
        return integrateFunctionBy1Val<T, 61>(func, y, x, 0, rr1);
    });*/

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
template<typename T>
class Parser<MeshProcessor<T>>
{
public:
    Parser() : size_(0), vals_() {}

    Parser(int size) : size_(size), vals_() {}

    friend std::istream& operator>>(std::istream& in, Parser& pp)
    {
        std::string dummy;
        std::getline(in, dummy);
        std::getline(in, dummy);
        std::getline(in, dummy);
        //pp.vals_=MeshProcessor<T>();
        std::vector<std::vector<T>> m=pp.vals_.getMeshGliff();
        for (int i = 0; i <m[0].size() ; ++i) {
            T temp=0;
            in>>temp;//diskard first number
            for (int j = 0; j < m.size(); ++j) {
                in>>m[j][i];
            }
        }
        pp.vals_.setMesh3(m);//todo мы не иницализируем финальную компоненту как надо, поэтому создаём глиф тут и печаетев всё в него
        return in;
    }

    MeshProcessor<T> vals_;
    int size_;
};


#endif //DIPLOM_MESHPROCESSOR_H
