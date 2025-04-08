#pragma once

#include "dipolesCommon.h"

///dipoles namespace
namespace dipoles {
    using namespace Eigen;
    using namespace myConcepts;
    using shared::FloatType, shared::params;

    /**
     * @brief Provides interface to solve system of dipoles
     * @details Allows to construct and solve system of equations for the selected mathematical model.
     * @details Can accept various input formats for coordinates and solutions to crete system,
     * @details to solve it, to generate direction graph for magnetic field.
     */
    class Dipoles {
    public:
        Dipoles() = default;

        /**
         * @brief Basic constructor that acccepts dipole coordinates Container
         * @tparam Container
         * @param xi
         */
        template<typename Container>
        requires HasSizeMethod<Container>
        Dipoles(const Container &xi);

        /**
         * @brief Function that allows to reset dipole coordinates
         * @details This function is used to avoid memory reallocation if dipole object already have been initialized.
         * @tparam Container
         * @param xi
         */
        template<typename Container>
        requires HasSizeMethod<Container>
        void setNewCoordinates(const Container &xi);

        /**
         * @brief Loads system of equations matrix
         * @param xi
         */
        void loadFromMatrix(const matrixType &xi);

        /**
         * Calculates directional graph and/or other it variants
         * @tparam Container
         * @tparam Container2
         * @param xi
         * @param sol
         */
        template<typename Container, typename Container2>
        requires HasSizeMethod<Container> && HasSizeMethod<Container2>
        void getFullFunction_(const Container &xi, const Container2 &sol);

        /**
         * @brief Computes solution vector for dipole parameters and returns it in available format
         * @tparam Container
         */
        template<typename Container=returnToDataType<returnType::EigenVector>>
        Container solve();

        const integrableFunction &getIfunction() const {
            return Ifunction_;
        }

        const directionGraph &getI2function() const {
            return I2function_;
        }

        /**
         * @brief Returns reference to the right part for system of equations
         */
        const EigenVec &getRightPart();

        /**
         * @brief Retrieves matrix computed for system of equations
         */
        matrixType getMatrixx();

    protected:

        /**
         * @brief Computes euclidean distacne for dipole coordinates
         * @tparam Container
         * @param i1
         * @param i2
         * @param xi
         */
        template<typename Container>
        requires HasBracketOperator<Container>
        FloatType getDistance(size_t i1, size_t i2, Container &xi);

        /**
         * @brief Computes vector difference for dipole coordinates
         * @details older name getRIM
         * @tparam Container
         * @param i1
         * @param i2
         * @param xi
         */
        template<typename Container>
        requires HasBracketOperator<Container>
        Eigen::Vector<FloatType, 2> twoDVecDifference(size_t i1, size_t i2, Container &xi);

        /**
         * @brief Intializes dimensions for system of equations matrices
         */
        void initArrays();

        /*
         * @brief Calculates temporary 2 by 2 matrices to facilitate computation
         */
        void getMatrices(const Eigen::Vector<FloatType, 2> &rim, FloatType rMode, Eigen::Matrix<FloatType, 2, 2> &K1,
                         Eigen::Matrix<FloatType, 2, 2> &K2) const;

        /**
         * @brief Recalculates matrix values for dipole coordinates vector
         * @tparam Container
         * @param xi
         */
        template<typename Container>
        void setMatrixes(const Container &xi);


        matrixType M1_;
        matrixType M2_;
        integrableFunction Ifunction_;
        directionGraph I2function_;
        Eigen::Vector<FloatType, Eigen::Dynamic> f;

        FloatType an = params::a;
        int N_;
    };

    template<typename Container>
    requires HasSizeMethod<Container>
    Dipoles::Dipoles(const Container &xi) {
        if constexpr (not HasBracketsNested<Container>) {
            this->N_ = xi.size() / 2;
        } else {
            this->N_ = xi[0].size();
        }
        initArrays();
        setMatrixes(xi);
    }

