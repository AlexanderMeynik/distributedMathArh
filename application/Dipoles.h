#ifndef DIPLOM_DIPOLES_H
#define DIPLOM_DIPOLES_H


#include <vector>
#include <memory>
#include <eigen3/Eigen/Dense>
#include <iostream>
#include "printUtils.h"
#include <iomanip>
#include <matplot/matplot.h>

using Eigen::MatrixXd;
using Eigen::VectorXd;
using Eigen::Vector2d;
using Eigen::Vector2d;

namespace dipoles {
    using matplot::gca;
    using std::function, std::pair, std::vector, std::array;




    template<class T>
    class Dipoles {
    public:
        //todo метод для импорта/экспорта данных
        //определить где и как будем хранить
        Dipoles() = default;

        Dipoles(int N, std::array<std::vector<T>, 2> &xi);

        void setNewCoordinates(std::array<std::vector<T>, 2> &xi);
        //todo кто будет собственником x_i(нужно ли его вообще здесь хранить)
        void setSolution(std::array<Eigen::Vector<T, Eigen::Dynamic>, 2> &sol);

        void getFullFunction(const std::array<std::vector<T>, 2> &xi,
                             const std::array<Eigen::Vector<T, Eigen::Dynamic>, 2> &sol);

        std::array<Eigen::Vector<T, Eigen::Dynamic>, 2> solve_() {
            auto tt = (M1_ * M1_ + M2_ * M2_).lu();
            std::array<Eigen::Vector<T, Eigen::Dynamic>, 2> solution_;
            solution_[0] = tt.solve(M1_ * f1 + M2_ * f2);
            solution_[1] = tt.solve(M1_ * f2 - M2_ * f1);
            return solution_;
        };

        const function<T(T, T, T)> &getIfunction() const {
            return Ifunction_;
        }

        const function<T(T, T)> &getI2function() const {
            return I2function_;
        }


        std::array<Eigen::Vector<T, Eigen::Dynamic>, 2> getRightPart();

        const Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> &getMatrixx();


        void printMatrix(std::ostream &out, Eigen::IOFormat &format);
        //todo move
        void printCoordinates(std::ostream &out,std::array<std::vector<T>, 2> &xi);

        void printRightPart(std::ostream &out, Eigen::IOFormat &format);
        //todo move
        void printSolution(std::ostream &out, Eigen::IOFormat &format,std::array<Eigen::Vector<T, Eigen::Dynamic>, 2>& solution_);
          //todo move
        void printSolutionFormat1(std::ostream &out,std::array<Eigen::Vector<T, Eigen::Dynamic>, 2>& solution_);
        //todo move
        void plotCoordinates(std::string name, T ar,std::array<std::vector<T>, 2> &xi);

        static constexpr T c = 3.0 * 1e8;
        static constexpr T yo = 1e7;
        static constexpr T omega = 1e15;
        static constexpr T omega0 = omega;
        static constexpr T a = 1;
        static constexpr T eps = 1;
    private:
        T getDistance(int i1, int i2,std::array<std::vector<T>, 2> &xi) {
            T d1 = xi[0][i1] - xi[0][i2];
            T d2 = xi[1][i1] - xi[1][i2];

            return std::sqrt(d1 * d1 + d2 * d2);
        };

        Eigen::Vector<T, 2> getRIM(int i, int m,std::array<std::vector<T>, 2> &xi) {
            T d1 = xi[0][i] - xi[0][m];
            T d2 = xi[1][i] - xi[1][m];
            return {d1, d2};
        };

        void initArrays(std::array<std::vector<T>, 2> &xi) {
            //xi_ = xi;
            an = a / N_;
            M1_.resize(2 * N_, 2 * N_);
            M2_.resize(2 * N_, 2 * N_);


            f1.resize(2 * N_);
            f2.resize(2 * N_);
            for (int i = 0; i < N_; ++i) {
                f1(2 * i) = an * eps;
                f1(2 * i + 1) = 0;
                f2(2 * i) = 0;
                f2(2 * i + 1) = an * eps;
            }
        }
        //std::array<std::vector<T>, 2> xi_;
        Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> M1_;
        Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> M2_;
        Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> matrixx;
        //std::array<Eigen::Vector<T, Eigen::Dynamic>, 2> solution_;

        std::function<T(T, T, T)> Ifunction_;
        std::function<T(T, T)> I2function_;
        Eigen::Vector<T, Eigen::Dynamic> f1;
        Eigen::Vector<T, Eigen::Dynamic> f2;

        T an = a;
        int N_;

        void
        getMatrixes(const Eigen::Vector<T, 2> &rim, T rMode, Eigen::Matrix<T, 2, 2> &K1,
                    Eigen::Matrix<T, 2, 2> &K2) const;

