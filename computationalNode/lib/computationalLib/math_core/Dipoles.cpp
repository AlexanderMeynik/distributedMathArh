//
//
//#include <cmath>
//#include "../../../include/computationalLib/math_core/Dipoles.h"
///*
//
//dipoles::Dipoles::Dipoles(int N, std::vector<Eigen::Vector<long double,2>>&xi):N_(N) {
//    xi_=xi;
//    an=a/N_;
//    //double rdist= getDistance()
//    matrixx.resize(4 * N_, 4 * N_);
//    //matrixx=Eigen::Matrix<long double,4 * N_, 4 * N_>;
//
//    std::vector<std::pair<int,int>> sectors(4);
//    sectors[0]={0,0};
//    sectors[1]={0,2*N_};
//    sectors[2]={2*N_,0};
//    sectors[3]={2*N_,2*N_};
//    for (int I = 0; I < N_; ++I) {//MAC
//        for (int M = 0; M < N_; ++M) {
//            if(I==M)
//            {
//                matrixx.block<2, 2>(2 * I, 2 * M) = Eigen::Matrix<long double,2,2>::Identity()*(omega0*omega0-omega*omega);
//            }
//            else {
//
//                Eigen::Vector<long double,2> rim = getRIM(I, M);
//                double rMode = getDistance(I, M);
//                Eigen::Matrix<long double,2,2> K1;
//                Eigen::Matrix<long double,2,2> K2;
//                getMatrixes(rim, rMode, K1, K2);
//                double arg = omega * rMode / c;
//                matrixx.block<2, 2>(2 * I, 2 * M) = -an * (K1 * cos(arg) - K2 * sin(arg));
//            }
//
//        }
//    }
//    //std::cout<<matrixx<<"\n\n\n";
//
//
//    for (int I = 0; I < N_; ++I) {//MBC
//        for (int M = 0; M < N_; ++M) {
//            if(I==M)
//            {
//                matrixx.block<2, 2>(2 * I+sectors[1].first, 2 * M+sectors[1].second) = Eigen::Matrix<long double,2,2>::Identity()*(yo*omega);
//            }
//            else {
//
//                Eigen::Vector<long double,2> rim = getRIM(I, M);
//                double rMode = getDistance(I, M);
//                Eigen::Matrix<long double,2,2> K1;
//                Eigen::Matrix<long double,2,2> K2;
//                getMatrixes(rim, rMode, K1, K2);
//                double arg = omega * rMode / c;
//                matrixx.block<2, 2>(2 * I+sectors[1].first, 2 * M+sectors[1].second)  = -an * (-K2 * cos(arg) - K1 * sin(arg));
//            }
//
//        }
//    }
//
//    //std::cout<<matrixx<<"\n\n\n";
//
//
//    for (int I = 0; I < N_; ++I) {//MAS
//        for (int M = 0; M < N_; ++M) {
//            if(I==M)
//            {
//                matrixx.block<2, 2>(2 * I+sectors[2].first, 2 * M+sectors[2].second) = Eigen::Matrix<long double,2,2>::Identity()*(-yo*omega);
//            }
//            else {
//
//                Eigen::Vector<long double,2> rim = getRIM(I, M);
//                double rMode = getDistance(I, M);
//                Eigen::Matrix<long double,2,2> K1;
//                Eigen::Matrix<long double,2,2> K2;
//                getMatrixes(rim, rMode, K1, K2);
//                double arg = omega * rMode / c;
//                matrixx.block<2, 2>(2 * I+sectors[2].first, 2 * M+sectors[2].second) = -an * (-K1 * sin(arg) + K2 * cos(arg));
//            }
//
//        }
//    }
//    //std::cout<<matrixx<<"\n\n\n";
//
//
//    for (int I = 0; I < N_; ++I) {//MBs
//        for (int M = 0; M < N_; ++M) {
//            if(I==M)
//            {
//                matrixx.block<2, 2>(2 * I+sectors[3].first, 2 * M+sectors[3].second) = Eigen::Matrix<long double,2,2>::Identity()*(omega0*omega0-omega*omega);
//            }
//            else {
//
//                Eigen::Vector<long double,2> rim = getRIM(I, M);
//                double rMode = getDistance(I, M);
//                Eigen::Matrix<long double,2,2> K1;
//                Eigen::Matrix<long double,2,2> K2;
//                getMatrixes(rim, rMode, K1, K2);
//                double arg = omega * rMode / c;
//                matrixx.block<2, 2>(2 * I+sectors[3].first, 2 * M+sectors[3].second) = -an * (-K2 * sin(arg) + K1 * cos(arg));
//            }
//
//        }
//    }
//
//
//
//
//
//    rightPart.resize(4*N_);
//
//    for (int i = 0; i < N_; ++i) {
//
//        rightPart(2*i)=a*eps;
//        rightPart(2*i+1)=0;
//
//        rightPart(2*N_+2*i)=0;
//        rightPart(2*N_+2*i+1)=a*eps;
//    }
//
//}
//
//void
//dipoles::Dipoles::getMatrixes(const Eigen::Vector<long double,2> &rim, double rMode, Eigen::Matrix<long double,2,2> &K1, Eigen::Matrix<long double,2,2> &K2) const {
//    K1<<3 * rim(0) * rim(0) / pow(rMode, 5) - 1 / pow(rMode, 3),3 * rim(0) * rim(1),
//            3 * rim(0) * rim(1), 3 * rim(1) * rim(1) / pow(rMode, 5) - 1 / pow(rMode, 3);
//
//    K2<<omega / (c * pow(rMode, 2)),0,
//            0,omega / (c * pow(rMode, 2));
//}
//
//
//long double dipoles::Dipoles::getDistance(int i1,int i2) {
//
//    long double d1=xi_[i1].coeffRef(0)-xi_[i2].coeffRef(0);
//    long double d2=xi_[i1].coeffRef(1)-xi_[i2].coeffRef(1);
//    return  std::sqrt(d1*d1+d2*d2);
//}
//Eigen::Vector<long double,2> dipoles::Dipoles::getRIM(int i, int m) {
//    long double d1=xi_[i].coeffRef(0)-xi_[m].coeffRef(0);
//    long double d2=xi_[i].coeffRef(1)-xi_[m].coeffRef(1);
//    return  {d1,d2};
//
//}
//
//
//
//
//
//Eigen::Vector<long double,Eigen::Dynamic> dipoles::Dipoles::solve()
//{
//    return matrixx.colPivHouseholderQr().solve(rightPart);
//}
//
//
//const Eigen::Matrix<long double,Eigen::Dynamic,Eigen::Dynamic> &dipoles::Dipoles::getMatrixx() const {
//    return matrixx;
//}
//
//const Eigen::Vector<long double,Eigen::Dynamic> &dipoles::Dipoles::getRightPart() const {
//    return rightPart;
//}
//*/
//
//
//
//
//
//
//
