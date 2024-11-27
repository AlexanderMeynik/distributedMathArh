
#ifndef DIPLOM_MESHPROCESSOR_H
#define DIPLOM_MESHPROCESSOR_H


#include <ostream>
#include <vector>
#include <span>



#include <ranges>
#include <concepts>
#include <type_traits>


#include <matplot/matplot.h>
#include <boost/math/quadrature/gauss_kronrod.hpp>

#include "iolib/Printers.h"
#include "const.h"
#include "computationalLib/math_core/dipolesCommon.h"

#include "iolib/Parsers.h"
#include "common/constants.h"

using const_::FloatType;
using floatVector = std::vector<FloatType>;
using meshDrawClass = std::vector<floatVector>;

//todo add local namespace

template<unsigned Ndots>
FloatType integrate(const std::function<FloatType(FloatType)> &function,
                    FloatType left,
                    FloatType right,
                    unsigned int max_depth = 5,
                    FloatType tol = 1e-20) {
    FloatType error;
    double Q = boost::math::quadrature::gauss_kronrod<FloatType, Ndots>::integrate(
            function,
            left,
            right,
            max_depth,
            tol,
            &error);
    return Q;
}

template<unsigned N>
FloatType integrateFunctionBy1Val(const dipoles::integrableFunction &function,
                                  FloatType theta,
                                  FloatType phi,
                                  FloatType left,
                                  FloatType right,
                                  unsigned int max_depth = 5,
                                  FloatType tol = 1e-20) {
    std::function<FloatType(FloatType)> tt = [&theta, &phi, &function](FloatType t) { return function(theta, phi, t); };
    return integrate<N>(tt, left, right, max_depth, tol);
}


template<typename Container>
meshDrawClass applyFunctionToVVD(const Container &a, const Container &b,
                                 const std::function<FloatType(FloatType, FloatType)> &func) {
    size_t rows = a.size();
    size_t cols = a[0].size();

    meshDrawClass result(rows, floatVector(cols));

    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            result[i][j] = func(a[i][j], b[i][j]);
        }
    }

    return result;
}



FloatType getMeshDiffNorm(const meshDrawClass &mesh1, const meshDrawClass &mesh2);

void addMesh(meshDrawClass &a, meshDrawClass &b);


using namespace matplot;


std::pair<meshDrawClass, meshDrawClass> mymeshGrid(const floatVector &a, const floatVector &b);


template<typename T>
std::vector<T> myLinspace(T lower_bound, T upper_bound, size_t n, bool end = true) {
    std::vector<T> result(n, T());

    size_t div = (end) ? (n - 1) : n;
    T step = (upper_bound - lower_bound) / (T) div;

    for (int i = 0; i < n; ++i) {
        result[i] = i * step + lower_bound;
    }
    return result;
}

using dipoles::integrableFunction;
using dipoles::directionGraph;

class MeshProcessor {
public:

    MeshProcessor() {
        initCoordMeshes();
    }

    using confType = std::pair<std::array<size_t, 2>, std::array<FloatType, 2>>;

    confType export_conf() {
        return {{this->nums[0],  this->nums[1]},
                {this->steps[0], this->steps[1]}};
    }

    void importConf(const confType &conf, bool precompute = false) {
        if (precompute) {
            this->setSteps(conf.second);
            return;
        }
        this->nums[0] = conf.first[0];
        this->nums[1] = conf.first[1];
        this->steps[0] = conf.second[0];
        this->steps[1] = conf.second[1];
        initCoordMeshes();
    }

    void setSteps(const std::array<FloatType, 2> &elems) {
        steps[0] = elems[0];
        steps[1] = elems[1];
        nums[0] = (philims[1] - philims[0]) / (steps[0]) + 1;
        nums[1] = (thelims[1] - thelims[0]) / (steps[1]) + 1;//y=(a2-a1)/x+1
        initCoordMeshes();
        //y-1=(a2-a1)/x
        //x=(a2-a1)/(y-1)
    }

    void setNums(const std::array<size_t, 2> &elems) {
        nums[0] = elems[0];
        nums[1] = elems[1];
        steps[0] = (philims[1] - philims[0]) / (nums[0] - 1);
        steps[1] = (thelims[1] - thelims[0]) / (nums[1] - 1);
        initCoordMeshes();
    }

    const std::array<meshDrawClass, 3> &getMeshdec() const {
        return meshdec;
    }

    meshDrawClass getMeshGliff() {
        return meshDrawClass(this->nums[1], floatVector(this->nums[0], 0.0));
    }

    const std::array<meshDrawClass, 3> &getMeshsph() const {
        return meshsph;
    }

    void generateMeshes(const integrableFunction &func);

    void generateNoInt(const directionGraph &func);

    void plotSpherical(std::string filename);

    void printDec(std::ostream &out);

    void setMesh3(meshDrawClass &val);

private:
    void initCoordMeshes();

    void sphericalTransformation();

    std::array<meshDrawClass, 3> meshdec;
    std::array<meshDrawClass, 3> meshsph;

    FloatType philims[2] = {0, M_PI * 2};
    FloatType thelims[2] = {0, M_PI_2};
    FloatType steps[2] = {M_PI / 12, M_PI / 12};
    size_t nums[2] = {static_cast<size_t >((philims[1] - philims[0]) / (steps[0])) + 1,
                      static_cast<size_t >((thelims[1] - thelims[0]) / (steps[1])) + 1};
    static constexpr const FloatType rr = 2 * M_PI / params::omega;
    static constexpr const FloatType step = M_PI / 12;
};

//todo move parser away from mth lib
template<>
class Parser<MeshProcessor> {
public:
    Parser() : vals_() {}

    Parser(int size) : vals_() {}

    friend std::istream &operator>>(std::istream &in, Parser &pp) {
        std::string dummy;
        std::getline(in, dummy);
        std::getline(in, dummy);
        std::getline(in, dummy);
        //pp.vals_=MeshProcessor<T>();
        meshDrawClass m = pp.vals_.getMeshGliff();
        for (int i = 0; i < m[0].size(); ++i) {
            FloatType temp = 0;
            in >> temp;//diskard first number
            for (int j = 0; j < m.size(); ++j) {
                FloatType val;
                in >> val;
                m[j][i] = val;
            }
        }
        pp.vals_.setMesh3(
                m);//todo мы не иницализируем финальную компоненту как надо, поэтому создаём глиф тут и печаетев всё в него
        return in;
    }

    MeshProcessor vals_;

};


#endif //DIPLOM_MESHPROCESSOR_H
