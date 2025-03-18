#ifndef DIPLOM_PRINTUTILS_H
#define DIPLOM_PRINTUTILS_H

#include <iosfwd>
#include <limits>

#include <eigen3/Eigen/Dense>//todo use froward declaration
#include <unordered_map>
#include <vector>

/// printUtils namespace
namespace printUtils {




    /**
   * Print format function
   * @param zcFormat
   * @param ...
   */
    std::string vformat(const char *zcFormat, ...);


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
    template<typename TupleT,typename TupleT2>
    requires (std::tuple_size_v<TupleT> == std::tuple_size_v<TupleT2>)
    std::pair<bool,std::vector<std::string>> verboseTupCompare(const TupleT &tp, const TupleT2*tp2);




    template<typename ...Args >
    class parametrizedException:std::exception
    {
    public:
        parametrizedException(Args && ... args){
            params={std::forward<Args>(args)...};

            message= "Exception params "+tupleToString(params);
        }

        virtual const char* what() const noexcept
        {
            return message.data();
        };

        auto getParams()
        {
            return params;
        }
        bool operator==(const parametrizedException<Args...>&second)
        {
            return verboseTupCompare(this->params,second.params).first;
        }

    private:
        std::tuple<Args...> params;
        std::string message;
    };
    template<class T>
    using invalidOption= parametrizedException<T>;


        /**
         * @b A guard class to save current iostream state
         * @d Object of this class stores current iostream state and restores it when it goes out of scope
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
     * @b Sets iostream precision to a fixed value after saving it's state
     */
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


    enum class ioFormat
    {
        Serializable,
        HumanReadable
    };

    static const std::unordered_map<std::string,ioFormat> stringToIoFormat=
            {{"Serializable",ioFormat::Serializable},
             {"HumanReadable",ioFormat::HumanReadable}};

    std::ostream &operator<<(std::ostream &out,const ioFormat&form);

    std::istream &operator>>(std::istream &in,ioFormat&form);






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

    template<typename TupleT,typename TupleT2>
    requires (std::tuple_size_v<TupleT> == std::tuple_size_v<TupleT2>)
    std::pair<bool,std::vector<std::string>> verboseTupCompare(const TupleT &tp, const TupleT2*tp2)
    {
        std::pair<bool,std::vector<std::string>> pp={true,{}};
        constexpr auto tS=std::tuple_size_v<TupleT>;
        for (size_t i = 0; i < tS; ++i) {
            if(get<i>(tp)!=get<i>(tp2))//todo will this work?
            {
                pp.second.push_back(std::to_string(get<i>(tp))+"!="+get<i>(tp2));
                pp.first= false;
            }
        }

       /* return [&]<typename TupleTy,typename TupleTy2, std::size_t... Is>(const TupleTy &tp,const TupleTy2 &tp2,
                                                                         std::index_sequence<Is...>) {




            std::stringstream res;
            res << left;

            (..., (res << (Is == 0 ? "" : delim) << get<Is>(tp)));

            res << right;
            return res.str();
        }.operator()(tp, std::make_index_sequence<std::tuple_size_v<TupleT>>{});*/

        return pp;
    }


}


#endif //DIPLOM_PRINTUTILS_H