        void setMatrixes(std::array<std::vector<T>, 2> &xi);
    };




    template<class T>
    void Dipoles<T>::plotCoordinates(std::string name, T ar,std::array<std::vector<T>, 2> &xi) {
        auto ax = gca();
        ax->scatter(xi[0], xi[1]);
        ax->xlim({-8 * ar, 8 * ar});
        ax->ylim({-8 * ar, 8 * ar});
        matplot::save(name);
        ax.reset();
    }

    template<class T>
    void Dipoles<T>::printSolutionFormat1(std::ostream &out,std::array<Eigen::Vector<T, Eigen::Dynamic>, 2>& solution_) {
        out << "Решение системы диполей\n Ai(x\\ny)\tBi(x\\ny)\tCi(x\\ny)\n";
        for (int i = 0; i < N_; ++i) {
            auto cx = sqrt(solution_[0].coeffRef(2 * i) * solution_[0].coeffRef(2 * i) +
                           solution_[1].coeffRef(2 * i) * solution_[1].coeffRef(2 * i));
            auto cy = sqrt(solution_[0].coeffRef(2 * i + 1) * solution_[0].coeffRef(2 * i + 1) +
                           solution_[1].coeffRef(2 * i + 1) * solution_[1].coeffRef(2 * i + 1));
            IosStatePreserve state(out);
            out << std::scientific;

            out << solution_[0].coeffRef(2 * i) << "\t"
                << solution_[1].coeffRef(2 * i) << "\t"
                << cx << "\n";
            out << solution_[0].coeffRef(2 * i + 1) << "\t"
                << solution_[1].coeffRef(2 * i + 1) << "\t"
                << cy << "\n";
        }
    }

    template<class T>
    void Dipoles<T>::printSolution(std::ostream &out, Eigen::IOFormat &format,std::array<Eigen::Vector<T, Eigen::Dynamic>, 2>& solution_) {
        out << "Вектор решения\n" << solution_[0].format(format) << '\n' << solution_[1].format(format) << "\n\n";
    }

    template<class T>
    void Dipoles<T>::printRightPart(std::ostream &out, Eigen::IOFormat &format) {
        out << "Правая часть\n" << this->f1.format(format) << '\n' << this->f2.format(format)
            << "\n\n";
    }

    template<class T>
    void Dipoles<T>::printCoordinates(std::ostream &out,std::array<std::vector<T>, 2> &xi) {
        out << "Координаты диполей\n";
        for (int i = 0; i < xi[0].size(); ++i) {
            out << xi[0][i] << '\t' << xi[1][i] << "\n";
        }
    }

    template<class T>
    void Dipoles<T>::printMatrix(std::ostream &out, Eigen::IOFormat &format) {
        out << "Матрица\n" << getMatrixx().format(format) << "\n\n";
    }

    template<class T>
    void Dipoles<T>::setNewCoordinates(std::array<std::vector<T>, 2> &xi) {
        if (an==a) {
            this->N_ = xi[0].size();
            initArrays(xi);
        }
        setMatrixes(xi);
    }

    template<class T>
    void Dipoles<T>::setSolution(std::array<Eigen::Vector<T, Eigen::Dynamic>, 2> &sol) {//todo по большому счёту функцию можно вынуть
        this->solution_ = sol;
        this->N_ = sol[0].size();
    }