    template<typename Container>
    void Dipoles::setMatrixes(const Container &xi) {
        std::vector<std::pair<int, int>> sectors(4);
        sectors[0] = {0, 0};
        sectors[1] = {0, 2 * N_};
        sectors[2] = {2 * N_, 0};
        sectors[3] = {2 * N_, 2 * N_};
        for (int I = 0; I < N_; ++I) {//MAC
            for (int M = 0; M < N_; ++M) {
                if (I == M) {
                    Eigen::Matrix<FloatType, 2, 2> id = Eigen::Matrix<FloatType, 2, 2>::Identity() *
                                                        (params::omega0 * params::omega0 -
                                                         params::omega * params::omega);
                    M1_.template block<2, 2>(2 * I, 2 * M) = id;
                } else {

                    Eigen::Vector<FloatType, 2> rim = twoDVecDifference(M, I, xi);
                    auto rMode = getDistance(I, M, xi);
                    Eigen::Matrix<FloatType, 2, 2> K1;
                    Eigen::Matrix<FloatType, 2, 2> K2;
                    getMatrices(rim, rMode, K1, K2);
                    auto arg = params::omega * rMode / params::c;
                    Eigen::Matrix<FloatType, 2, 2> tmpmatr = -an * (K1 * cos(arg) - K2 * sin(arg));
                    M1_.template block<2, 2>(2 * I, 2 * M) = tmpmatr;
                }
            }
        }

        for (int I = 0; I < N_; ++I) {//MBC
            for (int M = 0; M < N_; ++M) {
                if (I == M) {
                    Eigen::Matrix<FloatType, 2, 2> id =
                            Eigen::Matrix<FloatType, 2, 2>::Identity() * (params::yo * params::omega);
                    M2_.template block<2, 2>(2 * I, 2 * M) = -id;
                } else {

                    Eigen::Vector<FloatType, 2> rim = twoDVecDifference(M, I, xi);
                    auto rMode = getDistance(I, M, xi);
                    Eigen::Matrix<FloatType, 2, 2> K1;
                    Eigen::Matrix<FloatType, 2, 2> K2;
                    getMatrices(rim, rMode, K1, K2);
                    auto arg = params::omega * rMode / params::c;

                    Eigen::Matrix<FloatType, 2, 2> tmpmatr = -an * (K2 * cos(arg) + K1 * sin(arg));

                    M2_.template block<2, 2>(2 * I, 2 * M) = tmpmatr;
                }

            }
        }
    }

    template<typename Container>
    requires HasSizeMethod<Container>
    void Dipoles::setNewCoordinates(const Container &xi) {
        if (this->f.size() != xi.size() * 2) {
            this->N_ = xi.size() / 2;
            initArrays();
        }
        setMatrixes(xi);
    }

