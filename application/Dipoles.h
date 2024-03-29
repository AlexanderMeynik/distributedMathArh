#ifndef DIPLOM_DIPOLES_H
#define DIPLOM_DIPOLES_H
using namespace std;

#include <vector>
#include <memory>
#include <eigen3/Eigen/Dense>
#include <iostream>

using Eigen::MatrixXd;
using Eigen::VectorXd;
using Eigen::Vector2d;
using Eigen::Vector2d;

namespace dipoles {

    static const long double c = 3.0 * pow(10, 8);
    static const long double yo = pow(10, 7);
    static const long double omega = pow(10, 15);
    static const long double omega0 = omega;
    static const long double a = 1;
    static const long double eps = 1;


    template<class T>
    class Dipoles {
    public:
        Dipoles(int N, std::array<std::vector<T>, 2> &xi);

        void setNewCoordinates(std::array<std::vector<T>, 2> &xi);

        void getFullFunction();
        void solve_() {
            Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> tt = (M1_ * M1_ + M2_ * M2_).inverse();
            solution_[0] = tt * (M1_ * f1 + M2_ * f2);
            solution_[1] = tt * (M1_ * f2 - M2_ * f1);
        };
        const function<T(T, T, T)> &getIfunction() const {
            return Ifunction_;
        }
        const function<T(T, T)> &getI2function() const {
            return I2function_;
        }
        const std::array<Eigen::Vector<T, Eigen::Dynamic>, 2> &getSolution_() const;
        std::array<Eigen::Vector<T, Eigen::Dynamic>, 2> getRightPart();
        const Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> &getMatrixx();

    private:
        T getDistance(int i1, int i2) {
            T d1 = xi_[0][i1] - xi_[0][i2];
            T d2 = xi_[1][i1] - xi_[1][i2];

            return std::sqrt(d1 * d1 + d2 * d2);
        };

        Eigen::Vector<T, 2> getRIM(int i, int m) {
            T d1 = xi_[0][i] - xi_[0][m];
            T d2 = xi_[1][i] - xi_[1][m];
            return {d1, d2};
        };

        Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> matrixx;
        Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> M1_;
        Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> M2_;
        Eigen::Vector<T, Eigen::Dynamic> f1;
        Eigen::Vector<T, Eigen::Dynamic> f2;
        std::array<Eigen::Vector<T, Eigen::Dynamic>, 2> solution_;
        std::function<T(T, T, T)> Ifunction_;
        std::function<T(T, T)> I2function_;
        T an = a;
        //std::vector<Eigen::Vector<T,2>> xi_;
        std::array<std::vector<T>, 2> xi_;
        int N_;

        void
        getMatrixes(const Eigen::Vector<T, 2> &rim, T rMode, Eigen::Matrix<T, 2, 2> &K1,
                    Eigen::Matrix<T, 2, 2> &K2) const;

        void setMatrixes();
    };

    template<class T>
    void Dipoles<T>::setNewCoordinates(std::array<std::vector<T>, 2> &xi) {
        xi_ = xi;

        setMatrixes();


        /*for (int i = 0; i < N_; ++i) {//todo эта часть уже не пустая
            f1(2 * i) = an * eps;
            f1(2 * i + 1) = 0;
            f2(2 * i) = 0;
            f2(2 * i + 1) = an * eps;
        }*/
    }

    template<class T>
    void Dipoles<T>::setMatrixes() {
        vector<pair<int, int>> sectors(4);
        sectors[0] = {0, 0};
        sectors[1] = {0, 2 * N_};
        sectors[2] = {2 * N_, 0};
        sectors[3] = {2 * N_, 2 * N_};
        for (int I = 0; I < N_; ++I) {//MAC
            for (int M = 0; M < N_; ++M) {
                if (I == M) {
                    auto id = Eigen::Matrix<T, 2, 2>::Identity() * (omega0 * omega0 - omega * omega);
                    M1_.block(2 * I, 2 * M, 2, 2) = id;
                } else {

                    Eigen::Vector<T, 2> rim = getRIM(M, I);
                    T rMode = getDistance(I, M);
                    Eigen::Matrix<T, 2, 2> K1;
                    Eigen::Matrix<T, 2, 2> K2;
                    getMatrixes(rim, rMode, K1, K2);
                    T arg = omega * rMode / c;
                    auto tmpmatr = -an * (K1 * cos(arg) - K2 * sin(arg));
                    M1_.block(2 * I, 2 * M, 2, 2) = tmpmatr;
                }

            }
        }

        for (int I = 0; I < N_; ++I) {//MBC
            for (int M = 0; M < N_; ++M) {
                if (I == M) {
                    auto id = Eigen::Matrix<T, 2, 2>::Identity() * (yo * omega);
                    M2_.block(2 * I, 2 * M, 2, 2) = -id;
                } else {

                    Eigen::Vector<T, 2> rim = getRIM(M, I);
                    T rMode = getDistance(I, M);
                    Eigen::Matrix<T, 2, 2> K1;
                    Eigen::Matrix<T, 2, 2> K2;
                    getMatrixes(rim, rMode, K1, K2);
                    T arg = omega * rMode / c;
                    auto tmpmatr = -an * (K2 * cos(arg) + K1 * sin(arg));

                    M2_.block(2 * I, 2 * M, 2, 2) = tmpmatr;
                }

            }
        }
    }

