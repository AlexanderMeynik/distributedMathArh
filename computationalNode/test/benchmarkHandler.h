#pragma once
#ifndef DIPLOM_BENCHMARKHANDLER_H
#define DIPLOM_BENCHMARKHANDLER_H

#include <functional>
#include <optional>
#include <concepts>
#include <tuple>

#include "fileHandler.h"
#include "parallelUtils/commonDeclarations.h"
/// benchUtils namespace
namespace benchUtils {
    namespace fu= fileUtils;
    namespace tu=timing;
    template<typename ratio=std::milli>
    using clockType= tu::chronoClockTemplate<ratio>;

    using clk1=clockType<std::micro>;


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
    std::string tupleToString(const TupleT& tp,
                              const char* delim=",",const char* left="(",
                              const char* right=")");

    /**
     * @brief Compile time function to compute cartessian product for arbitrary number of arrays
     * @tparam ARRAYS
     * @param arrays
     */
    template<typename...ARRAYS>
    constexpr auto cartesianProduct(ARRAYS...arrays) {
        using type = std::tuple<typename ARRAYS::value_type...>;

        constexpr std::size_t N = (1 * ... * arrays.size());

        std::array<std::size_t, sizeof...(arrays)> dims{arrays.size()...};
        for (std::size_t i = 1; i < dims.size(); ++i) { dims[i] *= dims[i - 1]; }

        return [&]() {
            std::array<type, N> result;

            for (std::size_t i = 0; i < result.size(); ++i) {
                [&]<std::size_t... Is>(std::index_sequence<Is...>) {
                    auto idx = std::make_tuple(((i * std::get<Is>(dims)) / N) % arrays.size() ...);
                    result[i] = std::make_tuple(arrays[std::get<Is>(idx)]...);

                }(std::make_index_sequence<sizeof...(arrays)>{});
            }

            return result;
        }();
    }


    /**
     * @brief Class that handles benchmark file creation time measurements and other functionality
     */
    class benchmarkHandler {
        public:

        //todo check whether benchmark name can be used as a path
        /**
         *
         * @param name -
         * @param path - may be optional -> current directory will be used
         */
        explicit benchmarkHandler(std::string_view name,
                         std::optional<std::string>path=std::nullopt);


        /**
         * @tparam ARRAYS - input arrays types for cartessian product
         * @param benchNameGenerator - function that generates iteration name
         * @param benchFunction - function that performs benchmark iteration
         * @param arrays - arrays for cartessian product
         */
        static inline std::filesystem::path ddpath="timers";
        template<typename...ARRAYS>
        constexpr auto runBenchmark(const std::function<std::string(typename ARRAYS::value_type ...)>&benchNameGenerator,
                                    const std::function<void(clk1 &,fu::fileHandler&, typename ARRAYS::value_type ...)>&benchFunction,
                                    ARRAYS...arrays);
        /**
         * @brief Prints each entry in clk to a designated file
         * @param clk
         * @param preprint - is printed before each entry
         * @param delim - delimeter that is printed after each timer record
         */
        void snapshotTimers(clk1&clk,const std::string&preprint,const std::string&delim="\n");

        /**
         * @brief Prints global clock results into designated file
         */
        void printClocks()
        {
            auto name="benchTimers.txt";
            fh.upsert(name);
            fh.output(name,clkArr);
        }
        ~benchmarkHandler()
        {
            printClocks();
            fh.closeFiles();
        }


    private:
        fu::fileHandler fh;
        std::string benchmarkName;
        clk1 clkArr;
    };

    template<typename TupleT, std::size_t TupSize>
    std::string tupleToString(const TupleT& tp,
                              const char* delim,const char* left,
                              const char* right)
    {
        return []<typename TupleTy, std::size_t... Is>(const TupleTy& tp,const char* delim,const char* left,
                                             const char* right,std::index_sequence<Is...>)->std::string
        {
            std::stringstream res;
            res<<left;

            (...,(res<<(Is==0?"":delim)<<get<Is>(tp)));

            res<<right;
            return res.str();
        }.operator()(tp,delim,left,right,std::make_index_sequence<TupSize>{});
    }


    template<typename... ARRAYS>
    constexpr auto
    benchmarkHandler::runBenchmark(const std::function<std::string(typename ARRAYS::value_type ...)>&benchNameGenerator,
                                   const std::function<void(clk1 &,fu::fileHandler&, typename ARRAYS::value_type ...)>&benchFunction,
                                   ARRAYS...arrays) {
        clk1 clkdc= this->clkArr;

        auto cart= cartesianProduct(std::forward<ARRAYS>(arrays)...);
        auto size=std::tuple_size<typename decltype(cart)::value_type>{};
        size_t innerCounter=0;
        auto lambda=[this,&benchFunction]<ARRAYS>(typename ARRAYS::value_type ... vals)
        {
            return benchFunction(clkArr,fh,std::forward<typename ARRAYS::value_type>(vals)...);
        };


        fileUtils::createDirIfNotPresent(fh.getParentPath()/ddpath);

        for (auto&tuple:cart) {
            auto itername=benchmarkName+std::apply(benchNameGenerator, tuple);
            std::apply(lambda,tuple);
            snapshotTimers(clkArr, tupleToString(tuple,"\t","","")+"\t");

            innerCounter++;
            clkdc.advance(clkArr);
            clkArr.reset();
        }
        clkArr=clkdc;
    }
}

#endif //DIPLOM_BENCHMARKHANDLER_H
