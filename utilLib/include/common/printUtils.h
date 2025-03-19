#pragma once
#ifndef DATA_DEDUPLICATION_SERVICE_PRINTUTILS_H
#define DATA_DEDUPLICATION_SERVICE_PRINTUTILS_H

#define EIGENF(form) enumTo.at(static_cast<size_t>(form))
#define FORMAT_OPT(NAME)  { #NAME,  ioFormat::NAME}

#include <iosfwd>
#include <limits>

#include <eigen3/Eigen/Dense>
#include <unordered_map>
#include <vector>
#include "common/errorHandling.h"
//#include <fmt/format.h>

/// printUtils namespace
namespace printUtils {

    DEFINE_EXCEPTION(InvalidOption,"Option {} does not exist!",const std::string&);

    using EFormat = Eigen::IOFormat;

    /**
     * @brief Prints tuple to string
     * @tparam TupleT
     * @tparam TupSize
     * @param tp
     * @param delim - is printed after each tuple element
     * @param left - is printed before all tuple elements
     * @param right  - is printed after all tuple elements
     */
    template<typename TupleT, std::size_t TupSize = std::tuple_size_v<TupleT>>
    std::string tupleToString(const TupleT &tp,
                              const char *delim = ",", const char *left = "(",
                              const char *right = ")");

    /**
     * @brief A guard class to save current iostream state
     * @details Object of this class stores current iostream state and restores it when it goes out of scope
     */
    class IosStatePreserve {
    public:
        explicit IosStatePreserve(std::ostream &out);

        ~IosStatePreserve();

        [[nodiscard]] std::ios_base::fmtflags getFlags() const;

    private:
        std::ios_base::fmtflags flags_;
        std::ostream &out_;
    };

    /**
     * @brief Sets iostream precision to a fixed value after saving it's state
     */
    class IosStateScientific : public IosStatePreserve {
    public:
        using IosStatePreserve::IosStatePreserve;

        explicit IosStateScientific(std::ostream &out, long precision = std::numeric_limits<double>::max_digits10);
    };

    /**
     * @brief Eformat lookup Table
     */
    extern const std::array<EFormat, 4> enumTo;
    enum class EigenPrintFormats {
        BasicOneDimensionalVector = 0,
        VectorFormat1,
        MatrixFormat,
        MatrixFormat1
    };


    /**
     * @brief Casts EigenFormat to Eformat
     * @param fmt
     */
    const EFormat &printEnumToFormat(EigenPrintFormats fmt);


    /**
     * @brief General IOformat
     */

    enum class ioFormat {
        Serializable,
        HumanReadable
    };

    static const std::unordered_map<std::string, ioFormat> stringToIoFormat =
            {
            FORMAT_OPT(Serializable),
            FORMAT_OPT(HumanReadable)
            };

    std::ostream &operator<<(std::ostream &out, const ioFormat &form);

    std::istream &operator>>(std::istream &in, ioFormat &form);


    template<typename TupleT, std::size_t TupSize>
    std::string tupleToString(const TupleT &tp,
                              const char *delim, const char *left,
                              const char *right) {
        return []<typename TupleTy, std::size_t... Is>(const TupleTy &tp, const char *delim, const char *left,
                                                       const char *right, std::index_sequence<Is...>) -> std::string {
            std::stringstream res;
            res << left;

            (..., (res << (Is == 0 ? "" : delim) << get<Is>(tp)));

            res << right;
            return res.str();
        }.operator()(tp, delim, left, right, std::make_index_sequence<TupSize>{});
    }




}


#endif //DATA_DEDUPLICATION_SERVICE_PRINTUTILS_H