    template<class T>
    const array<Eigen::Vector<T, Eigen::Dynamic>, 2> &Dipoles<T>::getSolution_() const {
        return solution_;//todo может лучше возвращать hsared pointer
    }

    template<class T>
    void Dipoles<T>::getFullFunction(){

        this->Ifunction_ = [this](T theta, T phi, T t) {
            int N = this->xi_[0].size();
            T res = 0;
            T s[2] = {cos(phi), sin(phi)};
            T ress[3] = {0, 0, 0};
            for (int i = 0; i < N; ++i) {
                T ri[2] = {this->xi_[0][i], this->xi_[1][i]};
                T ys = (ri[1] * cos(phi) - ri[0] * sin(phi)) * sin(theta);
                T t0 = t - ys / c;
                T Ai[2] = {this->solution_[0].coeffRef(2 * i), this->solution_[0].coeffRef(2 * i + 1)};
                T Bi[2] = {this->solution_[1].coeffRef(2 * i), this->solution_[1].coeffRef(2 * i + 1)};


                T Di[2] = {Ai[0] * cos((T)omega * t0) + Bi[0] * sin((T)omega * t0),
                           Ai[1] * cos((T)omega * t0) + Bi[1] * sin((T)omega * t0)};

                T vi[2] = {(T)omega * (Bi[0] * cos((T)omega * t0) - Ai[0] * sin((T)omega * t0)) / c,
                           (T)omega * (Bi[1] * cos((T)omega * t0) - Ai[1] * sin((T)omega)) / c};
                T ai[2] = {-pow((T)omega, 2) * Di[0], -pow((T)omega, 2) * Di[1]};

                T vsi = vi[0] * s[0] + vi[1] * s[1];
                T asi = ai[0] * s[0] + ai[1] * s[1];

                for (int coord = 0; coord < 2; ++coord) {
                    T ttt = ai[coord] * (vsi * sin(theta) - 1) +
                            s[coord] * asi * pow(sin(theta), 2) -
                            vi[coord] * asi * sin(theta);
                    ress[coord] += ttt;
                }

                T t3 = asi * sin(theta) * cos(theta);
                ress[2] += t3;
            }
            for (T elem: ress) {
                res += elem * elem;
            }
            return res;
        };
        this->I2function_ = [this](T phi, T theta) {//todo анал решение 
            return theta+0*phi;
        };
    }


    template<class T>
    void Dipoles<T>::getMatrixes(const Eigen::Vector<T, 2> &rim, T rMode, Eigen::Matrix<T, 2, 2> &K1,
                                 Eigen::Matrix<T, 2, 2> &K2) const {
        K1 << 3 * rim(0) * rim(0) / pow(rMode, 5) - 1 / pow(rMode, 3), 3 * rim(0) * rim(1) / pow(rMode, 5),
                3 * rim(0) * rim(1) / pow(rMode, 5), 3 * rim(1) * rim(1) / pow(rMode, 5) - 1 / pow(rMode, 3);

        K2 << omega / (c * pow(rMode, 2)), 0,
                0, omega / (c * pow(rMode, 2));
    }

    template<class T>
    const Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> &Dipoles<T>::getMatrixx() {

        matrixx.resize(4 * N_, 4 * N_);
        matrixx.topLeftCorner(2 * N_, 2 * N_).noalias() = M1_;
        //std::cout<<M1_<<"\n\n\n\n"<<M2_<<"\n\n\n\n\n";
        matrixx.topRightCorner(2 * N_, 2 * N_).noalias() = -M2_;
        matrixx.bottomLeftCorner(2 * N_, 2 * N_).noalias() = M2_;
        matrixx.bottomRightCorner(2 * N_, 2 * N_).noalias() = M1_;

        return matrixx;
    }

    template<class T>
    std::array<Eigen::Vector<T, Eigen::Dynamic>, 2> Dipoles<T>::getRightPart() {

        return {f1, f2};
    }

    template<class T>
    Dipoles<T>::Dipoles(int N, std::array<std::vector<T>, 2> &xi):N_(
            N) {
        xi_ = xi;
        an = a / N_;
        M1_.resize(2 * N_, 2 * N_);
        M2_.resize(2 * N_, 2 * N_);

        setMatrixes();


        f1.resize(2 * N_);
        f2.resize(2 * N_);
        for (int i = 0; i < N_; ++i) {
            f1(2 * i) = an * eps;
            f1(2 * i + 1) = 0;
            f2(2 * i) = 0;
            f2(2 * i + 1) = an * eps;
        }
    }

}
#endif //DIPLOM_DIPOLES_H
