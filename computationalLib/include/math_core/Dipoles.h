#pragma once

#include "common/myConcepts.h"
#include "dipolesCommon.h"

///dipoles namespace
namespace dipoles {
using namespace Eigen;
using namespace my_concepts;
using shared::FloatType, shared::params;

/**
 * @brief Provides interface to Solve system of dipoles
 * @details Allows to construct and Solve system of equations for the selected mathematical model.
 * @details Can accept various input formats for coordinates and solutions to crete system,
 * @details This class implements more verbose solution algorithm. It uses quasi symmetricity of block matrix
 * @details to cut computation i half.
 * @details to Solve it, to Generate direction graph for magnetic field.
 */
class Dipoles {
 public:
  Dipoles() = default;

  /**
   * @brief Basic constructor that accepts dipole coordinates Container
   * @tparam Container
   * @param xi
   */
  template<typename Container>
  requires HasSizeMethod<Container>
  Dipoles(const Container &xi);

  /**
   * @brief Function that allows to Reset dipole coordinates
   * @details This function is used to avoid memory reallocation if dipole object already have been initialized.
   * @tparam Container
   * @param xi
   */
  template<typename Container>
  requires HasSizeMethod<Container>
  void SetNewCoordinates(const Container &xi);

  /**
   * @brief Loads system of equations matrix
   * @param xi
   */
  void LoadFromMatrix(const MatrixType &xi);

  /**
   * Calculates directional graph and/or other it variants
   * @tparam Container
   * @tparam Container2
   * @param xi
   * @param sol
   */
  template<typename Container, typename Container2>
  requires HasSizeMethod<Container> && HasSizeMethod<Container2>
  void GetFullFunction(const Container &xi, const Container2 &sol);

  /**
   * @brief Computes solution vector for dipole parameters and returns it in available format
   * @tparam Container
   * @return Container containing solution for the system
   */
  template<typename Container=common_types::ReturnToDataType<ReturnType::EIGEN_VECTOR>>
  Container Solve();

  const IntegrableFunction &GetIfunction() const {
    return ifunction_;
  }

  const DirectionGraph &GetI2Function() const {
    return i_2_function_;
  }

  /**
   * @brief Returns reference to the right part for system of equations
   */
  const EigenVec &GetRightPart();

  /**
   * @brief Retrieves matrix computed for system of equations
   */
  MatrixType GetMatrixx();

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
  FloatType GetDistance(size_t i1, size_t i2, Container &xi);

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
  Eigen::Vector<FloatType, 2> TwoDVecDifference(size_t i1, size_t i2, Container &xi);

  /**
   * @brief Intializes dimensions for system of equations matrices
   */
  void InitArrays();

  /*
   * @brief Calculates temporary 2 by 2 matrices to facilitate computation
   */
  void GetMatrices(const Eigen::Vector<FloatType, 2> &rim, FloatType r_mode, Eigen::Matrix<FloatType, 2, 2> &k_1,
                   Eigen::Matrix<FloatType, 2, 2> &k_2) const;

  /**
   * @brief Recalculates matrix values for dipole coordinates vector
   * @tparam Container
   * @param xi
   */
  template<typename Container>
  void SetMatrixes(const Container &xi);

  MatrixType m_1_; ///< upper left block of matrix
  MatrixType m_2_; ///< down left block of matrix

  Eigen::Vector<FloatType, Eigen::Dynamic> f_; ///< right part of system of equations

