#ifndef DIPLOM_DIPOLES_H
#define DIPLOM_DIPOLES_H


#include <vector>
#include <memory>
#include <eigen3/Eigen/Dense>
#include <iostream>

#include <iomanip>

#include "const.h"

using Eigen::MatrixXd;
using Eigen::VectorXd;
using Eigen::Vector2d;
using Eigen::Vector2d;

namespace dipoles {
    using matplot::gca;
    using std::function, std::pair, std::vector, std::array;


    template<class T>
    static bool isSymmetric(Eigen::Matrix<T, -1, -1> &matr) {
        size_t N = matr.size();

        for (int i = 0; i < N; ++i) {

            for (int j = 0; j < i; ++j) {
                if (matr.coeffRef(i, j) != matr.coeffRef(j, i)) {
                    return false;
                }
            }

        }
        return true;
    }


    template<class T>
    static bool isSymmetric(Eigen::Matrix<T, -1, -1> &&matr)//todo проверить функцю
    {
        size_t N = matr.size();

        for (int i = 0; i < N; ++i) {

            for (int j = 0; j < i; ++j) {
                if (matr.coeffRef(i, j) != matr.coeffRef(j, i)) {
                    return false;
                }
            }

        }
        return true;
    }


#include <cassert>
#include <eigen3/Eigen/Geometry>

    template<class T>
    class Dipoles {
    public:
        //todo метод для импорта/экспорта данных
        //определить где и как будем хранить
        Dipoles() = default;

        Dipoles(int N, Eigen::Vector<T, Eigen::Dynamic> &xi);

        Dipoles(int N, std::vector<T> &xi);

        void setNewCoordinates(Eigen::Vector<T, Eigen::Dynamic> &xi);

        void loadFromMatrix(Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> &xi);

        void getFullFunction(const Eigen::Vector<T, Eigen::Dynamic> &xi,
                             const Eigen::Vector<T, Eigen::Dynamic> &sol);//поидее нам не нужна coefRef индексация

        void getFullFunction3(const std::vector<T> &xi,
                              const std::vector<T> &sol);//todo introduce function generator
        void getFullFunction2(const std::array<std::vector<T>, 2> &xi,
                              const std::array<Eigen::Vector<T, Eigen::Dynamic>, 2> &sol);





        std::array<Eigen::Vector<T, Eigen::Dynamic>, 2> solve2() {

            // auto tt = (M1_ * M1_ + M2_ * M2_).lu();//todo посомотреть как auto влияет на наши вещи
            Eigen::PartialPivLU tt = (M1_ * M1_ + M2_ * M2_).lu();
            Eigen::Vector<T, Eigen::Dynamic> solution_1;
            Eigen::Vector<T, Eigen::Dynamic> solution_2;
            solution_1.resize(2 * N_);
            solution_2.resize(2 * N_);
            solution_1 = tt.solve(M1_ * f.block(0, 0, 2 * N_, 1) + M2_ * f.block(2 * N_, 0, 2 * N_, 1));
            solution_2 = tt.solve(M1_ * f.block(2 * N_, 0, 2 * N_, 1) - M2_ * f.block(0, 0, 2 * N_, 1));
            return {solution_1, solution_2};
        }


        Eigen::Vector<T, Eigen::Dynamic> solve3() {
            // auto tt = (M1_ * M1_ + M2_ * M2_).lu();
            Eigen::PartialPivLU tt = (M1_ * M1_ + M2_ * M2_).lu();
            Eigen::Vector<T, Eigen::Dynamic> solution_;
            solution_.resize(4 * N_);
            solution_.block(0, 0, 2 * N_, 1) = tt.solve(
                    M1_ * f.block(0, 0, 2 * N_, 1) + M2_ * f.block(2 * N_, 0, 2 * N_, 1));
            solution_.block(2 * N_, 0, 2 * N_, 1) = tt.solve(
                    M1_ * f.block(2 * N_, 0, 2 * N_, 1) - M2_ * f.block(0, 0, 2 * N_, 1));
            return solution_;
        }

