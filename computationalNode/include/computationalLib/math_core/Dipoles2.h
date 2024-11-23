#ifndef DIPLOM_DIPOLES2_H
#define DIPLOM_DIPOLES2_H
#include <vector>
#include <memory>

#include <iostream>
#include <iomanip>
#include <variant>

#include <eigen3/Eigen/Dense>

#include "common/my_consepts.h"
#include "common/constants.h"
#include "const.h"

using namespace Eigen;
using namespace myconceps;
namespace dipoles1
{
    using matplot::gca;
    using std::function, std::pair, std::vector, std::array;
    using const_::FloatType;

    bool isSymmetric(const Eigen::Matrix<FloatType , -1, -1> &matr);
    enum ReturnType:size_t
    {
        ArrayEigenVectors=0,
        EigenVector,
        StdVector
    };
    using Arr2EigenVec=std::array<Eigen::Vector<FloatType, Eigen::Dynamic>, 2>;
    using EigenVec=Eigen::Vector<FloatType, Eigen::Dynamic>;
    using standartVec=std::vector<FloatType>;

    using integrableFunction = std::function<FloatType(FloatType, FloatType, FloatType)> ;
    using directionGraph= std::function<FloatType(FloatType, FloatType)> ;
    using matrixType=Eigen::Matrix<FloatType, Eigen::Dynamic, Eigen::Dynamic>;



    using retTypes =std::variant< std::type_identity<Arr2EigenVec>,
            std::type_identity<EigenVec>,
            std::type_identity<standartVec>>;


    /**
    *
    */
    static const std::unordered_map <size_t , retTypes> enumToType = {
            {ArrayEigenVectors,   std::type_identity<Arr2EigenVec>{}},
            {EigenVector, std::type_identity<EigenVec>{} },
            {StdVector,   std::type_identity<standartVec>{}}
    };//todo reverse



    class Dipoless
    {
    public:
        Dipoless() = default;

        template<typename Container>
        requires HasSizeMethod<Container>
        Dipoless(int N,const Container &xi)
        {
            initArrays();
            setMatrixes(xi);
        }

        /*Dipoless(int N,const Eigen::Vector<FloatType , Eigen::Dynamic> &xi);

        Dipoless(int N,const std::vector<FloatType > &xi);*/

        template<typename Container>
        requires HasSizeMethod<Container>
        void setNewCoordinates(const Container &xi);

        void loadFromMatrix(const Eigen::Matrix<FloatType , Eigen::Dynamic, Eigen::Dynamic> &xi);

        template<typename Container,typename Container2>
        requires HasSizeMethod<Container>&&HasSizeMethod<Container2>
        void getFullFunction_(const Container &xi, const Container2 &sol);


        template<typename Container>
        Container solve();

        std::array<Eigen::Vector<FloatType, Eigen::Dynamic>, 2> solve2() {//todo  убрать
            //по своей сути мы делаем 1 действие(решаем блоучну систему, поэтому надо сей проецсс унифицировать

            // auto tt = (M1_ * M1_ + M2_ * M2_).lu();//todo посомотреть как auto влияет на наши вещи
            Eigen::PartialPivLU tt = (M1_ * M1_ + M2_ * M2_).lu();
            Eigen::Vector<FloatType, Eigen::Dynamic> solution_1;
            Eigen::Vector<FloatType, Eigen::Dynamic> solution_2;
            solution_1.resize(2 * N_);
            solution_2.resize(2 * N_);
            solution_1 = tt.solve(M1_ * f.block(0, 0, 2 * N_, 1) + M2_ * f.block(2 * N_, 0, 2 * N_, 1));
            solution_2 = tt.solve(M1_ * f.block(2 * N_, 0, 2 * N_, 1) - M2_ * f.block(0, 0, 2 * N_, 1));
            return {solution_1, solution_2};
        }


