#pragma once
#ifndef DIPLOM_MESHCREATOR_H
#define DIPLOM_MESHCREATOR_H


#include <iosfwd>
#include <optional>

#include <boost/math/quadrature/gauss_kronrod.hpp>

#include "common/commonTypes.h"
#include "common/printUtils.h"

/// meshStorage namespace
namespace meshStorage {
    namespace co = commonTypes;
    using shared::FloatType, shared::params;
    using printUtils::IosStateScientific;
    using commonTypes::dimType;

    ///stores upper and lower limit pairs for meshes
    using limType = std::array<FloatType, 4>;


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
    FloatType integrate(const std::function<FloatType(FloatType)> &function,
                               FloatType left,
                               FloatType right,
                               unsigned int max_depth = 5,
                               FloatType tol = Eigen::NumTraits<FloatType>::epsilon());


    template<unsigned Ndots = 61>
    FloatType integrateLambdaForOneVariable(const co::integrableFunction &function,
                                            FloatType theta,
                                            FloatType phi,
                                            FloatType left,
                                            FloatType right,
                                            unsigned int max_depth = 5,
                                            FloatType tol = 1e-20);


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
    FloatType getMeshDiffNorm(const co::meshStorageType &mesh1,
                              const co::meshStorageType &mesh2);

    void addMesh(co::meshStorageType &a,
                 const co::meshStorageType &b);

    /**
     * @brief Computes meshes using 2 double arrays
     * @param a
     * @param b
     * @return
     */
    template<template<typename ...> typename container =std::vector>
    std::array<co::meshStorageType, 2> myMeshGrid(const container<co::FloatType> &a,
                                                  const container<co::FloatType> &b);


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
    container<T> myLinspace(T lower_bound,
                            T upper_bound,
                            size_t n);


    co::meshDrawClass unflatten(const co::meshStorageType &mm,
                                const dimType &dims);


    static constexpr inline const FloatType rr = 2 * M_PI / params::omega;

    static constexpr inline dimType defaultDims = {7, 25};
    static constexpr inline limType defaultLims = {0, M_PI_2, 0, M_PI * 2};

    /**
     * @brief Class that  handles 3d mesh creation and parameters management
     */
    class MeshCreator {
    public:
        /**
         * @brief Default MeshCreator constructor
         * @param construct - will allocate memory for data arrays if is true
         */
        MeshCreator(bool construct=true);

        /**
         * @brief Computes 2d meshgrid using dims,lims
         * @param dims
         * @param lims
         */
        void constructMeshes(const dimType &dims,
                             const limType &lims);

        /**
         * @brief Will substitute any null-opt with default values
         * @param dimOpt
         * @param limOpt
         */
        void constructMeshes(std::optional<dimType> dimOpt = std::nullopt,
                             std::optional<limType> limOpt = std::nullopt);

        /**
         * @brief Cartesian to spherical coordinate transformation
         * @param oth
         * @return
         */
        friend meshArr<3> sphericalTransformation(const MeshCreator &oth);

        /**
         * @brief Computes data[2] using provided func function
         * @param func
         */
        void applyFunction(const co::directionGraph &func);

        /**
         * @brief Uses numerical integration with range [a,b] compute data[2]
         * @param func
         * @param a
         * @param b
         */
        void applyIntegrate(const co::integrableFunction &func,
                            FloatType a = 0,
                            FloatType b = rr);

        /**
         * @brief Calls provided callback to plot this mesh
         * @param filename
         * @param plotCallback
         */
        void plotAndSave[[deprecated("remove mathplot support")]](const std::string &filename,
                         const std::function<void(const std::string &filename,
                                                  const MeshCreator &)> &plotCallback) {
            plotCallback(filename, *this);
        }

        dimType dimensions;
        limType limits;
        meshArr<3> data;
    };

    /**
     * @brief Prints mesh in human readable way
     * @param mmesh
     * @param out
     */
    void printDec(const meshStorage::MeshCreator &mmesh,
                  std::ostream &out);


}

namespace meshStorage
{

    template<unsigned int Ndots>
    FloatType integrate(const std::function<FloatType(FloatType)> &function,
                        FloatType left, FloatType right,
                        unsigned int max_depth,
                        FloatType tol) {
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

    template<unsigned int Ndots>
    FloatType
    integrateLambdaForOneVariable(const commonTypes::integrableFunction &function,
                                  FloatType theta,
                                  FloatType phi,
                                  FloatType left,
                                  FloatType right,
                                  unsigned int max_depth,
                                  FloatType tol) {
        std::function<FloatType(FloatType)> tt = [&theta, &phi, &function](FloatType t) {
            return function(theta, phi, t);
        };
        return integrate<Ndots>(tt, left, right, max_depth, tol);
    }

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
    myMeshGrid(const container<co::FloatType> &a,
               const container<co::FloatType> &b) {
        std::array<co::meshStorageType, 2> ret = {co::meshStorageType(b.size() * a.size()),
                                                  co::meshStorageType(b.size() * a.size())};

        for (size_t i = 0; i < b.size(); ++i) {
            for (size_t j = 0; j < a.size(); ++j) {
                ret[0][i*a.size()+j] = a[j];
                ret[1][i*a.size()+j] = b[i];
            }
        }
        return ret;
    }


}


#endif //DIPLOM_MESHCREATOR_H