        std::vector<T> solve4() {

            std::vector<T> sol(4 * N_);
            Eigen::PartialPivLU<Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>> tt = (M1_ * M1_ + M2_ * M2_).lu();
            Eigen::Map<Eigen::Vector<T, Eigen::Dynamic>> solution_(sol.data(), sol.size());
            solution_.resize(4 * N_);
            solution_.block(0, 0, 2 * N_, 1) = tt.solve(
                    M1_ * f.block(0, 0, 2 * N_, 1) + M2_ * f.block(2 * N_, 0, 2 * N_, 1));
            solution_.block(2 * N_, 0, 2 * N_, 1) = tt.solve(
                    M1_ * f.block(2 * N_, 0, 2 * N_, 1) - M2_ * f.block(0, 0, 2 * N_, 1));
            return sol;

            /*Eigen::PartialPivLU tt= (M1_ * M1_ + M2_ * M2_).lu();
            Eigen::Vector<T, Eigen::Dynamic> solution_;
            solution_.resize(4*N_);
            solution_.block(0,0,2*N_,1) = tt.solve(M1_ * f.block(0,0,2*N_,1) + M2_ * f.block(2*N_,0,2*N_,1));
            solution_.block(2*N_,0,2*N_,1) = tt.solve(M1_ * f.block(2*N_,0,2*N_,1) - M2_ * f.block(0,0,2*N_,1));
            std::vector<T> sol(solution_.size());
            for (int i = 0; i < sol.size(); ++i) {
                sol[i]=solution_[i];
            }
            return sol;*/
        }


        const function<T(T, T, T)> &getIfunction() const {
            return Ifunction_;
        }

        const function<T(T, T)> &getI2function() const {
            return I2function_;
        }


        Eigen::Vector<T, Eigen::Dynamic> &getRightPart2();

        Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> getMatrixx();


        void printMatrix(std::ostream &out, Eigen::IOFormat &format);

        void printRightPart(std::ostream &out, Eigen::IOFormat &format);

    private:

        template<typename... Args, template<typename...> typename Container>
        T getDistance(int i1, int i2, Container<Args...> &xi) {
            T d1 = xi[i1] - xi[i2];
            T d2 = xi[i1 + N_] - xi[i2 + N_];

            return std::sqrt(d1 * d1 + d2 * d2);
        };


        template<typename... Args, template<typename...> typename Container>
        Eigen::Vector<T, 2> getRIM(int i, int m, Container<Args...> &xi) {
            T d1 = xi[i] - xi[m];
            T d2 = xi[i + N_] - xi[m + N_];
            return {d1, d2};
        };

        void initArrays() {

            an = params<T>::a / N_;
            M1_.resize(2 * N_, 2 * N_);
            M2_.resize(2 * N_, 2 * N_);


            f.resize(4 * N_);
            for (int i = 0; i < N_; ++i) {
                f(2 * i) = an * params<T>::eps;
                f(2 * i + 1) = 0;
                f(2 * i + 2 * N_) = 0;
                f(2 * i + 1 + 2 * N_) = an * params<T>::eps;
            }
        }

        Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> M1_;
        Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> M2_;


        std::function<T(T, T, T)> Ifunction_;
        std::function<T(T, T)> I2function_;

        Eigen::Vector<T, Eigen::Dynamic> f;

        T an = params<T>::a;
        int N_;

        void
        getMatrixes(const Eigen::Vector<T, 2> &rim, T rMode, Eigen::Matrix<T, 2, 2> &K1,
                    Eigen::Matrix<T, 2, 2> &K2) const;

        template<typename ... Args, template<typename ...> typename Container>
        //todo создать коцепт для котнейнера сметодами индексации
        void setMatrixes(Container<Args ...> &xi) {
            vector<pair<int, int>> sectors(4);
            sectors[0] = {0, 0};
            sectors[1] = {0, 2 * N_};
            sectors[2] = {2 * N_, 0};
            sectors[3] = {2 * N_, 2 * N_};
            for (int I = 0; I < N_; ++I) {//MAC
                for (int M = 0; M < N_; ++M) {
                    if (I == M) {
                        Eigen::Matrix<T, 2, 2> id = Eigen::Matrix<T, 2, 2>::Identity() *
                                                    (params<T>::omega0 * params<T>::omega0 -
                                                     params<T>::omega * params<T>::omega);
                        M1_.block(2 * I, 2 * M, 2, 2) = id;//todo block are 2 by 2
                    } else {

                        Eigen::Vector<T, 2> rim = getRIM(M, I, xi);
                        T rMode = getDistance(I, M, xi);
                        Eigen::Matrix<T, 2, 2> K1;
                        Eigen::Matrix<T, 2, 2> K2;
                        getMatrixes(rim, rMode, K1, K2);
                        T arg = params<T>::omega * rMode / params<T>::c;
                        Eigen::Matrix<T, 2, 2> tmpmatr = -an * (K1 * cos(arg) - K2 * sin(arg));
                        M1_.block(2 * I, 2 * M, 2, 2) = tmpmatr;
                    }
                }
            }

            for (int I = 0; I < N_; ++I) {//MBC
                for (int M = 0; M < N_; ++M) {
                    if (I == M) {
                        Eigen::Matrix<T, 2, 2> id =
                                Eigen::Matrix<T, 2, 2>::Identity() * (params<T>::yo * params<T>::omega);
                        M2_.block(2 * I, 2 * M, 2, 2) = -id;
                    } else {

                        Eigen::Vector<T, 2> rim = getRIM(M, I, xi);
                        T rMode = getDistance(I, M, xi);
                        Eigen::Matrix<T, 2, 2> K1;
                        Eigen::Matrix<T, 2, 2> K2;
                        getMatrixes(rim, rMode, K1, K2);
                        T arg = params<T>::omega * rMode / params<T>::c;

                        Eigen::Matrix<T, 2, 2> tmpmatr = -an * (K2 * cos(arg) + K1 * sin(arg));

                        M2_.block(2 * I, 2 * M, 2, 2) = tmpmatr;
                    }

                }
            }
        }
    };


