#pragma once
#ifndef DIPLOM_MESHCREATOR_H
#define DIPLOM_MESHCREATOR_H


#include <iosfwd>
#include "mdspan/mdspan.hpp"
#include <boost/math/quadrature/gauss_kronrod.hpp>

#include "common/commonTypes.h"
#include "common/printUtils.h"


namespace meshStorage {
    namespace co = commonTypes;
    using shared::FloatType, shared::params;
    using printUtils::IosStateScientific;

    using etx = Kokkos::extents<size_t, Kokkos::dynamic_extent, Kokkos::dynamic_extent>;
    using mdSpanType = Kokkos::mdspan<FloatType, etx>;

    template<size_t N>
    using meshArr = std::array<co::meshStorageType, N>;

    /**
     *
     * @tparam Ndots
     * @param function
     * @param left
     * @param right
     * @param max_depth
     * @param tol
     */
    template<unsigned Ndots = 61>
    FloatType inline integrate(const std::function<FloatType(FloatType)> &function,
                               FloatType left,
                               FloatType right,
                               unsigned int max_depth = 5,
                               FloatType tol = Eigen::NumTraits<FloatType>::epsilon()) {
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


    template<unsigned Ndots = 61>
    FloatType integrateLambdaForOneVariable(const co::integrableFunction &function,
                                            FloatType theta,
                                            FloatType phi,
                                            FloatType left,
                                            FloatType right,
                                            unsigned int max_depth = 5,
                                            FloatType tol = 1e-20) {
        std::function<FloatType(FloatType)> tt = [&theta, &phi, &function](FloatType t) {
            return function(theta, phi, t);
        };
        return integrate<Ndots>(tt, left, right, max_depth, tol);
    }


    /**
     * @brief Computes functional mesh for 2 coordinates
     * @param a
     * @param b
     * @param func
     */
    co::meshStorageType computeFunction(const co::meshStorageType &a,
                                        const co::meshStorageType &b,
                                        const co::directionGraph &func);


    /**
     * @brief Computes square Diff norm for two meshes
     * @param mesh1
     * @param mesh2
     */
    FloatType getMeshDiffNorm(const co::meshStorageType &mesh1, const co::meshStorageType &mesh2);

    void addMesh(co::meshStorageType &a, const co::meshStorageType &b);

    /**
     * @brief Computes meshes using 2 double arrays
     * @param a
     * @param b
     * @return
     */
    template<template<typename ...> typename container =std::vector>
    std::array<co::meshStorageType, 2> myMeshGrid(const container<co::FloatType> &a, const container<co::FloatType> &b);


    /**
     * @brief Generates linearly spaced vectors
     * @tparam container
     * @tparam T
     * @tparam end
     * @param lower_bound
     * @param upper_bound
     * @param n
     * @return
     */
    template<template<typename ...> typename container =std::vector, typename T=FloatType, bool end = true>
    container<T> myLinspace(T lower_bound, T upper_bound, size_t n);


    co::meshDrawClass inline unflatten(const co::meshStorageType &mm, const std::array<size_t, 2> &numss) {
        auto res = co::meshDrawClass(numss[0], co::standartVec(numss[1], 0.0));


        mdSpanType resSpan = Kokkos::mdspan((FloatType *) &(mm[0]), numss[0], numss[1]);

        for (size_t i = 0; i < resSpan.extent(0); ++i) {
            for (size_t j = 0; j < resSpan.extent(1); ++j) {
                res[i][j] = resSpan[std::array{i, j}];
            }
        }

        return res;
    }

    co::meshDrawClass inline unflatten(const mdSpanType &resSpan) {
        auto res = co::meshDrawClass(resSpan.extent(0), co::standartVec(resSpan.extent(1), 0.0));


        for (size_t i = 0; i < resSpan.extent(0); ++i) {
            for (size_t j = 0; j < resSpan.extent(1); ++j) {
                res[i][j] = resSpan[std::array{i, j}];
            }
        }

        return res;
    }


    static const size_t dimCount = 2;
    static constexpr inline const FloatType rr = 2 * M_PI / params::omega;


    /**
     * @brief Class that  handles 3d mesh creation and parameters management
     */
    class MeshCreator {
    public:
        //todo designated separated printer for bulk print
        MeshCreator() : dimensions({7, 25}), limits({0, M_PI_2, 0, M_PI * 2}),
                        data({std::valarray<FloatType>(dimensions[0] * dimensions[1]),
                              std::valarray<FloatType>(dimensions[0] * dimensions[1]),
                              std::valarray<FloatType>(dimensions[0] * dimensions[1])}) {
        }

        void constructMeshes();

        void constructMeshes(const std::array<size_t, 2> &dimenstion) {
            this->dimensions = dimenstion;
        }

        void constructMeshes(
                const std::array<FloatType, 4> &limit) {
            this->limits = limit;
        }

        void constructMeshes(const std::array<size_t, 2> &dimenstion,
                             const std::array<FloatType, 4> &limit) {
            this->dimensions = dimenstion;
            this->limits = limit;
        }

        friend meshArr<dimCount + 1> sphericalTransformation(const MeshCreator &oth);

        void applyFunction(const co::directionGraph &plot);

        void applyIntegrate(const co::integrableFunction &func, FloatType a = 0, FloatType b = rr) {
            this->applyFunction([&func, &a, b](FloatType x, FloatType y) {
                return integrateLambdaForOneVariable<61>(func, y, x, a, b);
            });

        }

        std::array<meshStorage::mdSpanType, 3> spans;

        void computeViews(int val = -1);

        void plotAndSave(const std::string &filename,
                         const std::function<void(const std::string &filename, const MeshCreator &)> &plotCallback) {
            plotCallback(filename, *this);
        }

        std::array<size_t, dimCount> dimensions;
        std::array<FloatType, dimCount * 2> limits;
        meshArr<dimCount + 1> data;
    };

    void
    printDec(meshStorage::MeshCreator &mmesh, std::ostream &out, int N = std::numeric_limits<FloatType>::digits10 - 1);


    template<template<typename ...> typename container, typename T, bool end>
    container<T> myLinspace(T lower_bound, T upper_bound, size_t n) {

        if (n == 0 || lower_bound - upper_bound == 0) {
            throw std::invalid_argument("Zero linspace size");
        }

        container<T> result(n);

        size_t div;
        if constexpr (end) {
            div = n - 1;
        } else {
            div = n;
        }
        T step = (upper_bound - lower_bound) / (T) div;

        for (int i = 0; i < n; ++i) {
            result[i] = i * step + lower_bound;
        }
        return result;
    }

    template<template<typename ...> typename container>
    std::array<co::meshStorageType, 2>
    myMeshGrid(const container<co::FloatType> &a, const container<co::FloatType> &b) {
        std::array<co::meshStorageType, 2> ret = {co::meshStorageType(b.size() * a.size()),
                                                  co::meshStorageType(b.size() * a.size())};


        auto x_mesh = Kokkos::mdspan(&(ret[0][0]), b.size(), a.size());
        auto y_mesh = Kokkos::mdspan(&(ret[1][0]), b.size(), a.size());

        for (size_t i = 0; i < b.size(); ++i) {
            for (size_t j = 0; j < a.size(); ++j) {
                x_mesh[std::array{i, j}] = a[j];
                y_mesh[std::array{i, j}] = b[i];
            }
        }
        return ret;
    }


}


#endif //DIPLOM_MESHCREATOR_H