        Eigen::Vector<FloatType, Eigen::Dynamic> solve3() {
            Eigen::PartialPivLU tt = (M1_ * M1_ + M2_ * M2_).lu();
            Eigen::Vector<FloatType, Eigen::Dynamic> solution_;
            solution_.resize(4 * N_);
            solution_.block(0, 0, 2 * N_, 1) = tt.solve(
                    M1_ * f.block(0, 0, 2 * N_, 1) + M2_ * f.block(2 * N_, 0, 2 * N_, 1));
            solution_.block(2 * N_, 0, 2 * N_, 1) = tt.solve(
                    M1_ * f.block(2 * N_, 0, 2 * N_, 1) - M2_ * f.block(0, 0, 2 * N_, 1));
            return solution_;
        }

        std::vector<FloatType>  solve4() {

            std::vector<FloatType>  sol(4 * N_);
            Eigen::PartialPivLU<Eigen::Matrix<FloatType, Eigen::Dynamic, Eigen::Dynamic>> tt = (M1_ * M1_ + M2_ * M2_).lu();
            Eigen::Map<Eigen::Vector<FloatType, Eigen::Dynamic>> solution_(sol.data(), sol.size());
            solution_.resize(4 * N_);
            solution_.block(0, 0, 2 * N_, 1) = tt.solve(
                    M1_ * f.block(0, 0, 2 * N_, 1) + M2_ * f.block(2 * N_, 0, 2 * N_, 1));
            solution_.block(2 * N_, 0, 2 * N_, 1) = tt.solve(
                    M1_ * f.block(2 * N_, 0, 2 * N_, 1) - M2_ * f.block(0, 0, 2 * N_, 1));
            return sol;
        }


        const integrableFunction &   getIfunction() const {
            return Ifunction_;
        }

        const directionGraph &getI2function() const {
            return I2function_;
        }


        Eigen::Vector<FloatType, Eigen::Dynamic> &getRightPart2();

        matrixType getMatrixx();

        void printMatrix(std::ostream &out, Eigen::IOFormat &format);

        void printRightPart(std::ostream &out, Eigen::IOFormat &format);

    protected:
        template<typename Container>
        //todo bracket operator
        FloatType getDistance(int i1, int i2, Container &xi) {
            FloatType d1;
            FloatType d2;
            if constexpr (HasSizeMethod<typename Container::value_type>)
            {
                d1 = xi[0][i1] - xi[0][i2];
                d2 = xi[1][i1] - xi[1][i2];
            }
            else
            {
                d1 = xi[i1] - xi[i2];
                d2 = xi[i1 + N_] - xi[i2 + N_];
            }


            return std::sqrt(d1 * d1 + d2 * d2);
        };

        template<typename Container>
        //todo Container value type
        Eigen::Vector<FloatType , 2> getRIM(int i, int m, Container &xi) {

            FloatType d1;
            FloatType d2;
            if constexpr (HasSizeMethod<typename Container::value_type>)
            {
                d1 = xi[0][i] - xi[0][m];
                d2 = xi[1][i] - xi[1][m];
            }
            else
            {
                d1 = xi[i] - xi[m];
                d2 = xi[i + N_] - xi[m + N_];
            }
            /*FloatType d1 = xi[i] - xi[m];
            FloatType d2 = xi[i + N_] - xi[m + N_];*/
            return {d1, d2};
        };


        matrixType M1_;
        matrixType M2_;
        integrableFunction Ifunction_;
        directionGraph I2function_;

        Eigen::Vector<FloatType, Eigen::Dynamic> f;
        void initArrays();


        void
        getMatrixes(const Eigen::Vector<FloatType , 2> &rim, FloatType rMode, Eigen::Matrix<FloatType , 2, 2> &K1,
                    Eigen::Matrix<FloatType , 2, 2> &K2) const;

        template<typename Container>
        void setMatrixes(const Container &xi);

        FloatType an = params2::a;
        int N_;


    };






