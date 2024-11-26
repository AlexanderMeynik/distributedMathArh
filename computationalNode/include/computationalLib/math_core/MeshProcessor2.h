
#ifndef DIPLOM_MESHPROCESSOR_H
#define DIPLOM_MESHPROCESSOR_H


#include <ostream>
#include <vector>
#include <valarray>
#include "mdspan/mdspan.hpp"

#include <ranges>
#include <concepts>
#include <type_traits>

#include <matplot/matplot.h>
#include <boost/math/quadrature/gauss_kronrod.hpp>

#include "common/Printers.h"
#include "const.h"
#include "computationalLib/math_core/dipolesCommon.h"

#include "common/Parsers.h"
#include "common/constants.h"

using const_::FloatType;
using floatVector = std::vector<FloatType>;
using meshDrawClass = std::vector<floatVector>;
using meshStorageType =std::valarray<FloatType>;

namespace meshStorage
{
    using namespace matplot;
    /**
     *
     * @tparam Ndots
     * @param function
     * @param left
     * @param right
     * @param max_depth
     * @param tol
     */
    template<unsigned Ndots=61>
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



    template<unsigned Ndots=61>
    FloatType integrateLambdaForOneVariable(const dipoles::integrableFunction &function,
                                            FloatType theta,
                                            FloatType phi,
                                            FloatType left,
                                            FloatType right,
                                            unsigned int max_depth = 5,
                                            FloatType tol = 1e-20) {
        std::function<FloatType(FloatType)> tt = [&theta, &phi, &function](FloatType t) { return function(theta, phi, t); };
        return integrate<Ndots>(tt, left, right, max_depth, tol);
    }


    /**
     * @brief Computes functional mesh for N coordinates
     * @tparam T
     * @tparam Args
     */
    template<typename T, typename... Args>
    requires (std::is_same_v<T,Args>&&...)
    T computeFunction_t(size_t size,const Args&... args,const std::function<FloatType (FloatType ...)>&function)
    {
        //todo size checks
        T result(size);
        for (size_t i = 0; i < size; ++i) {
            result[i]=func(args[i]...);
        }
        return result;
    }

    /**
     * @brief Computes functional mesh for 2 coordinates
     * @param a
     * @param b
     * @param func
     */
    meshStorageType computeFunction(const meshStorageType&a,
                                    const meshStorageType&b,
                                    const dipoles::directionGraph&func);



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
    std::array<meshStorageType,2> myMeshGrid(const container<FloatType> &a, const container<FloatType> &b);




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
    template<template<typename ...> typename container =std::vector,typename T=FloatType,bool end = true>
    container<T> myLinspace(T lower_bound, T upper_bound, size_t n);


    class MeshProcessor {
    public:

        template<size_t N>
        using meshArr = std::array<meshStorageType,N>;

        using etx=Kokkos::extents<size_t,Kokkos::dynamic_extent,Kokkos::dynamic_extent>;
        using mdSpanType= Kokkos::mdspan<FloatType,etx>;

        typedef dipoles::integrableFunction integrableFunction;
        typedef dipoles::directionGraph directionGraph;

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
            nums[1] = (thelims[1] - thelims[0]) / (steps[1]) + 1;
            initCoordMeshes();
        }

        void setNums(const std::array<size_t, 2> &elems) {
            nums[0] = elems[0];
            nums[1] = elems[1];
            steps[0] = (philims[1] - philims[0]) / (nums[0] - 1);
            steps[1] = (thelims[1] - thelims[0]) / (nums[1] - 1);
            initCoordMeshes();
        }

        const meshArr<3> &getMeshdec() const {
            return meshdec;
        }

        meshStorageType getMeshGliff() {
            return meshStorageType(this->nums[1]*this->nums[0]);
        }

        const meshArr<3> &getMeshsph() const {
            return meshsph;
        }

        void generateMeshes(const integrableFunction &func);

        void generateNoInt(const directionGraph &func);

        void plotSpherical(std::string filename);

        void printDec(std::ostream &out);

        void setMesh3(meshStorageType &val);

    private:
        static meshDrawClass unflatten(const meshStorageType &mm,size_t  numss[2])
        {
            auto res=meshDrawClass(numss[1], floatVector(numss[1], 0.0));


            mdSpanType resSpan=Kokkos::mdspan((FloatType *)&(mm[0]),numss[1],numss[1]);

            for (size_t i = 0; i < resSpan.extent(0); ++i) {
                for (size_t j = 0; j < resSpan.extent(1); ++j) {
                    res[i][j]=resSpan[std::array{i,j}];
                }
            }

            return res;
        }
        void initCoordMeshes();

        void sphericalTransformation();
        void updateSpans()
        {
            for (size_t i = 0; i < meshdec.size() ; ++i) {
                meshDecSpans[i]=mdSpanType((FloatType *)&(meshdec[0]),nums[1],nums[0]);
                meshSphSpans[i]=mdSpanType((FloatType *)&(meshsph[0]),nums[1],nums[0]);
            }
        }

        meshArr<3>  meshdec;
        meshArr<3>  meshsph;
        std::array<mdSpanType ,3> meshDecSpans;
        std::array<mdSpanType ,3> meshSphSpans;


        FloatType philims[2] = {0, M_PI * 2};
        FloatType thelims[2] = {0, M_PI_2};
        FloatType steps[2] = {M_PI / 12, M_PI / 12};
        size_t nums[2] = {static_cast<size_t >((philims[1] - philims[0]) / (steps[0])) + 1,
                          static_cast<size_t >((thelims[1] - thelims[0]) / (steps[1])) + 1};
        static constexpr const FloatType rr = 2 * M_PI / params::omega;
        static constexpr const FloatType step = M_PI / 12;
    };






















    template<template<typename ...> typename container,typename T,bool end>
    container<T> myLinspace(T lower_bound, T upper_bound, size_t n) {

        if(n==0)
        {
            throw std::invalid_argument("Zero linspace size");
        }

        container<T> result(n);

        size_t div;
        if constexpr (end)
        {
            div=n-1;
        }
        else
        {
            div=n;
        }
        T step = (upper_bound - lower_bound) / (T) div;

        for (int i = 0; i < n; ++i) {
            result[i] = i * step + lower_bound;
        }
        return result;
    }

    template<template<typename ...> typename container>
    std::array<meshStorageType,2> myMeshGrid(const container<FloatType> &a, const container<FloatType> &b) {
        std::array<meshStorageType,2> ret={meshStorageType(b.size()*a.size()),
                                           meshStorageType(b.size()*a.size())};


        auto x_mesh=Kokkos::mdspan(&(ret[0][0]),b.size(),a.size());
        auto y_mesh =Kokkos::mdspan(&(ret[1][0]),b.size(),a.size());

        for (size_t i = 0; i < b.size(); ++i) {
            for (size_t j = 0; j < a.size(); ++j) {
                x_mesh[std::array{i,j}]=a[j];
                y_mesh[std::array{i, j}] = b[i];
            }
        }
        return ret;
    }
}


#endif //DIPLOM_MESHPROCESSOR_H
