#ifndef DIPLOM_PRINTUTILS_H
#define DIPLOM_PRINTUTILS_H

#include <iosfwd>
#include <limits>

#include <eigen3/Eigen/Dense>

/// printUtils namespace
namespace printUtils {
    class IosStatePreserve {
    public:
        explicit IosStatePreserve(std::ostream &out);

        ~IosStatePreserve();

        std::ios_base::fmtflags getFlags() const;

    private:
        std::ios_base::fmtflags flags_;
        std::ostream &out_;
    };

    class IosStateScientific : public IosStatePreserve {
    public:
        using IosStatePreserve::IosStatePreserve;

        explicit IosStateScientific(std::ostream &out, int precision = std::numeric_limits<double>::max_digits10);
    };

    static inline std::array<Eigen::IOFormat, 4> enumTo
            = {Eigen::IOFormat(Eigen::StreamPrecision, Eigen::DontAlignCols, "\t", "", "[", "]", "", "\n") = 0,
                    Eigen::IOFormat(Eigen::StreamPrecision, Eigen::DontAlignCols, "\t", "", "", "", "", "\n"),
                    Eigen::IOFormat(Eigen::StreamPrecision, Eigen::DontAlignCols, "\t", "\n", "[", "]", "", "\n"),
                    Eigen::IOFormat(Eigen::StreamPrecision, Eigen::DontAlignCols, "\t", "\n", "", "", "", "\n")
            };
    enum class EigenPrintFormats {
        BasicOneDimensionalVector = 0,
        VectorFormat1,
        MatrixFormat,
        MatrixFormat1

    };

    Eigen::IOFormat &printEnumToFormat(EigenPrintFormats fmt);

}





#endif //DIPLOM_PRINTUTILS_H
