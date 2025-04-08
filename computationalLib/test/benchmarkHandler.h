#pragma once

#include <functional>
#include <optional>
#include <concepts>
#include <tuple>

#include "common/sharedDeclarations.h"
#include "common/printUtils.h"
#include "fileHandler.h"
#include "parallelUtils/chronoClock.h"

/// benchUtils namespace
namespace benchUtils {
    namespace fu = fileUtils;
    using printUtils::tupleToString;

    template<typename ratio=std::milli>
    using clockType = chronoClock::chronoClockTemplate<ratio>;


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
    template<typename range=std::micro>
    class benchmarkHandler {
    public:
        using clk1 = clockType<range>;

        /**
         *
         * @param name -benchmark name
         * @param path - may be optional -> current directory will be used
         */
        explicit benchmarkHandler(std::string_view name,
                                  std::optional<std::string> path = std::nullopt);


        /**
         * @tparam ARRAYS - input arrays types for cartessian product
         * @param benchNameGenerator - function that generates iteration name
         * @param benchFunction - function that performs benchmark iteration
         * @param arrays - arrays for cartessian product
         */
        static inline std::filesystem::path ddpath = "timers";

        template<typename...ARRAYS>
        constexpr void
        runBenchmark(const std::function<std::string(typename ARRAYS::value_type ...)> &benchNameGenerator,
                     const std::function<void(clk1 &, fu::fileHandler &, size_t &,
                                              typename ARRAYS::value_type ...)> &benchFunction,
                     ARRAYS...arrays);

        /**
         * @brief Prints each entry in clk to a designated file
         * @param clk
         * @param preprint - is printed before each entry
         * @param delim - delimeter that is printed after each timer record
         * @param multiplier - timer time is multiplied by given multiplier(defaults to 1)
         */
        void snapshotTimers(clk1 &clk, const std::string &preprint, const std::string &delim = "\n",
                            size_t multiplier = 1);

        /**
         * @brief Prints global clock results into designated file
         */
        void printClocks() {
            auto name = "benchTimers.txt";
            fh.upsert(name);
            fh.output(name, clkArr);
        }

        ~benchmarkHandler() {
            printClocks();
            fh.closeFiles();
        }


    private:
        fu::fileHandler fh;
        std::string benchmarkName;
        clk1 clkArr;
        size_t mul;
    };


    template<typename range>
    void benchmarkHandler<range>::snapshotTimers(clk1 &clk, const std::string &preprint, const std::string &delim,
                                                 size_t multiplier) {
        for (auto &val: clk) {

            std::string name = ddpath / (val.first[3] + "_" + val.first[1]);
            fh.upsert(name);
            fh.output(name, preprint);
            fh.output(name, val.second.time * (unsigned long long int) (multiplier));
            fh.output(name, delim);
        }
    }


    template<typename range>
    benchmarkHandler<range>::benchmarkHandler(std::string_view name, std::optional<std::string> path):
            benchmarkName(name),
            fh(path.has_value() ? fu::getNormalAbs(path.value()) : ""),
            clkArr(),
            mul(1) {

    }

    template<typename range>
    template<typename... ARRAYS>
    constexpr void
    benchmarkHandler<range>::runBenchmark(
            const std::function<std::string(typename ARRAYS::value_type ...)> &benchNameGenerator,
            const std::function<void(clk1 &, fu::fileHandler &, size_t &,
                                     typename ARRAYS::value_type ...)> &benchFunction,
            ARRAYS...arrays) {
        clk1 clkdc = this->clkArr;

        auto cart = cartesianProduct(std::forward<ARRAYS>(arrays)...);
        auto size = std::tuple_size<typename decltype(cart)::value_type>{};
        size_t innerCounter = 0;
        auto lambda = [this, &benchFunction]<ARRAYS>(typename ARRAYS::value_type ... vals) {
            return benchFunction(clkArr, fh, mul, std::forward<typename ARRAYS::value_type>(vals)...);
        };


        fileUtils::createDirIfNotPresent(fh.getParentPath() / ddpath);

        for (auto &tuple: cart) {
            auto itername = benchmarkName + std::apply(benchNameGenerator, tuple);
            std::apply(lambda, tuple);

            snapshotTimers(clkArr, tupleToString(tuple, "\t", "", "") + "\t", "\n", mul);

            innerCounter++;
            clkdc.advance(clkArr);
            clkArr.reset();
        }
        clkArr = clkdc;
    }
}