    template<class T>
    void Dipoles<T>::printRightPart(std::ostream &out, Eigen::IOFormat &format) {
        out << "Правая часть\n" << this->f.format(format)
            << "\n\n";
    }


    template<class T>
    void Dipoles<T>::printMatrix(std::ostream &out, Eigen::IOFormat &format) {
        out << "Матрица\n" << getMatrixx().format(format) << "\n\n";
    }

    template<class T>
    void Dipoles<T>::setNewCoordinates(Eigen::Vector<T, Eigen::Dynamic> &xi) {
        if (an == params<T>::a) {
            this->N_ = xi.size() / 2;
            initArrays();
        }
        setMatrixes(xi);
    }

    template<class T>
    void Dipoles<T>::loadFromMatrix(Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> &xi) {
        this->N_ = xi.rows()/4;
        initArrays();

        M1_=xi.topLeftCorner(2 * N_, 2 * N_);
        M2_=xi.bottomLeftCorner(2 * N_, 2 * N_);
    }


    /* // Specialization for std::array<Eigen::Vector<T, Eigen::Dynamic>, 2>
     template <typename T>
     std::array<Eigen::Vector<T, Eigen::Dynamic>, 2> Dipoles<T>::solveFinal<Eigen::Vector<T, Eigen::Dynamic>, 2,std::array>()
     {
         Eigen::PartialPivLU<Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>> tt = (M1_ * M1_ + M2_ * M2_).lu();
         Eigen::Vector<T, Eigen::Dynamic> solution_1(2 * N_);
         Eigen::Vector<T, Eigen::Dynamic> solution_2(2 * N_);
         solution_1 = tt.solve(M1_ * f.block(0, 0, 2 * N_, 1) + M2_ * f.block(2 * N_, 0, 2 * N_, 1));
         solution_2 = tt.solve(M1_ * f.block(2 * N_, 0, 2 * N_, 1) - M2_ * f.block(0, 0, 2 * N_, 1));
         return {solution_1, solution_2};
     }

 // Specialization for Eigen::Vector<T, Eigen::Dynamic>
     template <typename T>
     Eigen::Vector<T, Eigen::Dynamic> Dipoles<T>::solveFinal<T, Eigen::Dynamic,Eigen::Vector>()
     {
         Eigen::PartialPivLU<Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>> tt = (M1_ * M1_ + M2_ * M2_).lu();
         Eigen::Vector<T, Eigen::Dynamic> solution_(4 * N_);
         solution_.block(0, 0, 2 * N_, 1) = tt.solve(M1_ * f.block(0, 0, 2 * N_, 1) + M2_ * f.block(2 * N_, 0, 2 * N_, 1));
         solution_.block(2 * N_, 0, 2 * N_, 1) = tt.solve(M1_ * f.block(2 * N_, 0, 2 * N_, 1) - M2_ * f.block(0, 0, 2 * N_, 1));
         return solution_;
     }

 // Specialization for std::vector<T>

     template <typename T>
     std::vector<T> Dipoles<T>::solveFinal<T,std::vector>()
     {
         std::vector<T> sol(4 * N_);
         Eigen::PartialPivLU<Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>> tt = (M1_ * M1_ + M2_ * M2_).lu();
         Eigen::Map<Eigen::Vector<T, Eigen::Dynamic>> solution_(sol.data(), sol.size());
         solution_.resize(4 * N_);
         solution_.block(0, 0, 2 * N_, 1) = tt.solve(M1_ * f.block(0, 0, 2 * N_, 1) + M2_ * f.block(2 * N_, 0, 2 * N_, 1));
         solution_.block(2 * N_, 0, 2 * N_, 1) = tt.solve(M1_ * f.block(2 * N_, 0, 2 * N_, 1) - M2_ * f.block(0, 0, 2 * N_, 1));
         return sol;
     }*/




