#include "computationalLib/math_core/Dipoles2.h"
namespace dipoles1
{

    bool isSymmetric(const Matrix<FloatType, -1, -1> &matr) {
        size_t N = matr.rows();

        for (int i = 0; i < N; ++i) {

            for (int j = 0; j < i; ++j) {
                if (matr.coeffRef(i, j) != matr.coeffRef(j, i)) {
                    return false;
                }
            }

        }
        return true;
    }

   /* Dipoless::Dipoless(int N, const Vector<FloatType, -1> &xi) {
        initArrays();
        setMatrixes(xi);
    }

    Dipoless::Dipoless(int N, const vector<FloatType> &xi) {
        initArrays();
        //todo if we hve const qualifier than interfacing wont work
       *//* const Eigen::Map<Eigen::Vector<FloatType, Eigen::Dynamic>> txx(xi.data(), xi.size());*//*
        setMatrixes(xi);
    }
*/

   void
   Dipoless::getMatrixes(const Eigen::Vector<FloatType , 2> &rim, FloatType rMode, Eigen::Matrix<FloatType , 2, 2> &K1,
                         Eigen::Matrix<FloatType , 2, 2> &K2) const
   {
       K1 << 3 * rim(0) * rim(0) / pow(rMode, 5) - 1 / pow(rMode, 3), 3 * rim(0) * rim(1) / pow(rMode, 5),
               3 * rim(0) * rim(1) / pow(rMode, 5), 3 * rim(1) * rim(1) / pow(rMode, 5) - 1 / pow(rMode, 3);

       K2 << params2::omega / (params2::c * pow(rMode, 2)), 0,
               0, params2::omega / (params2::c * pow(rMode, 2));
   }
    void Dipoless::initArrays() {
        an = params2::a / N_;
        M1_.resize(2 * N_, 2 * N_);
        M2_.resize(2 * N_, 2 * N_);


        f.resize(4 * N_);
        for (int i = 0; i < N_; ++i) {
            f(2 * i) = an * params2::eps;
            f(2 * i + 1) = 0;
            f(2 * i + 2 * N_) = 0;
            f(2 * i + 1 + 2 * N_) = an * params2::eps;
        }
    }

    void Dipoless::loadFromMatrix(const Matrix<FloatType, Eigen::Dynamic, Eigen::Dynamic> &xi) {
        this->N_ = xi.rows() / 4;
        initArrays();

        M1_ = xi.topLeftCorner(2 * N_, 2 * N_);
        M2_ = xi.bottomLeftCorner(2 * N_, 2 * N_);
    }

    Eigen::Vector<FloatType, Eigen::Dynamic> &Dipoless::getRightPart2() {
        return f;
    }

    Eigen::Matrix<FloatType, Eigen::Dynamic, Eigen::Dynamic> Dipoless::getMatrixx() {
        Eigen::Matrix<FloatType, Eigen::Dynamic, Eigen::Dynamic> matrixx;
        matrixx.resize(4 * N_, 4 * N_);
        matrixx.topLeftCorner(2 * N_, 2 * N_).noalias() = M1_;
        matrixx.topRightCorner(2 * N_, 2 * N_).noalias() = -M2_;
        matrixx.bottomLeftCorner(2 * N_, 2 * N_).noalias() = M2_;
        matrixx.bottomRightCorner(2 * N_, 2 * N_).noalias() = M1_;
        return matrixx;
    }

    void Dipoless::printMatrix(std::ostream &out, IOFormat &format) {
        out << "Матрица\n" << getMatrixx().format(format) << "\n\n";
    }

    void Dipoless::printRightPart(std::ostream &out, IOFormat &format) {
        out << "Правая часть\n" << this->f.format(format)
            << "\n\n";
    }


}