    template<typename Container, typename Container2>
    requires HasSizeMethod<Container> && HasSizeMethod<Container2>
    void Dipoles::getFullFunction_(const Container &xi, const Container2 &sol) {
        this->Ifunction_ = [&xi, &sol](FloatType theta, FloatType phi, FloatType t) {
            int N = xi.size() / 2;
            FloatType res = 0;
            FloatType s[2] = {cos(phi), sin(phi)};
            FloatType ress[3] = {0, 0, 0};
            for (int i = 0; i < N; ++i) {
                FloatType ri[2] = {getElement(xi, 0, i, N),
                                   getElement(xi, 1, i, N)};
                FloatType ys = (ri[1] * cos(phi) - ri[0] * sin(phi)) * sin(theta);
                FloatType t0 = t - ys / params::c;
                FloatType Ai[2] = {get_value(sol, 2 * i), get_value(sol, 2 * i + 1)};
                FloatType Bi[2] = {get_value(sol, 2 * i + 2 * N), get_value(sol, 2 * i + 1 + 2 * N)};


                FloatType Di[2] = {Ai[0] * cos(params::omega * t0) + Bi[0] * sin(params::omega * t0),
                                   Ai[1] * cos(params::omega * t0) + Bi[1] * sin(params::omega * t0)};

                FloatType vi[2] = {params::omega * (Bi[0] * cos(params::omega * t0)
                                                    - Ai[0] * sin(params::omega * t0)) / params::c,
                                   params::omega * (Bi[1] * cos(params::omega * t0)
                                                    - Ai[1] * sin(params::omega)) / params::c};
                FloatType ai[2] = {-pow(params::omega, 2) * Di[0], -pow(params::omega, 2) * Di[1]};

                FloatType vsi = vi[0] * s[0] + vi[1] * s[1];
                FloatType asi = ai[0] * s[0] + ai[1] * s[1];

                for (int coord = 0; coord < 2; ++coord) {
                    FloatType ttt = ai[coord] * (vsi * sin(theta) - 1) +
                                    s[coord] * asi * pow(sin(theta), 2) -
                                    vi[coord] * asi * sin(theta);
                    ress[coord] += ttt;
                }

                FloatType t3 = asi * sin(theta) * cos(theta);
                ress[2] += t3;
            }
            for (FloatType elem: ress) {
                res += elem * elem;
            }
            return res;
        };

        this->I2function_ = [&xi, &sol](FloatType phi, FloatType theta) {
            int N = xi.size() / 2;
            FloatType omega0 = params::omega;
            FloatType T0 = M_PI * 2 / omega0;
            FloatType res;
            Eigen::Vector<FloatType, 2> resxy = {0.0, 0.0};
            FloatType resz = 0.0;
            FloatType o3dco3dc = pow(omega0, 3) / params::c;//todo why is this here?
            FloatType o2 = pow(omega0, 2);
            FloatType sinth2 = pow(sin(theta), 2);
            Eigen::Vector<FloatType, 2> s = {cos(phi),
                                             sin(phi)};
            for (int i = 0; i < N; ++i) {
                FloatType ri[2] = {getElement(xi, 0, i, N), getElement(xi, 1, i, N)};
                FloatType ys = (ri[1] * cos(phi) - ri[0] * sin(phi)) * sin(theta);
                Eigen::Vector<FloatType, 2> Ai = {get_value(sol, 2 * i), get_value(sol, 2 * i + 1)};
                Eigen::Vector<FloatType, 2> Bi = {get_value(sol, 2 * i + 2 * N), get_value(sol, 2 * i + 1 + 2 * N)};

                FloatType argument = omega0 * ys / params::c;
                FloatType Ais = Ai.dot(s);
                FloatType Bis = Bi.dot(s);
                Eigen::Vector<FloatType, 2> ABis = Ai * Bis;
                Eigen::Vector<FloatType, 2> BAis = Bi * Ais;

                Eigen::Vector<FloatType, 2> Pc1i = ((s * Ais * sinth2 - Ai) * cos(argument) -
                                                    (s * Bis * sinth2 - Bi) * sin(argument));
                Eigen::Vector<FloatType, 2> Ps1i = ((s * Ais * sinth2 - Ai) * sin(argument) +
                                                    (s * Bis * sinth2 - Bi) * cos(argument));
                Eigen::Vector<FloatType, 2> Pcomi = -(omega0 / params::c) * (sin(theta)) * (ABis - BAis);

                FloatType Pci = (Ais * cos(argument) - Bis * sin(argument));
                FloatType Psi = (Ais * sin(argument) + Bis * cos(argument));

                Eigen::Vector<FloatType, 2> rj;
                FloatType ysj;
                Eigen::Vector<FloatType, 2> Aj;
                Eigen::Vector<FloatType, 2> Bj;
                FloatType argumentj;

                FloatType Ajs;
                FloatType Bjs;


                FloatType Pcj;
                FloatType Psj;


                Eigen::Vector<FloatType, 2> Pc1j;
                Eigen::Vector<FloatType, 2> Ps1j;
                Eigen::Vector<FloatType, 2> Pcomj;


                Eigen::Vector<FloatType, 2> ABjs;
                Eigen::Vector<FloatType, 2> BAjs;
                for (int j = 0; j < i; ++j) {
                    rj = {getElement(xi, 0, j, N), getElement(xi, 1, j, N)};
                    ysj = (rj[1] * cos(phi) - rj[0] * sin(phi)) * sin(theta);

                    Aj = {get_value(sol, 2 * j), get_value(sol, 2 * j + 1)};
                    Bj = {get_value(sol, 2 * j + 2 * N), get_value(sol, j + 1 + 2 * N)};

                    argumentj = omega0 * ysj / params::c;
                    Ajs = Aj.dot(s);
                    Bjs = Bj.dot(s);

                    ABjs = Aj * Bjs;
                    BAjs = Bi * Ajs;

                    Pcj = (Ajs * cos(argumentj) - Bjs * sin(argumentj));
                    Psj = (Ajs * sin(argumentj) + Bjs * cos(argumentj));


                    Pc1j = ((s * Ajs * sinth2 - Aj) * cos(argumentj) -
                            (s * Bjs * sinth2 - Bj) * sin(argumentj));
                    Ps1j = ((s * Ajs * sinth2 - Aj) * sin(argumentj) +
                            (s * Bjs * sinth2 - Bj) * cos(argumentj));
                    Pcomj = -(omega0 / params::c) * (sin(theta)) * (ABjs - BAjs);


                    Eigen::Vector<FloatType, 2> rij_xy =
                            Pc1i.cwiseProduct(Pc1j) + Ps1i.cwiseProduct(Ps1j)
                            + 2 * Pcomi.cwiseProduct(Pcomj);

                    FloatType rij_z = (Pci * Pcj + Psi * Psj);
                    resz += rij_z;
                    resxy += rij_xy;
                }
                Eigen::Vector<FloatType, 2> ri_xy =
                        (Pc1i.cwiseProduct(Pc1i) + Ps1i.cwiseProduct(Ps1i)
                         + 2 * Pcomi.cwiseProduct(Pcomi)) / 2.0;
                resxy += ri_xy;

                FloatType ri_xz = (Pci * Pci + Psi * Psi) / 2;
                resz += ri_xz;
            }
            resxy = resxy * T0 * o2 * o2;
            resz = resz * T0 * (o2 * o2 * pow(sin(theta) * cos(theta), 2));
            res = resxy.sum() + resz;
            return res;

        };

    }

    template<typename Container>
    requires HasBracketOperator<Container>
    FloatType Dipoles::getDistance(size_t i1, size_t i2, Container &xi) {
        FloatType d1;
        FloatType d2;
        if constexpr (myConcepts::HasBracketsNested<Container>) {
            d1 = xi[0][i1] - xi[0][i2];
            d2 = xi[1][i1] - xi[1][i2];
        } else {
            d1 = xi[i1] - xi[i2];
            d2 = xi[i1 + N_] - xi[i2 + N_];
        }
        return std::sqrt(d1 * d1 + d2 * d2);
    }


    template<typename Container>
    requires HasBracketOperator<Container>
    Eigen::Vector<FloatType, 2> Dipoles::twoDVecDifference(size_t i1, size_t i2, Container &xi) {
        FloatType d1;
        FloatType d2;
        if constexpr (myConcepts::HasBracketsNested<Container>) {
            d1 = xi[0][i1] - xi[0][i2];
            d2 = xi[1][i1] - xi[1][i2];
        } else {
            d1 = xi[i1] - xi[i2];
            d2 = xi[i1 + N_] - xi[i2 + N_];
        }
        return {d1, d2};
    };
}
