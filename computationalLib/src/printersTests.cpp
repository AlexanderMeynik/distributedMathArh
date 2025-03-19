#include <eigen3/Eigen/Dense>
#include <iomanip>
#include <iostream>
#include <valarray>
#include <vector>
#include <array>

#include <concepts>
#include "common/Printers.h"

using namespace printUtils;

template<typename Container>
requires isOneDimensionalContinuous<Container>
int printMatrixEigen(std::ostream &out, const Container &solution_, const std::array<size_t, 2> &dims,
                     const Eigen::IOFormat &format = Eigen::IOFormat()) {
    Eigen::Map<const Eigen::Matrix<typename Container::value_type, Eigen::Dynamic, Eigen::Dynamic>> map(
            &solution_[0], dims[0], dims[1]);
    out << map.format(format);
    return 0;
}




DEFINE_EXCEPTION(lessException,"{}<{}",int,int)

int main() {
    int prec = 0;
    std::cin >> prec;
    IosStateScientific ioc(std::cout, prec);

    const std::valarray<double> ss = {6.232131313243e-23, 6e-23, 6e-23, 6e-23};
    std::valarray<double> ss2 = {6.232131313243e-23, 6e-23, 6e-23, 6e-23};
    const std::vector<double> asap = {6.232131313243e-23, 6e-23, 6e-23, 6e-23};
    std::vector<double> ass2 = {6.232131313243e-23, 6e-23, 6e-23, 1, 1, 1};

    //todo does this suppord array of 2 vecs
    //if yes can we somehow cast array<vecs> to one eigen vec
    printCoordinates(std::cout,ss);
    printCoordinates(std::cout,ss2);


    printCoordinates(std::cout,ss,ioFormat::HumanReadable);
    printSolution(std::cout,ss,ioFormat::HumanReadable);
    std::cout<<continuousToJson(ss).toStyledString();


    std::cout<<toEigenMatrix(ss,1).format(EIGENF(EigenPrintFormats::MatrixFormat1));

    try {
        //throw InvalidOption(std::string{"ss"});
        throw  lessException(1,2);
    }
    catch (std::logic_error&ll)
    {
        std::cout<<ll.what()<<'\n';
    }
}
