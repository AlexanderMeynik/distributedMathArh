#include <eigen3/Eigen/Dense>
#include <iomanip>
#include <iostream>
#include <valarray>
#include <vector>
#include <array>

#include <concepts>
#include "common/Printers.h"
#include "common/Parsers.h"

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




DEFINE_EXCEPTION(lessException,"{}<{}",info,int,int)

int main() {

    int prec = 0;
    std::cin >> prec;
    IosStateScientific ioc(std::cout, prec);
    const std::array<std::valarray<double>,2> as1{{{6.232131313243e-23, 6e-23}, {6e-23, 6e-23}}};
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

    //todo test my expections
    try {
        //throw InvalidOption(std::string{"ss"});
        throw  lessException(1,2);
    }
    catch (MyException&ll)
    {
        std::cout<<ll.what()<<'\n';
        std::cout<<ENUM_TO_STR(ll.getSev(),sevToStr)<<'\n';
    }
    //todo test mesh print/parse
    meshStorage::MeshCreator mm{};
    mm.constructMeshes({2,5},{0.0,2.0,1.0,10.});

    mm.applyFunction([](double a,double b)
                     {
                         return a*b-a;
                     });

    std::stringstream sstr;
    printMesh(mm,std::cout);
    printMesh(mm,sstr);
    //todo print serializtion type(into vec with io format(if it exist than dont read))

    auto new_mesh= parseMeshFrom(sstr);

    std::cout<<'\n';
    printMesh(new_mesh,std::cout);


    //todo test eformat functions
    std::stringstream sstr2;
    auto s1=EIGENF(EigenPrintFormats::BasicOneDimensionalVector);
    sstr2<<s1;
    std::cout<<sstr2.str()<<'\n';

    EFormat sssss;
    sstr2>>sssss;

    std::cout<<((sssss==s1)?"equal":"not equul");


    //todo move array equal checks

    //printMesh(mm,std::cout,ioFormat::HumanReadable);
}
