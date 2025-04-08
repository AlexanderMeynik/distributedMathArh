#include <eigen3/Eigen/Dense>
#include <iomanip>
#include <iostream>
#include <valarray>
#include <vector>
#include <array>

#include <concepts>
#include "common/Printers.h"
#include "common/Parsers.h"

using namespace print_utils;

template<typename Container>
requires isOneDimensionalContinuous<Container>
int PrintMatrixEigen(std::ostream &out, const Container &solution, const std::array<size_t, 2> &dims,
                     const Eigen::IOFormat &format = Eigen::IOFormat()) {
  Eigen::Map<const Eigen::Matrix<typename Container::value_type, Eigen::Dynamic, Eigen::Dynamic>> map(
      &solution[0], dims[0], dims[1]);
  out << map.format(format);
  return 0;
}

DEFINE_EXCEPTION(lessException, "{}<{}", info, int, int)

int main() {

  int prec = 0;
  std::cin >> prec;
  IosStateScientific ioc(std::cout, prec);
  const std::array<std::valarray<double>, 2> kAs1{{{6.232131313243e-23, 6e-23}, {6e-23, 6e-23}}};
  const std::valarray<double> kSs = {6.232131313243e-23, 6e-23, 6e-23, 6e-23};
  std::valarray<double> ss2 = {6.232131313243e-23, 6e-23, 6e-23, 6e-23};
  const std::vector<double> kAsap = {6.232131313243e-23, 6e-23, 6e-23, 6e-23};
  std::vector<double> ass2 = {6.232131313243e-23, 6e-23, 6e-23, 1, 1, 1};


  //if yes can we somehow cast array<vecs> to one eigen vec
  PrintCoordinates(std::cout, kSs);
  PrintCoordinates(std::cout, ss2);

  PrintCoordinates(std::cout, kSs, IoFormat::HUMAN_READABLE);
  PrintSolution(std::cout, kSs, IoFormat::HUMAN_READABLE);
  std::cout << ContinuousToJson(kSs).toStyledString();

  std::cout << ToEigenMatrix(kSs, 1).format(EIGENF(EigenPrintFormats::MATRIX_FORMAT_1));

  try {
    throw lessException(1, 2);
  }
  catch (myException &ll) {
    std::cout << ll.what() << '\n';
    std::cout << ENUM_TO_STR(ll.getSev(), sevToStr) << '\n';
  }

  mesh_storage::MeshCreator mm{};
  mm.ConstructMeshes({2, 5}, {0.0, 2.0, 1.0, 10.});

  mm.ApplyFunction([](double a, double b) {
    return a * b - a;
  });

  std::stringstream sstr;
  PrintMesh(std::cout, mm);
  PrintMesh(sstr, mm);

  auto new_mesh = ParseMeshFrom(sstr);

  std::cout << '\n';
  PrintMesh(std::cout, new_mesh);

  std::stringstream sstr2;
  auto s1 = EIGENF(EigenPrintFormats::BASIC_ONE_DIMENSIONAL_VECTOR);
  sstr2 << s1;
  std::cout << sstr2.str() << '\n';

  EFormat sssss;
  sstr2 >> sssss;

  std::cout << ((sssss == s1) ? "equal" : "not equul");

}