    template<class T>
    void Dipoles<T>::setMatrixes(std::array<std::vector<T>, 2> &xi) {
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

                    Eigen::Vector<T, 2> rim = getRIM(M, I,xi);
                    T rMode = getDistance(I, M,xi);
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

                    Eigen::Vector<T, 2> rim = getRIM(M, I,xi);
                    T rMode = getDistance(I, M,xi);
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
    void Dipoles<T>::getFullFunction(const std::array<std::vector<T>, 2> &xi,
                                     const std::array<Eigen::Vector<T, Eigen::Dynamic>, 2> &sol) {
        this->Ifunction_ = [&xi,&sol](T theta, T phi, T t) {
            int N = xi[0].size();
            T res = 0;
            T s[2] = {cos(phi), sin(phi)};
            T ress[3] = {0, 0, 0};
            for (int i = 0; i < N; ++i) {
                T ri[2] = {xi[0][i], xi[1][i]};
                T ys = (ri[1] * cos(phi) - ri[0] * sin(phi)) * sin(theta);
                T t0 = t - ys / c;
                T Ai[2] = {sol[0].coeffRef(2 * i), sol[0].coeffRef(2 * i + 1)};
                T Bi[2] = {sol[1].coeffRef(2 * i), sol[1].coeffRef(2 * i + 1)};


                T Di[2] = {Ai[0] * cos((T) omega * t0) + Bi[0] * sin((T) omega * t0),
                           Ai[1] * cos((T) omega * t0) + Bi[1] * sin((T) omega * t0)};

                T vi[2] = {(T) omega * (Bi[0] * cos((T) omega * t0) - Ai[0] * sin((T) omega * t0)) / c,
                           (T) omega * (Bi[1] * cos((T) omega * t0) - Ai[1] * sin((T) omega)) / c};
                T ai[2] = {-pow((T) omega, 2) * Di[0], -pow((T) omega, 2) * Di[1]};

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


        this->I2function_ = [&xi,&sol](T phi, T theta) {
            int N = xi[0].size();
            T omega0 = omega;
            T T0 = M_PI * 2 / omega0;
            T res;
            Eigen::Vector<T, 2> resxy = {0.0, 0.0};
            T resz = 0.0;
            T o3dc = pow(omega0, 3) / c;
            T o2 = pow(omega0, 2);
            T sinth2 = pow(sin(theta), 2);
            Eigen::Vector<T, 2> s = {cos(phi),
                                     sin(phi)};
            for (int i = 0; i < N; ++i) {
                Eigen::Vector<T, 2> ri = {xi[0][i],
                                          xi[1][i]};
                T ys = (ri[1] * cos(phi) - ri[0] * sin(phi)) * sin(theta);

                Eigen::Vector<T, 2> Ai = {sol[0].coeffRef(2 * i),
                                          sol[0].coeffRef(2 * i + 1)};
                Eigen::Vector<T, 2> Bi = {sol[1].coeffRef(2 * i),
                                          sol[1].coeffRef(2 * i + 1)};
                T argument = omega0 * ys / c;
                T Ais = Ai.dot(s);
                T Bis = Bi.dot(s);
                Eigen::Vector<T, 2> ABis = Ai * Bis;
                Eigen::Vector<T, 2> BAis = Bi * Ais;

                Eigen::Vector<T, 2> Pc1i = ((s * Ais * sinth2 - Ai) * cos(argument) -
                                            (s * Bis * sinth2 - Bi) * sin(argument));
                Eigen::Vector<T, 2> Ps1i = ((s * Ais * sinth2 - Ai) * sin(argument) +
                                            (s * Bis * sinth2 - Bi) * cos(argument));
                Eigen::Vector<T, 2> Pcomi = -(omega0 / c) * (sin(theta)) * (ABis - BAis);

                T Pci = (Ais * cos(argument) - Bis * sin(argument));
                T Psi = (Ais * sin(argument) + Bis * cos(argument));

                Eigen::Vector<T, 2> rj;
                T ysj;
                Eigen::Vector<T, 2> Aj;
                Eigen::Vector<T, 2> Bj;
                T argumentj;

                T Ajs;
                T Bjs;


                T Pcj;
                T Psj;


                Eigen::Vector<T, 2> Pc1j;
                Eigen::Vector<T, 2> Ps1j;
                Eigen::Vector<T, 2> Pcomj;


                Eigen::Vector<T, 2> ABjs;
                Eigen::Vector<T, 2> BAjs;
                for (int j = 0; j < i; ++j) {

                    rj = {xi[0][j],
                          xi[1][j]};
                    ysj = (rj[1] * cos(phi) - rj[0] * sin(phi)) * sin(theta);
                    Aj = {sol[0].coeffRef(2 * j),
                          sol[0].coeffRef(2 * j + 1)};
                    Bj = {sol[1].coeffRef(2 * j),
                          sol[1].coeffRef(2 * j + 1)};

                    argumentj = omega0 * ysj / c;
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
                    Pcomj = -(omega0 / c) * (sin(theta)) * (ABjs - BAjs);


                    Eigen::Vector<T, 2> rij_xy =
                            Pc1i.cwiseProduct(Pc1j) + Ps1i.cwiseProduct(Ps1j)
                            + 2 * Pcomi.cwiseProduct(Pcomj);

                    T rij_z = (Pci * Pcj + Psi * Psj);
                    resz += rij_z;
                    resxy += rij_xy;
                }
                Eigen::Vector<T, 2> ri_xy =
                        (Pc1i.cwiseProduct(Pc1i) + Ps1i.cwiseProduct(Ps1i)
                         + 2 * Pcomi.cwiseProduct(Pcomi)) / 2.0;
                resxy += ri_xy;

                T ri_xz = (Pci * Pci + Psi * Psi) / 2;
                resz += ri_xz;
            }
            resxy = resxy * T0 * o2 * o2;
            resz = resz * T0 * (o2 * o2 * pow(sin(theta) * cos(theta), 2));
            res = resxy.sum() + resz;
            return res;

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
        initArrays(xi);
        setMatrixes(xi);
    }

}
#endif //DIPLOM_DIPOLES_H