  FloatType an_ = params::a; ///< a_n coefficient value
  int n_; /// < size of the system
  IntegrableFunction ifunction_; ///< numeric/analytic representation of the target function.
  DirectionGraph i_2_function_; ///< analytic representation of the target function
};

template<typename Container>
requires HasSizeMethod<Container>
Dipoles::Dipoles(const Container &xi) {
  if constexpr (not HasBracketsNested<Container>) {
    this->n_ = xi.size() / 2;
  } else {
    this->n_ = xi[0].size();
  }
  InitArrays();
  SetMatrixes(xi);
}

template<typename Container>
void Dipoles::SetMatrixes(const Container &xi) {
  std::vector<std::pair<int, int>> sectors(4);
  sectors[0] = {0, 0};
  sectors[1] = {0, 2 * n_};
  sectors[2] = {2 * n_, 0};
  sectors[3] = {2 * n_, 2 * n_};
  for (int i = 0; i < n_; ++i) {//MAC
    for (int m = 0; m < n_; ++m) {
      if (i == m) {
        Eigen::Matrix<FloatType, 2, 2> id = Eigen::Matrix<FloatType, 2, 2>::Identity() *
            (params::omega0 * params::omega0 -
                params::omega * params::omega);
        m_1_.template block<2, 2>(2 * i, 2 * m) = id;
      } else {

        Eigen::Vector<FloatType, 2> rim = TwoDVecDifference(m, i, xi);
        auto r_mode = GetDistance(i, m, xi);
        Eigen::Matrix<FloatType, 2, 2> k1;
        Eigen::Matrix<FloatType, 2, 2> k2;
        GetMatrices(rim, r_mode, k1, k2);
        auto arg = params::omega * r_mode / params::c;
        Eigen::Matrix<FloatType, 2, 2> tmpmatr = -an_ * (k1 * cos(arg) - k2 * sin(arg));
        m_1_.template block<2, 2>(2 * i, 2 * m) = tmpmatr;
      }
    }
  }

  for (int i = 0; i < n_; ++i) {//MBC
    for (int m = 0; m < n_; ++m) {
      if (i == m) {
        Eigen::Matrix<FloatType, 2, 2> id =
            Eigen::Matrix<FloatType, 2, 2>::Identity() * (params::yo * params::omega);
        m_2_.template block<2, 2>(2 * i, 2 * m) = -id;
      } else {

        Eigen::Vector<FloatType, 2> rim = TwoDVecDifference(m, i, xi);
        auto r_mode = GetDistance(i, m, xi);
        Eigen::Matrix<FloatType, 2, 2> k1;
        Eigen::Matrix<FloatType, 2, 2> k2;
        GetMatrices(rim, r_mode, k1, k2);
        auto arg = params::omega * r_mode / params::c;

        Eigen::Matrix<FloatType, 2, 2> tmpmatr = -an_ * (k2 * cos(arg) + k1 * sin(arg));

        m_2_.template block<2, 2>(2 * i, 2 * m) = tmpmatr;
      }

    }
  }
}

template<typename Container>
requires HasSizeMethod<Container>
void Dipoles::SetNewCoordinates(const Container &xi) {
  if (this->f_.size() != xi.size() * 2) {
    this->n_ = xi.size() / 2;
    InitArrays();
  }
  SetMatrixes(xi);
}

template<typename Container, typename Container2>
requires HasSizeMethod<Container> && HasSizeMethod<Container2>
void Dipoles::GetFullFunction(const Container &xi, const Container2 &sol) {
  this->ifunction_ = [&xi, &sol](FloatType theta, FloatType phi, FloatType t) {
    int n = xi.size() / 2;
    FloatType res = 0;
    FloatType s[2] = {cos(phi), sin(phi)};
    FloatType ress[3] = {0, 0, 0};
    for (int i = 0; i < n; ++i) {
      FloatType ri[2] = {GetElement(xi, 0, i, n),
                         GetElement(xi, 1, i, n)};
      FloatType ys = (ri[1] * cos(phi) - ri[0] * sin(phi)) * sin(theta);
      FloatType t0 = t - ys / params::c;
      FloatType aI[2] = {GetValue(sol, 2 * i), GetValue(sol, 2 * i + 1)};
      FloatType bi[2] = {GetValue(sol, 2 * i + 2 * n), GetValue(sol, 2 * i + 1 + 2 * n)};

      FloatType di[2] = {aI[0] * cos(params::omega * t0) + bi[0] * sin(params::omega * t0),
                         aI[1] * cos(params::omega * t0) + bi[1] * sin(params::omega * t0)};

      FloatType vi[2] = {params::omega * (bi[0] * cos(params::omega * t0)
          - aI[0] * sin(params::omega * t0)) / params::c,
                         params::omega * (bi[1] * cos(params::omega * t0)
                             - aI[1] * sin(params::omega)) / params::c};
      FloatType ai[2] = {-pow(params::omega, 2) * di[0], -pow(params::omega, 2) * di[1]};

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
    for (FloatType elem : ress) {
      res += elem * elem;
    }
    return res;
  };

  this->i_2_function_ = [&xi, &sol](FloatType phi, FloatType theta) {
    int n = xi.size() / 2;
    FloatType omega0 = params::omega;
    FloatType t_0 = M_PI * 2 / omega0;
    FloatType res;
    Eigen::Vector<FloatType, 2> resxy = {0.0, 0.0};
    FloatType resz = 0.0;
    FloatType o2 = pow(omega0, 2);
    FloatType sinth2 = pow(sin(theta), 2);
    Eigen::Vector<FloatType, 2> s = {cos(phi),
                                     sin(phi)};
    for (int i = 0; i < n; ++i) {
      FloatType ri[2] = {GetElement(xi, 0, i, n), GetElement(xi, 1, i, n)};
      FloatType ys = (ri[1] * cos(phi) - ri[0] * sin(phi)) * sin(theta);
      Eigen::Vector<FloatType, 2> Ai = {GetValue(sol, 2 * i), GetValue(sol, 2 * i + 1)};
      Eigen::Vector<FloatType, 2> bi = {GetValue(sol, 2 * i + 2 * n), GetValue(sol, 2 * i + 1 + 2 * n)};

      FloatType argument = omega0 * ys / params::c;
      FloatType ais = Ai.dot(s);
      FloatType bis = bi.dot(s);
      Eigen::Vector<FloatType, 2> a_bis = Ai * bis;
      Eigen::Vector<FloatType, 2> b_ais = bi * ais;

      Eigen::Vector<FloatType, 2> pc_1_i = ((s * ais * sinth2 - Ai) * cos(argument) -
          (s * bis * sinth2 - bi) * sin(argument));
      Eigen::Vector<FloatType, 2> ps_1_i = ((s * ais * sinth2 - Ai) * sin(argument) +
          (s * bis * sinth2 - bi) * cos(argument));
      Eigen::Vector<FloatType, 2> pcomi = -(omega0 / params::c) * (sin(theta)) * (a_bis - b_ais);

      FloatType pci = (ais * cos(argument) - bis * sin(argument));
      FloatType psi = (ais * sin(argument) + bis * cos(argument));

      Eigen::Vector<FloatType, 2> rj;
      FloatType ysj;
      Eigen::Vector<FloatType, 2> aj;
      Eigen::Vector<FloatType, 2> bj;
      FloatType argumentj;

      FloatType ajs;
      FloatType bjs;

      FloatType pcj;
      FloatType psj;

      Eigen::Vector<FloatType, 2> pc_1_j;
      Eigen::Vector<FloatType, 2> ps_1_j;
      Eigen::Vector<FloatType, 2> pcomj;

      Eigen::Vector<FloatType, 2> a_bjs;
      Eigen::Vector<FloatType, 2> b_ajs;
      for (int j = 0; j < i; ++j) {
        rj = {GetElement(xi, 0, j, n), GetElement(xi, 1, j, n)};
        ysj = (rj[1] * cos(phi) - rj[0] * sin(phi)) * sin(theta);

        aj = {GetValue(sol, 2 * j), GetValue(sol, 2 * j + 1)};
        bj = {GetValue(sol, 2 * j + 2 * n), GetValue(sol, j + 1 + 2 * n)};

        argumentj = omega0 * ysj / params::c;
        ajs = aj.dot(s);
        bjs = bj.dot(s);

        a_bjs = aj * bjs;
        b_ajs = bi * ajs;

        pcj = (ajs * cos(argumentj) - bjs * sin(argumentj));
        psj = (ajs * sin(argumentj) + bjs * cos(argumentj));

        pc_1_j = ((s * ajs * sinth2 - aj) * cos(argumentj) -
            (s * bjs * sinth2 - bj) * sin(argumentj));
        ps_1_j = ((s * ajs * sinth2 - aj) * sin(argumentj) +
            (s * bjs * sinth2 - bj) * cos(argumentj));
        pcomj = -(omega0 / params::c) * (sin(theta)) * (a_bjs - b_ajs);

        Eigen::Vector<FloatType, 2> rij_xy =
            pc_1_i.cwiseProduct(pc_1_j) + ps_1_i.cwiseProduct(ps_1_j)
                + 2 * pcomi.cwiseProduct(pcomj);

        FloatType rij_z = (pci * pcj + psi * psj);
        resz += rij_z;
        resxy += rij_xy;
      }
      Eigen::Vector<FloatType, 2> ri_xy =
          (pc_1_i.cwiseProduct(pc_1_i) + ps_1_i.cwiseProduct(ps_1_i)
              + 2 * pcomi.cwiseProduct(pcomi)) / 2.0;
      resxy += ri_xy;

      FloatType ri_xz = (pci * pci + psi * psi) / 2;
      resz += ri_xz;
    }
    resxy = resxy * t_0 * o2 * o2;
    resz = resz * t_0 * (o2 * o2 * pow(sin(theta) * cos(theta), 2));
    res = resxy.sum() + resz;
    return res;

  };

}

template<typename Container>
requires HasBracketOperator<Container>
FloatType Dipoles::GetDistance(size_t i1, size_t i2, Container &xi) {
  FloatType d1;
  FloatType d2;
  if constexpr (my_concepts::HasBracketsNested<Container>) {
    d1 = xi[0][i1] - xi[0][i2];
    d2 = xi[1][i1] - xi[1][i2];
  } else {
    d1 = xi[i1] - xi[i2];
    d2 = xi[i1 + n_] - xi[i2 + n_];
  }
  return std::sqrt(d1 * d1 + d2 * d2);
}

template<typename Container>
requires HasBracketOperator<Container>
Eigen::Vector<FloatType, 2> Dipoles::TwoDVecDifference(size_t i1, size_t i2, Container &xi) {
  FloatType d1;
  FloatType d2;
  if constexpr (my_concepts::HasBracketsNested<Container>) {
    d1 = xi[0][i1] - xi[0][i2];
    d2 = xi[1][i1] - xi[1][i2];
  } else {
    d1 = xi[i1] - xi[i2];
    d2 = xi[i1 + n_] - xi[i2 + n_];
  }
  return {d1, d2};
}
}
