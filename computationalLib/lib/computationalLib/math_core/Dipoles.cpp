#include "computationalLib/math_core/Dipoles.h"

namespace dipoles {

void
Dipoles::GetMatrices(const Eigen::Vector<FloatType, 2> &rim, FloatType r_mode, Eigen::Matrix<FloatType, 2, 2> &k_1,
                     Eigen::Matrix<FloatType, 2, 2> &k_2) const {
  k_1 << 3 * rim(0) * rim(0) / pow(r_mode, 5) - 1 / pow(r_mode, 3), 3 * rim(0) * rim(1) / pow(r_mode, 5),
      3 * rim(0) * rim(1) / pow(r_mode, 5), 3 * rim(1) * rim(1) / pow(r_mode, 5) - 1 / pow(r_mode, 3);

  k_2 << params::omega / (params::c * pow(r_mode, 2)), 0,
      0, params::omega / (params::c * pow(r_mode, 2));
}

void Dipoles::InitArrays() {
  an_ = params::a / n_;
  m_1_.resize(2 * n_, 2 * n_);
  m_2_.resize(2 * n_, 2 * n_);

  f_.resize(4 * n_);
  for (int i = 0; i < n_; ++i) {
    f_(2 * i) = an_ * params::eps;
    f_(2 * i + 1) = 0;
    f_(2 * i + 2 * n_) = 0;
    f_(2 * i + 1 + 2 * n_) = an_ * params::eps;
  }
}

void Dipoles::LoadFromMatrix(const MatrixType &xi) {
  this->n_ = xi.rows() / 4;
  InitArrays();

  m_1_ = xi.topLeftCorner(2 * n_, 2 * n_);
  m_2_ = xi.bottomLeftCorner(2 * n_, 2 * n_);
}

const EigenVec &Dipoles::GetRightPart() {
  return f_;
}

MatrixType Dipoles::GetMatrixx() {
  MatrixType matrixx;
  matrixx.resize(4 * n_, 4 * n_);
  matrixx.topLeftCorner(2 * n_, 2 * n_).noalias() = m_1_;
  matrixx.topRightCorner(2 * n_, 2 * n_).noalias() = -m_2_;
  matrixx.bottomLeftCorner(2 * n_, 2 * n_).noalias() = m_2_;
  matrixx.bottomRightCorner(2 * n_, 2 * n_).noalias() = m_1_;
  return matrixx;
}

template<>
Arr2EigenVec Dipoles::Solve() {
  Eigen::PartialPivLU tt = (m_1_ * m_1_ + m_2_ * m_2_).lu();
  Eigen::Vector<FloatType, Eigen::Dynamic> solution_1;
  Eigen::Vector<FloatType, Eigen::Dynamic> solution_2;
  solution_1.resize(2 * n_);
  solution_2.resize(2 * n_);
  solution_1 = tt.solve(m_1_ * f_.block(0, 0, 2 * n_, 1) + m_2_ * f_.block(2 * n_, 0, 2 * n_, 1));
  solution_2 = tt.solve(m_1_ * f_.block(2 * n_, 0, 2 * n_, 1) - m_2_ * f_.block(0, 0, 2 * n_, 1));
  return {solution_1, solution_2};
}

template<>
EigenVec Dipoles::Solve() {
  Eigen::PartialPivLU tt = (m_1_ * m_1_ + m_2_ * m_2_).lu();
  Eigen::Vector<FloatType, Eigen::Dynamic> solution;
  solution.resize(4 * n_);
  solution.block(0, 0, 2 * n_, 1) = tt.solve(
      m_1_ * f_.block(0, 0, 2 * n_, 1) + m_2_ * f_.block(2 * n_, 0, 2 * n_, 1));
  solution.block(2 * n_, 0, 2 * n_, 1) = tt.solve(
      m_1_ * f_.block(2 * n_, 0, 2 * n_, 1) - m_2_ * f_.block(0, 0, 2 * n_, 1));
  return solution;
}

template<>
StdVec Dipoles::Solve() {
  std::vector<FloatType> sol(4 * n_);
  Eigen::PartialPivLU<Eigen::Matrix<FloatType, Eigen::Dynamic, Eigen::Dynamic>> tt = (m_1_ * m_1_ + m_2_ * m_2_).lu();
  Eigen::Map<Eigen::Vector<FloatType, Eigen::Dynamic>> solution(sol.data(), sol.size());
  solution.resize(4 * n_);
  solution.block(0, 0, 2 * n_, 1) = tt.solve(
      m_1_ * f_.block(0, 0, 2 * n_, 1) + m_2_ * f_.block(2 * n_, 0, 2 * n_, 1));
  solution.block(2 * n_, 0, 2 * n_, 1) = tt.solve(
      m_1_ * f_.block(2 * n_, 0, 2 * n_, 1) - m_2_ * f_.block(0, 0, 2 * n_, 1));
  return sol;
}

template<>
StdValarr Dipoles::Solve() {
  StdValarr sol(4 * n_);
  Eigen::PartialPivLU<Eigen::Matrix<FloatType, Eigen::Dynamic, Eigen::Dynamic>> tt = (m_1_ * m_1_ + m_2_ * m_2_).lu();
  Eigen::Map<Eigen::Vector<FloatType, Eigen::Dynamic>> solution(&(sol[0]), sol.size());
  solution.resize(4 * n_);
  solution.block(0, 0, 2 * n_, 1) = tt.solve(
      m_1_ * f_.block(0, 0, 2 * n_, 1) + m_2_ * f_.block(2 * n_, 0, 2 * n_, 1));
  solution.block(2 * n_, 0, 2 * n_, 1) = tt.solve(
      m_1_ * f_.block(2 * n_, 0, 2 * n_, 1) - m_2_ * f_.block(0, 0, 2 * n_, 1));
  return sol;
}

}