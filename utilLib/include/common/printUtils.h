#pragma once
#ifndef DATA_DEDUPLICATION_SERVICE_PRINTUTILS_H
#define DATA_DEDUPLICATION_SERVICE_PRINTUTILS_H

#define EIGENF(form) enumTo.at(static_cast<size_t>(form))

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
     * @brief Compares 2 tuples and return the verbose output result
     * @tparam TupleT
     * @tparam TupleT2
     * @param tp First tuple
     * @param tp2 Second tuple
     */
    //todo use expected
    template<typename TupleT, typename TupleT2>
    requires (std::tuple_size_v<TupleT> == std::tuple_size_v<TupleT2>)
    std::pair<bool, std::vector<std::string>> verboseTupCompare(const TupleT &tp, const TupleT2 *tp2);




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

    static const std::array<EFormat, 4> enumTo = {{
                                                                  // Format 0: Matrix with row enclosures "[...]"
                                                                  EFormat(
                                                                          Eigen::StreamPrecision,  // Precision
                                                                          Eigen::DontAlignCols,    // Flags (no column alignment)
                                                                          ",",                    // Coefficient separator (between elements in a row)
                                                                          "",                      // Row separator (between rows)
                                                                          "[",                     // Row prefix
                                                                          "]",                     // Row suffix
                                                                          "",                      // Matrix prefix
                                                                          "\n"                     // Matrix suffix
                                                                  ),
                                                                  // Format 1: Simple space-separated values
                                                                  EFormat(
                                                                          Eigen::StreamPrecision,
                                                                          Eigen::DontAlignCols,
                                                                          "\t",
                                                                          "",
                                                                          "",
                                                                          "",
                                                                          "",
                                                                          "\n"
                                                                  ),
                                                                  // Format 2: Row-enclosed with newline separators
                                                                  EFormat(
                                                                          Eigen::StreamPrecision,
                                                                          Eigen::DontAlignCols,
                                                                          "\t",
                                                                          "\n",
                                                                          "[",
                                                                          "]",
                                                                          "",
                                                                          "\n"
                                                                  ),
                                                                  // Format 3: Newline-separated rows
                                                                  EFormat(
                                                                          Eigen::StreamPrecision,
                                                                          Eigen::DontAlignCols,
                                                                          "\t",
                                                                          "\n",
                                                                          "",
                                                                          "",
                                                                          "",
                                                                          "\n"
                                                                  )
                                                          }};
    enum class EigenPrintFormats {
        BasicOneDimensionalVector = 0,
        VectorFormat1,
        MatrixFormat,
        MatrixFormat1
    };


    const EFormat &printEnumToFormat(EigenPrintFormats fmt);


    enum class ioFormat {
        Serializable,
        HumanReadable
    };
    #define FORMAT_OPT(NAME)  { #NAME,  ioFormat::NAME}

    static const std::unordered_map<std::string, ioFormat> stringToIoFormat =
            {FORMAT_OPT(Serializable),
             FORMAT_OPT(HumanReadable)
             /*{"HumanReadable", ioFormat::HumanReadable}*/};

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

    template<typename TupleT, typename TupleT2>
    requires (std::tuple_size_v<TupleT> == std::tuple_size_v<TupleT2>)
    std::pair<bool, std::vector<std::string>> verboseTupCompare(const TupleT &tp, const TupleT2 *tp2) {
        std::pair<bool, std::vector<std::string>> pp = {true, {}};
        constexpr auto tS = std::tuple_size_v<TupleT>;
        for (size_t i = 0; i < tS; ++i) {
            if (get<i>(tp) != get<i>(tp2))//todo will this work?
            {
                pp.second.push_back(std::to_string(get<i>(tp)) + "!=" + get<i>(tp2));
                pp.first = false;
            }
        }


        return pp;
    }


}


#endif //DATA_DEDUPLICATION_SERVICE_PRINTUTILS_H
