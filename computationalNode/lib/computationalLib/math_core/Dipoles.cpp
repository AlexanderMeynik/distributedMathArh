#include "computationalLib/math_core/Dipoles.h"

namespace dipoles {

    void
    Dipoles::getMatrices(const Eigen::Vector<FloatType, 2> &rim, FloatType rMode, Eigen::Matrix<FloatType, 2, 2> &K1,
                         Eigen::Matrix<FloatType, 2, 2> &K2) const {
        K1 << 3 * rim(0) * rim(0) / pow(rMode, 5) - 1 / pow(rMode, 3), 3 * rim(0) * rim(1) / pow(rMode, 5),
                3 * rim(0) * rim(1) / pow(rMode, 5), 3 * rim(1) * rim(1) / pow(rMode, 5) - 1 / pow(rMode, 3);

        K2 << params::omega / (params::c * pow(rMode, 2)), 0,
                0, params::omega / (params::c * pow(rMode, 2));
    }

    void Dipoles::initArrays() {
        an = params::a / N_;
        M1_.resize(2 * N_, 2 * N_);
        M2_.resize(2 * N_, 2 * N_);


        f.resize(4 * N_);
        for (int i = 0; i < N_; ++i) {
            f(2 * i) = an * params::eps;
            f(2 * i + 1) = 0;
            f(2 * i + 2 * N_) = 0;
            f(2 * i + 1 + 2 * N_) = an * params::eps;
        }
    }

    void Dipoles::loadFromMatrix(const Matrix<FloatType, Eigen::Dynamic, Eigen::Dynamic> &xi) {
        this->N_ = xi.rows() / 4;
        initArrays();

        M1_ = xi.topLeftCorner(2 * N_, 2 * N_);
        M2_ = xi.bottomLeftCorner(2 * N_, 2 * N_);
    }

    const Eigen::Vector<FloatType, Eigen::Dynamic> &Dipoles::getRightPart() {
        return f;
    }

    co::matrixType Dipoles::getMatrixx() {
        co::matrixType matrixx;
        matrixx.resize(4 * N_, 4 * N_);
        matrixx.topLeftCorner(2 * N_, 2 * N_).noalias() = M1_;
        matrixx.topRightCorner(2 * N_, 2 * N_).noalias() = -M2_;
        matrixx.bottomLeftCorner(2 * N_, 2 * N_).noalias() = M2_;
        matrixx.bottomRightCorner(2 * N_, 2 * N_).noalias() = M1_;
        return matrixx;
    }

    void Dipoles::printMatrix(std::ostream &out, IOFormat &format) {
        out << "Матрица\n" << getMatrixx().format(format) << "\n\n";
    }

    void Dipoles::printRightPart(std::ostream &out, IOFormat &format) {
        out << "Правая часть\n" << this->f.format(format)
            << "\n\n";
    }


    template<>
    co::Arr2EigenVec Dipoles::solve() {
        Eigen::PartialPivLU tt = (M1_ * M1_ + M2_ * M2_).lu();
        Eigen::Vector<FloatType, Eigen::Dynamic> solution_1;
        Eigen::Vector<FloatType, Eigen::Dynamic> solution_2;
        solution_1.resize(2 * N_);
        solution_2.resize(2 * N_);
        solution_1 = tt.solve(M1_ * f.block(0, 0, 2 * N_, 1) + M2_ * f.block(2 * N_, 0, 2 * N_, 1));
        solution_2 = tt.solve(M1_ * f.block(2 * N_, 0, 2 * N_, 1) - M2_ * f.block(0, 0, 2 * N_, 1));
        return {solution_1, solution_2};
    }

    template<>
    co::EigenVec Dipoles::solve() {
        Eigen::PartialPivLU tt = (M1_ * M1_ + M2_ * M2_).lu();
        Eigen::Vector<FloatType, Eigen::Dynamic> solution_;
        solution_.resize(4 * N_);
        solution_.block(0, 0, 2 * N_, 1) = tt.solve(
                M1_ * f.block(0, 0, 2 * N_, 1) + M2_ * f.block(2 * N_, 0, 2 * N_, 1));
        solution_.block(2 * N_, 0, 2 * N_, 1) = tt.solve(
                M1_ * f.block(2 * N_, 0, 2 * N_, 1) - M2_ * f.block(0, 0, 2 * N_, 1));
        return solution_;
    }

    template<>
    co::standartVec Dipoles::solve() {
        std::vector<FloatType> sol(4 * N_);
        Eigen::PartialPivLU<Eigen::Matrix<FloatType, Eigen::Dynamic, Eigen::Dynamic>> tt = (M1_ * M1_ + M2_ * M2_).lu();
        Eigen::Map<Eigen::Vector<FloatType, Eigen::Dynamic>> solution_(sol.data(), sol.size());
        solution_.resize(4 * N_);
        solution_.block(0, 0, 2 * N_, 1) = tt.solve(
                M1_ * f.block(0, 0, 2 * N_, 1) + M2_ * f.block(2 * N_, 0, 2 * N_, 1));
        solution_.block(2 * N_, 0, 2 * N_, 1) = tt.solve(
                M1_ * f.block(2 * N_, 0, 2 * N_, 1) - M2_ * f.block(0, 0, 2 * N_, 1));
        return sol;
    }

    template<>
    co::standartValarr Dipoles::solve() {
        co::standartValarr sol(4 * N_);
        Eigen::PartialPivLU<Eigen::Matrix<FloatType, Eigen::Dynamic, Eigen::Dynamic>> tt = (M1_ * M1_ + M2_ * M2_).lu();
        Eigen::Map<Eigen::Vector<FloatType, Eigen::Dynamic>> solution_(&(sol[0]), sol.size());
        solution_.resize(4 * N_);
        solution_.block(0, 0, 2 * N_, 1) = tt.solve(
                M1_ * f.block(0, 0, 2 * N_, 1) + M2_ * f.block(2 * N_, 0, 2 * N_, 1));
        solution_.block(2 * N_, 0, 2 * N_, 1) = tt.solve(
                M1_ * f.block(2 * N_, 0, 2 * N_, 1) - M2_ * f.block(0, 0, 2 * N_, 1));
        return sol;
    }


}