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

    template<class T>
    class Dipoles {
    public:
        Dipoles(int N, std::vector<Eigen::Vector<T,2>> &xi);


        void solve_(){
            Eigen::Matrix<T,Eigen::Dynamic,Eigen::Dynamic> tt=(M1_*M1_+M2_*M2_).inverse();
            solution_[0]=tt*(M1_*f1+M2_*f2);
            solution_[1]=tt*(M1_*f2-M2_*f1);
            return ;
        };

    private:
        T getDistance(int i1, int i2){

            T d1=xi_[i1].coeffRef(0)-xi_[i2].coeffRef(0);
            T d2=xi_[i1].coeffRef(1)-xi_[i2].coeffRef(1);
            return  std::sqrt(d1*d1+d2*d2);
        };
        Eigen::Vector<T,2> getRIM(int i, int m) {
            T d1 = xi_[i].coeffRef(0) - xi_[m].coeffRef(0);
            T d2 = xi_[i].coeffRef(1) - xi_[m].coeffRef(1);
            return {d1, d2};
        };

        Eigen::Matrix<T,Eigen::Dynamic,Eigen::Dynamic> matrixx;
        Eigen::Matrix<T,Eigen::Dynamic,Eigen::Dynamic> M1_;
        Eigen::Matrix<T,Eigen::Dynamic,Eigen::Dynamic> M2_;
        //Eigen::Vector<T,Eigen::Dynamic> rightPart;
        Eigen::Vector<T,Eigen::Dynamic> f1;
        Eigen::Vector<T,Eigen::Dynamic> f2;
        //Eigen::Vector<T,Eigen::Dynamic> solution;
        std::array<Eigen::Vector<T,Eigen::Dynamic>,2> solution_;
    public:

        const std::array<Eigen::Vector<T,Eigen::Dynamic>,2> &getSolution_() const {
            return solution_;
        }

    public:
        const std::array<Eigen::Vector<T, Eigen::Dynamic>, 2> getRightPart();

    public:
        const Eigen::Matrix<T,Eigen::Dynamic,Eigen::Dynamic> &getMatrixx();

    private:


        std::vector<Eigen::Vector<T,2>> xi_;
        int N_;
        T c = 3.0 * pow(10, 8);
        T yo = pow(10, 7);
        T omega = pow(10, 15);
        T omega0 = omega;
        T a = 1;
        T eps = 1;
        T an=a;

        void
        getMatrixes(const Eigen::Vector<T,2> &rim, T rMode, Eigen::Matrix<T,2,2> &K1, Eigen::Matrix<T,2,2> &K2) const;
    };

    template<class T>
    void Dipoles<T>::getMatrixes(const Eigen::Vector<T, 2> &rim, T rMode, Eigen::Matrix<T, 2, 2> &K1,
                                 Eigen::Matrix<T, 2, 2> &K2) const {
            K1<<3 * rim(0) * rim(0) / pow(rMode, 5) - 1 / pow(rMode, 3),3 * rim(0) * rim(1)/ pow(rMode, 5),
                    3 * rim(0) * rim(1)/ pow(rMode, 5), 3 * rim(1) * rim(1) / pow(rMode, 5) - 1 / pow(rMode, 3);

            K2<<omega / (c * pow(rMode, 2)),0,
                    0,omega / (c * pow(rMode, 2));
    }

    template<class T>
    const Eigen::Matrix<T,Eigen::Dynamic,Eigen::Dynamic> &Dipoles<T>::getMatrixx() {

        matrixx.resize(4*N_,4*N_);
        matrixx.topLeftCorner(2*N_,2*N_).noalias()=M1_;
        matrixx.topRightCorner(2*N_,2*N_).noalias()=-M2_;
        matrixx.bottomLeftCorner(2*N_,2*N_).noalias()=M2_;
        matrixx.bottomRightCorner(2*N_,2*N_).noalias()=M1_;
        //std::cout<<MM<<"\n\n";

        return matrixx;
    }

    template<class T>
    const std::array<Eigen::Vector<T, Eigen::Dynamic>, 2> Dipoles<T>::getRightPart(){

        return {f1,f2};
    }

    template<class T>
    Dipoles<T>::Dipoles(int N, vector<Eigen::Vector<T, 2>> &xi):N_(N) {
            xi_=xi;
            an=a/N_;
            //double rdist= getDistance()
            matrixx.resize(4 * N_, 4 * N_);
            M1_.resize(2 * N_, 2 * N_);
            M2_.resize(2 * N_, 2 * N_);
            //matrixx=Eigen::Matrix<T,4 * N_, 4 * N_>;

            std::vector<std::pair<int,int>> sectors(4);
            sectors[0]={0,0};
            sectors[1]={0,2*N_};
            sectors[2]={2*N_,0};
            sectors[3]={2*N_,2*N_};
            for (int I = 0; I < N_; ++I) {//MAC
                for (int M = 0; M < N_; ++M) {
                    if(I==M)
                    {
                        auto id=Eigen::Matrix<T,2,2>::Identity()*(omega0*omega0-omega*omega);
                        matrixx.block(2 * I, 2 * M, 2, 2) = id;
                        matrixx.block(2 * I+sectors[3].first, 2 * M+sectors[3].second, 2, 2) = id;
                        M1_.block(2 * I, 2 * M, 2, 2)=id;
                    }
                    else {

                        Eigen::Vector<T,2> rim = getRIM(M, I);
                        T rMode = getDistance(I, M);
                        Eigen::Matrix<T,2,2> K1;
                        Eigen::Matrix<T,2,2> K2;
                        getMatrixes(rim, rMode, K1, K2);
                        T arg = omega * rMode / c;
                        auto tmpmatr=-an * (K1 * cos(arg) - K2 * sin(arg));
                        matrixx.block(2 * I, 2 * M, 2, 2) = tmpmatr;
                        matrixx.block(2 * I+sectors[3].first, 2 * M+sectors[3].second, 2, 2) = tmpmatr;
                        M1_.block(2 * I, 2 * M, 2, 2)=tmpmatr;
                    }

                }
            }
            //std::cout<<matrixx<<"\n\n\n";


            for (int I = 0; I < N_; ++I) {//MBC
                for (int M = 0; M < N_; ++M) {
                    if(I==M)
                    {
                        auto id=Eigen::Matrix<T,2,2>::Identity()*(yo*omega);
                        matrixx.block(2 * I+sectors[1].first, 2 * M+sectors[1].second, 2, 2)=id ;
                        matrixx.block(2 * I+sectors[2].first, 2 * M+sectors[2].second, 2, 2)=-id ;
                        //matrixx.block<2, 2>(2 * I+sectors[1].first, 2 * M+sectors[1].second) = Eigen::Matrix<T,2,2>::Identity()*(yo*omega);
                        M2_.block(2 * I, 2 * M, 2, 2)=-id;
                    }
                    else {

                        Eigen::Vector<T,2> rim = getRIM(M, I);
                        T rMode = getDistance(I, M);
                        Eigen::Matrix<T,2,2> K1;
                        Eigen::Matrix<T,2,2> K2;
                        getMatrixes(rim, rMode, K1, K2);
                        T arg = omega * rMode / c;
                        auto tmpmatr=-an * (K2 * cos(arg) +K1 * sin(arg));
                        //auto tmpmatr1=an * (K2 * cos(arg) +K1 * sin(arg));
                        matrixx.block(2 * I+sectors[1].first, 2 * M+sectors[1].second, 2, 2)  = -tmpmatr;
                        matrixx.block(2 * I+sectors[2].first, 2 * M+sectors[2].second, 2, 2)  = tmpmatr;

                        M2_.block(2 * I, 2 * M, 2, 2)=tmpmatr;
                    }

                }
            }


            f1.resize(2*N_);
            f2.resize(2*N_);
            for (int i = 0; i < N_; ++i) {
                f1(2*i)=an*eps;
                f1(2*i+1)=0;
                f2(2*i)=0;
                f2(2*i+1)=an*eps;
            }
    }

}
#endif //DIPLOM_DIPOLES_H