    template<class T>
    void Dipoles<T>::getFullFunction2(const std::array<std::vector<T>, 2> &xi,
                                      const std::array<Eigen::Vector<T, Eigen::Dynamic>, 2> &sol) {
        this->Ifunction_ = [&xi, &sol](T theta, T phi, T t) {
            int N = xi[0].size();
            T res = 0;
            T s[2] = {cos(phi), sin(phi)};
            T ress[3] = {0, 0, 0};
            for (int i = 0; i < N; ++i) {
                T ri[2] = {xi[0][i], xi[1][i]};
                T ys = (ri[1] * cos(phi) - ri[0] * sin(phi)) * sin(theta);
                T t0 = t - ys / params<T>::c;
                T Ai[2] = {sol[0].coeffRef(2 * i), sol[0].coeffRef(2 * i + 1)};
                T Bi[2] = {sol[1].coeffRef(2 * i), sol[1].coeffRef(2 * i + 1)};


                T Di[2] = {Ai[0] * cos((T) params<T>::omega * t0) + Bi[0] * sin((T) params<T>::omega * t0),
                           Ai[1] * cos((T) params<T>::omega * t0) + Bi[1] * sin((T) params<T>::omega * t0)};

                T vi[2] = {(T) params<T>::omega *
                           (Bi[0] * cos((T) params<T>::omega * t0) - Ai[0] * sin((T) params<T>::omega * t0)) /
                           params<T>::c,
                           (T) params<T>::omega *
                           (Bi[1] * cos((T) params<T>::omega * t0) - Ai[1] * sin((T) params<T>::omega)) / params<T>::c};
                T ai[2] = {-pow((T) params<T>::omega, 2) * Di[0], -pow((T) params<T>::omega, 2) * Di[1]};

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


        this->I2function_ = [&xi, &sol](T phi, T theta) {
            int N = xi[0].size();
            T omega0 = params<T>::omega;
            T T0 = M_PI * 2 / omega0;
            T res;
            Eigen::Vector<T, 2> resxy = {0.0, 0.0};
            T resz = 0.0;
            T o3dc = pow(omega0, 3) / params<T>::c;
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
                T argument = omega0 * ys / params<T>::c;
                T Ais = Ai.dot(s);
                T Bis = Bi.dot(s);
                Eigen::Vector<T, 2> ABis = Ai * Bis;
                Eigen::Vector<T, 2> BAis = Bi * Ais;

                Eigen::Vector<T, 2> Pc1i = ((s * Ais * sinth2 - Ai) * cos(argument) -
                                            (s * Bis * sinth2 - Bi) * sin(argument));
                Eigen::Vector<T, 2> Ps1i = ((s * Ais * sinth2 - Ai) * sin(argument) +
                                            (s * Bis * sinth2 - Bi) * cos(argument));
                Eigen::Vector<T, 2> Pcomi = -(omega0 / params<T>::c) * (sin(theta)) * (ABis - BAis);

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

                    argumentj = omega0 * ysj / params<T>::c;
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
                    Pcomj = -(omega0 / params<T>::c) * (sin(theta)) * (ABjs - BAjs);


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
    void Dipoles<T>::getFullFunction3(const vector<T> &xi, const vector<T> &sol) {
        this->Ifunction_ = [&xi, &sol](T theta, T phi, T t) {
            int N = xi.size() / 2;
            T res = 0;
            T s[2] = {cos(phi), sin(phi)};
            T ress[3] = {0, 0, 0};
            for (int i = 0; i < N; ++i) {
                T ri[2] = {xi[i], xi[i + N]};
                T ys = (ri[1] * cos(phi) - ri[0] * sin(phi)) * sin(theta);
                T t0 = t - ys / params<T>::c;
                T Ai[2] = {sol[2 * i], sol[2 * i + 1]};
                T Bi[2] = {sol[2 * i + 2 * N], sol[2 * i + 1 + 2 * N]};


                T Di[2] = {Ai[0] * cos(params<T>::omega * t0) + Bi[0] * sin(params<T>::omega * t0),
                           Ai[1] * cos(params<T>::omega * t0) + Bi[1] * sin(params<T>::omega * t0)};

                T vi[2] = {params<T>::omega * (Bi[0] * cos(params<T>::omega * t0)
                                               - Ai[0] * sin(params<T>::omega * t0)) / params<T>::c,
                           params<T>::omega * (Bi[1] * cos(params<T>::omega * t0)
                                               - Ai[1] * sin(params<T>::omega)) / params<T>::c};
                T ai[2] = {-pow(params<T>::omega, 2) * Di[0], -pow(params<T>::omega, 2) * Di[1]};

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


        this->I2function_ = [&xi, &sol](T phi, T theta) {
            int N = xi.size() / 2;
            T omega0 = params<T>::omega;
            T T0 = M_PI * 2 / omega0;
            T res;
            Eigen::Vector<T, 2> resxy = {0.0, 0.0};
            T resz = 0.0;
            T o3dc = pow(omega0, 3) / params<T>::c;
            T o2 = pow(omega0, 2);
            T sinth2 = pow(sin(theta), 2);
            Eigen::Vector<T, 2> s = {cos(phi),
                                     sin(phi)};
            for (int i = 0; i < N; ++i) {
                Eigen::Vector<T, 2> ri = {xi[i],
                                          xi[i + N]};
                T ys = (ri[1] * cos(phi) - ri[0] * sin(phi)) * sin(theta);

                Eigen::Vector<T, 2> Ai = {sol[2 * i],
                                          sol[2 * i + 1]};
                Eigen::Vector<T, 2> Bi = {sol[2 * i + 2 * N],
                                          sol[2 * i + 1 + 2 * N]};
                T argument = omega0 * ys / params<T>::c;
                T Ais = Ai.dot(s);
                T Bis = Bi.dot(s);
                Eigen::Vector<T, 2> ABis = Ai * Bis;
                Eigen::Vector<T, 2> BAis = Bi * Ais;

                Eigen::Vector<T, 2> Pc1i = ((s * Ais * sinth2 - Ai) * cos(argument) -
                                            (s * Bis * sinth2 - Bi) * sin(argument));
                Eigen::Vector<T, 2> Ps1i = ((s * Ais * sinth2 - Ai) * sin(argument) +
                                            (s * Bis * sinth2 - Bi) * cos(argument));
                Eigen::Vector<T, 2> Pcomi = -(omega0 / params<T>::c) * (sin(theta)) * (ABis - BAis);

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

                    rj = {xi[j],
                          xi[j + N]};
                    ysj = (rj[1] * cos(phi) - rj[0] * sin(phi)) * sin(theta);
                    Aj = {sol[2 * j],
                          sol[2 * j + 1]};
                    Bj = {sol[2 * j + 2 * N],
                          sol[2 * j + 1 + 2 * N]};

                    argumentj = omega0 * ysj / params<T>::c;
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
                    Pcomj = -(omega0 / params<T>::c) * (sin(theta)) * (ABjs - BAjs);


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
    void Dipoles<T>::getFullFunction(const Eigen::Vector<T, Eigen::Dynamic> &xi,
                                     const Eigen::Vector<T, Eigen::Dynamic> &sol) {
        this->Ifunction_ = [&xi, &sol](T theta, T phi, T t) {
            int N = xi.size() / 2;
            T res = 0;
            T s[2] = {cos(phi), sin(phi)};
            T ress[3] = {0, 0, 0};
            for (int i = 0; i < N; ++i) {
                T ri[2] = {xi[i], xi[i + N]};
                T ys = (ri[1] * cos(phi) - ri[0] * sin(phi)) * sin(theta);
                T t0 = t - ys / params<T>::c;
                T Ai[2] = {sol.coeffRef(2 * i), sol.coeffRef(2 * i + 1)};
                T Bi[2] = {sol.coeffRef(2 * i + 2 * N), sol.coeffRef(2 * i + 1 + 2 * N)};


                T Di[2] = {Ai[0] * cos(params<T>::omega * t0) + Bi[0] * sin(params<T>::omega * t0),
                           Ai[1] * cos(params<T>::omega * t0) + Bi[1] * sin(params<T>::omega * t0)};

                T vi[2] = {params<T>::omega * (Bi[0] * cos(params<T>::omega * t0)
                                               - Ai[0] * sin(params<T>::omega * t0)) / params<T>::c,
                           params<T>::omega * (Bi[1] * cos(params<T>::omega * t0)
                                               - Ai[1] * sin(params<T>::omega)) / params<T>::c};
                T ai[2] = {-pow(params<T>::omega, 2) * Di[0], -pow(params<T>::omega, 2) * Di[1]};

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


        this->I2function_ = [&xi, &sol](T phi, T theta) {
            int N = xi.size() / 2;
            T omega0 = params<T>::omega;
            T T0 = M_PI * 2 / omega0;
            T res;
            Eigen::Vector<T, 2> resxy = {0.0, 0.0};
            T resz = 0.0;
            T o3dc = pow(omega0, 3) / params<T>::c;
            T o2 = pow(omega0, 2);
            T sinth2 = pow(sin(theta), 2);
            Eigen::Vector<T, 2> s = {cos(phi),
                                     sin(phi)};
            for (int i = 0; i < N; ++i) {
                Eigen::Vector<T, 2> ri = {xi[i],
                                          xi[i + N]};
                T ys = (ri[1] * cos(phi) - ri[0] * sin(phi)) * sin(theta);

                Eigen::Vector<T, 2> Ai = {sol.coeffRef(2 * i),
                                          sol.coeffRef(2 * i + 1)};
                Eigen::Vector<T, 2> Bi = {sol.coeffRef(2 * i + 2 * N),
                                          sol.coeffRef(2 * i + 1 + 2 * N)};
                T argument = omega0 * ys / params<T>::c;
                T Ais = Ai.dot(s);
                T Bis = Bi.dot(s);
                Eigen::Vector<T, 2> ABis = Ai * Bis;
                Eigen::Vector<T, 2> BAis = Bi * Ais;

                Eigen::Vector<T, 2> Pc1i = ((s * Ais * sinth2 - Ai) * cos(argument) -
                                            (s * Bis * sinth2 - Bi) * sin(argument));
                Eigen::Vector<T, 2> Ps1i = ((s * Ais * sinth2 - Ai) * sin(argument) +
                                            (s * Bis * sinth2 - Bi) * cos(argument));
                Eigen::Vector<T, 2> Pcomi = -(omega0 / params<T>::c) * (sin(theta)) * (ABis - BAis);

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

                    rj = {xi[j],
                          xi[j + N]};
                    ysj = (rj[1] * cos(phi) - rj[0] * sin(phi)) * sin(theta);
                    Aj = {sol.coeffRef(2 * j),
                          sol.coeffRef(2 * j + 1)};
                    Bj = {sol.coeffRef(2 * j + 2 * N),
                          sol.coeffRef(2 * j + 1 + 2 * N)};

                    argumentj = omega0 * ysj / params<T>::c;
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
                    Pcomj = -(omega0 / params<T>::c) * (sin(theta)) * (ABjs - BAjs);


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

        K2 << params<T>::omega / (params<T>::c * pow(rMode, 2)), 0,
                0, params<T>::omega / (params<T>::c * pow(rMode, 2));
    }

    template<class T>
     Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> Dipoles<T>::getMatrixx() {
        Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> matrixx;
        matrixx.resize(4 * N_, 4 * N_);
        matrixx.topLeftCorner(2 * N_, 2 * N_).noalias() = M1_;
        matrixx.topRightCorner(2 * N_, 2 * N_).noalias() = -M2_;
        matrixx.bottomLeftCorner(2 * N_, 2 * N_).noalias() = M2_;
        matrixx.bottomRightCorner(2 * N_, 2 * N_).noalias() = M1_;
        return matrixx;
    }


    template<class T>
    Eigen::Vector<T, Eigen::Dynamic> &Dipoles<T>::getRightPart2() {
        return f;
    }

    template<class T>
    Dipoles<T>::Dipoles(int N, Eigen::Vector<T, Eigen::Dynamic> &xi):N_(
            N) {
        initArrays();//todo move matrix init


        setMatrixes(xi);
    }

    template<class T>
    Dipoles<T>::Dipoles(int N, vector<T> &xi) :N_(
            N) {

        initArrays();
        Eigen::Map<Eigen::Vector<T, Eigen::Dynamic>> txx(xi.data(), xi.size());
        //todo copy (not share)
        setMatrixes(txx);

    }

}
#endif //DIPLOM_DIPOLES_H
