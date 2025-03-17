#include <eigen3/Eigen/Dense>
#include <iomanip>
#include <iostream>
#include <valarray>
#include <vector>
#include <array>

#include <concepts>


template<typename T>
concept has_data=requires(T &t)
{
    { t.data() }->std::common_with<typename T::value_type *>;
};

/*template< typename T>
requires std::is_floating_point_v<T>
int printEigenVectorMap(Eigen::Map<Eigen::Vector<T
        , Eigen::Dynamic>>&map, const Eigen::IOFormat& format = Eigen::IOFormat())
{

}*/


//todo printer for eigen things
//printer for solutions(with and without additions)
//todo https://stackoverflow.com/questions/16470893/how-do-you-make-a-matrix-out-of-vectors-in-eigen/16476372 for solution printing


/**
     * @brief concept to check whether T has value_type
     * @tparam T
     */
template<typename T>
concept valueTyped = requires { typename T::value_type; };

/**
 * @brief Concept to check whether type T has subscript operator
 * @tparam T
 */
template<typename T>
concept HasBracketOperator = valueTyped<T> && requires(T t, size_t i) {
    { t[i] };
};

/**
 * @brief Concept to check whether type T hash size method
 * @tparam T
 */
template<typename T>
concept HasSizeMethod = requires(T a) {
    { a.size() } -> std::convertible_to<std::size_t>;
};


/**
 * @brief Checks whether T is a one dimensional continuous array of elements
 * @tparam T
 */
template<typename T>
concept isOneDimensionalContinuous = HasBracketOperator<T> && HasSizeMethod<T>;


template<typename Container>
//todo requires size and [].
requires isOneDimensionalContinuous<Container>
int printVectorEigen(std::ostream &out, Container &solution_, const Eigen::IOFormat &format = Eigen::IOFormat()) {


    Eigen::Map<Eigen::Vector<std::remove_reference_t<typename Container::value_type>, Eigen::Dynamic>> map(
            &solution_[0], solution_.size());

    out << map.format(format);
    return 0;
}

template<typename Container>
requires isOneDimensionalContinuous<Container>
int printMatrixEigen(std::ostream &out, Container &solution_, const std::array<size_t, 2> &dims,
                     const Eigen::IOFormat &format = Eigen::IOFormat()) {
    Eigen::Map<Eigen::Matrix<std::remove_reference_t<typename Container::value_type>, Eigen::Dynamic, Eigen::Dynamic>> map(
            &solution_[0], dims[0], dims[1]);
    out << map.format(format);
    return 0;
}

template<typename Container>
requires isOneDimensionalContinuous<Container>
Eigen::Map<Eigen::Vector<std::remove_reference_t<typename Container::value_type>, Eigen::Dynamic>>
toEigenVector(Container &container) {
    if (container.size() == 0) {
        throw std::length_error("Zero input container size!");
    }


    Eigen::Map<Eigen::Vector<std::remove_reference_t<typename Container::value_type>, Eigen::Dynamic>> map(
            &container[0], container.size());


    return map;
}

template<typename Container>
requires isOneDimensionalContinuous<Container>
Eigen::Map<Eigen::RowVector<std::remove_reference_t<typename Container::value_type>, Eigen::Dynamic>>
toEigenRowVector(Container &container) {
    if (container.size() == 0) {
        throw std::length_error("Zero input container size!");
    }


    Eigen::Map<Eigen::RowVector<std::remove_reference_t<typename Container::value_type>, Eigen::Dynamic>> map(
            &container[0], container.size());


    return map;
}

template<typename Container>
requires isOneDimensionalContinuous<Container>
Eigen::Map<Eigen::Matrix<std::remove_reference_t<typename Container::value_type>, Eigen::Dynamic, Eigen::Dynamic>>
toEigenMatrix(Container &container, int columns) {
    if (container.size() == 0) {
        throw std::length_error("Zero input container size!");
    }

    if (container.size() % columns != 0) {
        throw std::length_error("Invalid solutions size to columns ratio: container.size() % columns = " +
                                std::to_string(container.size() % columns));
    }
    size_t rows = container.size() / columns;


    Eigen::Map<Eigen::Matrix<std::remove_reference_t<typename Container::value_type>, Eigen::Dynamic, Eigen::Dynamic>> map(
            &container[0], rows, columns);


    return map;
}


namespace printEnums {
    std::array<Eigen::IOFormat, 4> enumTo
            = {Eigen::IOFormat(Eigen::StreamPrecision, Eigen::DontAlignCols, "\t", "", "[", "]", "", "\n") = 0,
                    Eigen::IOFormat(Eigen::StreamPrecision, Eigen::DontAlignCols, "\t", "", "", "", "", "\n"),
                    Eigen::IOFormat(Eigen::StreamPrecision, Eigen::DontAlignCols, "\t", "\n", "[", "]", "", "\n"),
                    Eigen::IOFormat(Eigen::StreamPrecision, Eigen::DontAlignCols, "\t", "\n", "", "", "", "\n")
            };
    enum class EigenPrintFormats {//todo remake this format
        BasicOneDimensionalVector = 0,
        VecctorFormat1,
        MatrixFormat,
        MatrixFormat1
        //todo  think about mesh print formats
        //todo reimplement functions for them