    template<typename Container>
    void Dipoless::setMatrixes(const Container &xi)
    {
        vector<pair<int, int>> sectors(4);
        sectors[0] = {0, 0};
        sectors[1] = {0, 2 * N_};
        sectors[2] = {2 * N_, 0};
        sectors[3] = {2 * N_, 2 * N_};
        for (int I = 0; I < N_; ++I) {//MAC
            for (int M = 0; M < N_; ++M) {
                if (I == M) {
                    Eigen::Matrix<FloatType, 2, 2> id = Eigen::Matrix<FloatType, 2, 2>::Identity() *
                                                (params2::omega0 * params2::omega0 -
                                                 params2::omega * params2::omega);
                    M1_.template block<2, 2>(2 * I, 2 * M) = id;
                } else {

                    Eigen::Vector<FloatType, 2> rim = getRIM(M, I, xi);
                    auto rMode = getDistance(I, M, xi);
                    Eigen::Matrix<FloatType, 2, 2> K1;
                    Eigen::Matrix<FloatType, 2, 2> K2;
                    getMatrixes(rim, rMode, K1, K2);
                    auto arg = params2::omega * rMode / params2::c;
                    Eigen::Matrix<FloatType, 2, 2> tmpmatr = -an * (K1 * cos(arg) - K2 * sin(arg));
                    M1_.template block<2, 2>(2 * I, 2 * M) = tmpmatr;
                }
            }
        }

        for (int I = 0; I < N_; ++I) {//MBC
            for (int M = 0; M < N_; ++M) {
                if (I == M) {
                    Eigen::Matrix<FloatType, 2, 2> id =
                            Eigen::Matrix<FloatType, 2, 2>::Identity() * (params2::yo * params2::omega);
                    M2_.template block<2, 2>(2 * I, 2 * M) = -id;
                } else {

                    Eigen::Vector<FloatType, 2> rim = getRIM(M, I, xi);
                    auto rMode = getDistance(I, M, xi);
                    Eigen::Matrix<FloatType, 2, 2> K1;
                    Eigen::Matrix<FloatType, 2, 2> K2;
                    getMatrixes(rim, rMode, K1, K2);
                    auto arg = params2::omega * rMode / params2::c;

                    Eigen::Matrix<FloatType, 2, 2> tmpmatr = -an * (K2 * cos(arg) + K1 * sin(arg));

                    M2_.template block<2, 2>(2 * I, 2 * M) = tmpmatr;
                }

            }
        }
    }

    template<typename Container>
    requires HasSizeMethod<Container>
    void Dipoless::setNewCoordinates(const Container &xi)
    {
        if (an == params2::a) {
            this->N_ = xi.size() / 2;
            initArrays();
        }
        setMatrixes(xi);
    }


