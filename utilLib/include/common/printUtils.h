#pragma once

#define EIGENF(form) enumTo.at(static_cast<size_t>(form))
#define FORMAT_OPT(NAME)  { #NAME,  ioFormat::NAME}

#include <iosfwd>
#include <limits>


#include <eigen3/Eigen/Dense>
#include "common/sharedDeclarations.h"
#include "common/errorHandling.h"

/// printUtils namespace
namespace printUtils {

    using EFormat = Eigen::IOFormat;

    static constexpr size_t defaultPrec = std::numeric_limits<shared::FloatType>::digits10;

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
        ///Basic print format(look a lot alike json)
        Serializable,
        //Human-readable representation for printed struct
        HumanReadable
    };

    /// String lookup map for ioformat
    static const std::unordered_map<std::string, ioFormat> stringToIoFormat =
            {
                    FORMAT_OPT(Serializable),
                    FORMAT_OPT(HumanReadable)
            };

    /// String lookup table for strings
    static constexpr std::array<const char *, 2> ioToStr =
            {
                    "Serializable",
                    "HumanReadable"
            };

    /**
     * @brief Printer for ioFormat
     * @param out
     * @param form
     */
    std::ostream &operator<<(std::ostream &out, const ioFormat &form);

    /**
     * @brief Parser for ioFormat
     * @param in
     * @param form
     * @throws InvalidOption
     */
    std::istream &operator>>(std::istream &in, ioFormat &form);

    /**
     * @brief Parser for EFormat
     * @param is
     * @param fmt
     */
    std::istream &operator>>(std::istream &is, EFormat &fmt);

    /**
     * @brief Printer for EFormat
     * @details This implementation uses \" \" to separate escape characters inside separators
     * @param os
     * @param fmt
     */
    std::ostream &operator<<(std::ostream &os, const EFormat &fmt);

    /**
     * @brief elementwise comparison between EFormat
     * @param lhs
     * @param rhs
     * @return
     */
    bool operator==(const EFormat &lhs, const EFormat &rhs);

    /**
     * @brief A guard class to save current iostream state
     * @details Object of this class stores current iostream state and restores it when it goes out of scope
     */
    class IosStatePreserve {
    public:
        explicit IosStatePreserve(std::ostream &out);

        IosStatePreserve(IosStatePreserve &oother) = delete;

        ~IosStatePreserve();

        [[nodiscard]] std::ios_base::fmtflags getFlags() const;

    protected:
        std::ios_base::fmtflags flags_;
        std::ostream &out_;
    };

    /**
     * @brief Sets iostream precision to a fixed value after saving it's state
     */
    class IosStateScientific : public IosStatePreserve {
    public:
        using IosStatePreserve::IosStatePreserve;

        explicit IosStateScientific(std::ostream &out, size_t precision = defaultPrec);

        ~IosStateScientific();

    protected:
        size_t oldPrecision;
    };

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