        //play with precisition so that double are printed with full prec(sstream prec) +
        //Function vecToMatrCanBeRedone - maybe we can implement it's inverse(for which types?)(when we read coords)
        // 1. and /2 can be canonical(serializable+pbuff) for any vector data
        // If we add dimensions to 6.(maybe not) we can get canonical representation for our meshes
        //

    };

    Eigen::IOFormat &printEnumToFormat(EigenPrintFormats fmt) {
        return enumTo[static_cast<size_t>(fmt)];
    }
}

#include <type_traits>
#include <limits>

class IosStatePreserve {
public:
    explicit IosStatePreserve(std::ostream &out) : out_(out) {
        flags_ = out.flags();

    }

    ~IosStatePreserve() {
        out_.flags(flags_);
    }

    std::ios_base::fmtflags getFlags() const {
        return flags_;
    }

private:
    std::ios_base::fmtflags flags_;
    std::ostream &out_;
};

class IosStateScientific : public IosStatePreserve {
public:
    using IosStatePreserve::IosStatePreserve;

    explicit IosStateScientific(std::ostream &out, int precision = std::numeric_limits<double>::max_digits10)
            : IosStatePreserve(out) {
        std::cout << std::setprecision(precision) << std::fixed << std::scientific;
    }
};

int main() {
    using namespace printEnums;
    int prec = 0;
    std::cin >> prec;
    IosStateScientific ioc(std::cout, prec);

    std::valarray<double> ss = {6.232131313243e-23, 6e-23, 6e-23, 6e-23};
    std::vector<double> ass = {6.232131313243e-23, 6e-23, 6e-23, 6e-23};
    std::vector<double> ass2 = {6.232131313243e-23, 6e-23, 6e-23, 1, 1, 1};

    std::cout << 1.3e-29;
    std::cout << "\n\n\n";
    printVectorEigen(std::cout, ss);

    std::cout << "\n\n\n";

    printMatrixEigen(std::cout, ass, {2, 2}, printEnumToFormat(EigenPrintFormats::MatrixFormat1));
    std::cout << "\n\n\n";
    std::cout << toEigenMatrix(ass2, 3).format(printEnumToFormat(EigenPrintFormats::MatrixFormat1));

    std::cout << "\n\n\n";
    std::cout << toEigenVector(ass2).format(printEnumToFormat(EigenPrintFormats::MatrixFormat1));


    auto r0 = toEigenMatrix(ass2, 2);
    std::cout << r0.format(printEnumToFormat(EigenPrintFormats::MatrixFormat1));
    std::cout << r0.format(printEnumToFormat(EigenPrintFormats::MatrixFormat1));

    std::cout << "\n\n\n";
    auto rr = toEigenRowVector(ass2);
    std::cout << rr.format(printEnumToFormat(EigenPrintFormats::VecctorFormat1));


    Eigen::Vector<double, 6> vec = {6.232131313243e-23, 6e-23, 6e-23, 1, 1, 1};
    auto rr2 = toEigenVector(vec);
    std::cout << rr.format(printEnumToFormat(EigenPrintFormats::VecctorFormat1));

    int a = 0;
}


/*
 *prec=10
 */

/* std::vector<double> ass2 ={6.232131313243e-23,6e-23,6e-23,1,1,1};
 * std::cout << toEigenMatrix(ass2, 3).format(printEnumToFormat(EigenPrintFormats::MatrixFormat1));
 * "6.2321313132e-23\t6.0000000000e-23\t1.0000000000e+00\n"
"6.0000000000e-23\t1.0000000000e+00\t1.0000000000e+00"
 */

/*
 * std::vector<double> ass2 ={6.232131313243e-23,6e-23,6e-23,1,1,1};
 * std::cout << toEigenVector(ass2).format(printEnumToFormat(EigenPrintFormats::MatrixFormat1));
 * "6.2321313132e-23\n"
"6.0000000000e-23\n"
"6.0000000000e-23\n"
"1.0000000000e+00\n"
"1.0000000000e+00\n"
"1.0000000000e+00"
 */


/*
 * std::vector<double> ass2 ={6.232131313243e-23,6e-23,6e-23,1,1,1};
 * auto rr=toEigenRowVector(ass2);
    std::cout << rr.format(printEnumToFormat(EigenPrintFormats::MatrixFormat1));

 "6.2321313132e-23	6.0000000000e-23	6.0000000000e-23	1.0000000000e+00	1.0000000000e+00	1.0000000000e+00"
  */