    template<typename Container,typename Container2>
    requires HasSizeMethod<Container>&&HasSizeMethod<Container2>
    void Dipoless::getFullFunction_(const Container &xi, const Container2 &sol)
    {


        this->Ifunction_ = [&xi, &sol](FloatType theta, FloatType phi, FloatType t) {
            int N = xi.size() / 2;
            FloatType res = 0;
            FloatType s[2] = {cos(phi), sin(phi)};
            FloatType ress[3] = {0, 0, 0};
            for (int i = 0; i < N; ++i) {
                FloatType ri[2] = {getElement(xi,0, i,N), getElement(xi,1, i,N)};//todo what are those mgick numbers for x and y
                FloatType ys = (ri[1] * cos(phi) - ri[0] * sin(phi)) * sin(theta);
                FloatType t0 = t - ys / params2::c;
                FloatType Ai[2] = {get_value(sol, 2 * i), get_value(sol, 2 * i + 1)};
                FloatType Bi[2] = {get_value(sol, 2 * i + 2 * N), get_value(sol, 2 * i + 1 + 2 * N)};


                FloatType Di[2] = {Ai[0] * cos(params2::omega * t0) + Bi[0] * sin(params2::omega * t0),
                           Ai[1] * cos(params2::omega * t0) + Bi[1] * sin(params2::omega * t0)};

                FloatType vi[2] = {params2::omega * (Bi[0] * cos(params2::omega * t0)
                                               - Ai[0] * sin(params2::omega * t0)) / params2::c,
                           params2::omega * (Bi[1] * cos(params2::omega * t0)
                                               - Ai[1] * sin(params2::omega)) / params2::c};
                FloatType ai[2] = {-pow(params2::omega, 2) * Di[0], -pow(params2::omega, 2) * Di[1]};

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
            FloatType omega0 = params2::omega;
            FloatType T0 = M_PI * 2 / omega0;
            FloatType res;
            Eigen::Vector<FloatType , 2> resxy = {0.0, 0.0};
            FloatType resz = 0.0;
            FloatType o3dc = pow(omega0, 3) / params2::c;
            FloatType o2 = pow(omega0, 2);
            FloatType sinth2 = pow(sin(theta), 2);
            Eigen::Vector<FloatType , 2> s = {cos(phi),
                                     sin(phi)};
            for (int i = 0; i < N; ++i) {
                FloatType ri[2] = {getElement(xi,0, i,N), getElement(xi,1, i,N)};
                FloatType ys = (ri[1] * cos(phi) - ri[0] * sin(phi)) * sin(theta);
                Eigen::Vector<FloatType , 2> Ai = {get_value(sol, 2 * i), get_value(sol, 2 * i + 1)};
                Eigen::Vector<FloatType , 2> Bi = {get_value(sol, 2 * i + 2 * N), get_value(sol, 2 * i + 1 + 2 * N)};

                FloatType argument = omega0 * ys / params2::c;
                FloatType Ais = Ai.dot(s);
                FloatType Bis = Bi.dot(s);
                Eigen::Vector<FloatType , 2> ABis = Ai * Bis;
                Eigen::Vector<FloatType , 2> BAis = Bi * Ais;

                Eigen::Vector<FloatType , 2> Pc1i = ((s * Ais * sinth2 - Ai) * cos(argument) -
                                            (s * Bis * sinth2 - Bi) * sin(argument));
                Eigen::Vector<FloatType , 2> Ps1i = ((s * Ais * sinth2 - Ai) * sin(argument) +
                                            (s * Bis * sinth2 - Bi) * cos(argument));
                Eigen::Vector<FloatType , 2> Pcomi = -(omega0 / params2::c) * (sin(theta)) * (ABis - BAis);

                FloatType Pci = (Ais * cos(argument) - Bis * sin(argument));
                FloatType Psi = (Ais * sin(argument) + Bis * cos(argument));

                Eigen::Vector<FloatType , 2> rj;
                FloatType ysj;
                Eigen::Vector<FloatType , 2> Aj;
                Eigen::Vector<FloatType , 2> Bj;
                FloatType argumentj;

                FloatType Ajs;
                FloatType Bjs;


                FloatType Pcj;
                FloatType Psj;


                Eigen::Vector<FloatType , 2> Pc1j;
                Eigen::Vector<FloatType , 2> Ps1j;
                Eigen::Vector<FloatType , 2> Pcomj;


                Eigen::Vector<FloatType , 2> ABjs;
                Eigen::Vector<FloatType , 2> BAjs;
                for (int j = 0; j < i; ++j) {
                    rj = {getElement(xi,0, j,N), getElement(xi,1, j,N)};
                    ysj = (rj[1] * cos(phi) - rj[0] * sin(phi)) * sin(theta);

                    Aj = {get_value(sol, 2 * j), get_value(sol, 2 * j + 1)};
                    Bj = {get_value(sol, 2 * j + 2 * N), get_value(sol, j + 1 + 2 * N)};

                    argumentj = omega0 * ysj / params2::c;
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
                    Pcomj = -(omega0 / params2::c) * (sin(theta)) * (ABjs - BAjs);


                    Eigen::Vector<FloatType , 2> rij_xy =
                            Pc1i.cwiseProduct(Pc1j) + Ps1i.cwiseProduct(Ps1j)
                            + 2 * Pcomi.cwiseProduct(Pcomj);

                    FloatType rij_z = (Pci * Pcj + Psi * Psj);
                    resz += rij_z;
                    resxy += rij_xy;
                }
                Eigen::Vector<FloatType , 2> ri_xy =
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
        //todo serialize part of this(sve result into table)

    }


}

#endif //DIPLOM_DIPOLES2_H
