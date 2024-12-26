#pragma once
#ifndef DIPLOM_MESHCREATOR_H
#define DIPLOM_MESHCREATOR_H


#include <iosfwd>
#include <vector>
#include <valarray>
#include <optional>
#include "mdspan/mdspan.hpp"

#include <matplot/matplot.h>
#include <boost/math/quadrature/gauss_kronrod.hpp>

/*#include "iolib/Printers.h"*/
#include "computationalLib/math_core/dipolesCommon.h"



namespace meshStorage {


    using const_::FloatType;
    using floatVector = std::vector<FloatType>;
    using meshDrawClass = std::vector<floatVector>;
    using meshStorageType =std::valarray<FloatType>;
    using etx = Kokkos::extents<size_t, Kokkos::dynamic_extent, Kokkos::dynamic_extent>;
    using mdSpanType = Kokkos::mdspan<FloatType, etx>;

    template<size_t N>
    using meshArr=std::array<meshStorageType,N>;

    using namespace matplot;//todo move

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


    template<unsigned Ndots = 61>
    FloatType integrateLambdaForOneVariable(const dipoles::integrableFunction &function,
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
    meshStorageType computeFunction(const meshStorageType &a,
                                    const meshStorageType &b,
                                    const dipoles::directionGraph &func);


    /**
     * @brief Computes square Diff norm for two meshes
     * @param mesh1
     * @param mesh2
     */
    FloatType getMeshDiffNorm(const meshStorageType &mesh1, const meshStorageType &mesh2);

    void addMesh(meshStorageType &a, const meshStorageType &b);

    /**
     * @brief Computes meshes using 2 double arrays
     * @param a
     * @param b
     * @return
     */
    template<template<typename ...> typename container =std::vector>
    std::array<meshStorageType, 2> myMeshGrid(const container<FloatType> &a, const container<FloatType> &b);


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


    meshDrawClass inline unflatten(const meshStorageType &mm, const std::array<size_t,2> &numss) {
        auto res = meshDrawClass(numss[0], floatVector(numss[1], 0.0));


        mdSpanType resSpan = Kokkos::mdspan((FloatType *) &(mm[0]), numss[0], numss[1]);

        for (size_t i = 0; i < resSpan.extent(0); ++i) {
            for (size_t j = 0; j < resSpan.extent(1); ++j) {
                res[i][j] = resSpan[std::array{i, j}];
            }
        }

        return res;
    }

    meshDrawClass inline unflatten(const mdSpanType &resSpan) {
        auto res = meshDrawClass(resSpan.extent(0), floatVector(resSpan.extent(1), 0.0));


        for (size_t i = 0; i < resSpan.extent(0); ++i) {
            for (size_t j = 0; j < resSpan.extent(1); ++j) {
                res[i][j] = resSpan[std::array{i, j}];
            }
        }

        return res;
    }


    static const  size_t dimCount=2;
    static constexpr inline const FloatType rr = 2 * M_PI / params::omega;


    /**
     * @brief Class that  handles 3d mesh creation and parameters management
     */
    class MeshCreator
    {
    public:
        //todo designated separated printer for bulk print
        MeshCreator(): dimensions({7, 25}), limits({0, M_PI_2, 0, M_PI * 2}),
                       data({std::valarray<FloatType>(dimensions[0]*dimensions[1]),
                              std::valarray<FloatType>(dimensions[0]*dimensions[1]),
                              std::valarray<FloatType>(dimensions[0]*dimensions[1])}){
        }

        void constructMeshes(const std::optional<std::array<size_t,2>> dimenstion=std::nullopt,
                             const std::optional<std::array<FloatType ,4>> limit=std::nullopt);

        friend meshArr<dimCount+1> sphericalTransformation(const MeshCreator&oth);

        void applyFunction(const dipoles::directionGraph&plot);

        void applyIntegrate(const dipoles::integrableFunction&func,FloatType a=0,FloatType b=rr)
        {
            this->applyFunction([&func, &a,b](FloatType x, FloatType y) {
                return integrateLambdaForOneVariable<61>(func, y, x, a, b);
            });

        }

        std::array<meshStorage::mdSpanType,3> spans;
        void computeViews(int val=-1);

        void plotAndSave(const std::string&filename,const std::function<void(const std::string&filename,const MeshCreator&)>&plotCallback)
        {
            plotCallback(filename, *this);
        }

        std::array<size_t,dimCount> dimensions;
        std::array<FloatType ,dimCount*2> limits;
        meshArr<dimCount+1> data;
    };



    template<template<typename ...> typename container, typename T, bool end>
    container<T> myLinspace(T lower_bound, T upper_bound, size_t n) {

        if (n == 0 || lower_bound-upper_bound==0) {
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
    std::array<meshStorageType, 2> myMeshGrid(const container<FloatType> &a, const container<FloatType> &b) {
        std::array<meshStorageType, 2> ret = {meshStorageType(b.size() * a.size()),
                                              meshStorageType(b.size() * a.size())};


        auto x_mesh = Kokkos::mdspan(&(ret[0][0]), b.size(), a.size());
        auto y_mesh = Kokkos::mdspan(&(ret[1][0]), b.size(), a.size());

        for (size_t i = 0; i < b.size(); ++i) {//todof use library mesh or compy one from net
            for (size_t j = 0; j < a.size(); ++j) {
                x_mesh[std::array{i, j}] = a[j];
                y_mesh[std::array{i, j}] = b[i];
            }
        }
        return ret;
    }

    //todo move
    static const std::function<void(const std::string&,const MeshCreator&)> plotFunction=[](const std::string& filename,const MeshCreator&mesh)
    {
        using namespace meshStorage;
        auto ax = matplot::gca();
        auto data_arr=sphericalTransformation(mesh);
        ax->surf(unflatten(data_arr[0],mesh.dimensions),
                 unflatten(data_arr[1],mesh.dimensions),
                 unflatten(data_arr[2],mesh.dimensions))
                ->lighting(true).primary(0.8f).specular(0.2f);//-> view(213,22)->xlim({-40,40})->ylim({-40,40});
        ax->view(213, 22);
        ax->xlim({-40, 40});
        ax->ylim({-40, 40});
        ax->zlim({0, 90});

        matplot::save(filename);
        ax.reset();
    };

}




#endif //DIPLOM_MESHCREATOR_H
