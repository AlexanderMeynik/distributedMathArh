
#ifndef DIPLOM_BENCHMARKHANDLER_H
#define DIPLOM_BENCHMARKHANDLER_H
#include <functional>
#include <optional>
#include <concepts>
#include <tuple>
#include "fileHandler.h"
#include "parallelUtils/commonDeclarations.h"

namespace benchUtils {
    namespace fu= fileUtils;
    namespace tu=timing;
    using clockType= tu::chronoClockTemplate<std::milli> ;

    template<typename...ARRAYS>
    constexpr auto cartesian_product(ARRAYS...arrays) {
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



    class benchmarkHandler {
        public:
        //todo check whether benchmark name can be used as a path
        explicit benchmarkHandler(std::string_view name,
                         std::optional<std::string>path=std::nullopt);


        static inline std::filesystem::path ddpath="timers";
        template<typename...ARRAYS>
        constexpr auto runThing(const std::function<std::string(typename ARRAYS::value_type ...)>&benchNameGenerator,
                                const std::function<void(clockType &,fu::fileHandler&, typename ARRAYS::value_type ...)>&benchFunction,
                                ARRAYS...arrays);
        void snapshotTimers(clockType&clk);
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
        clockType clkArr;
    };



    template<typename... ARRAYS>
    constexpr auto
    benchmarkHandler::runThing(const std::function<std::string(typename ARRAYS::value_type ...)>&benchNameGenerator,
                               const std::function<void(clockType &,fu::fileHandler&, typename ARRAYS::value_type ...)>&benchFunction,
                               ARRAYS...arrays) {
        clockType clkdc= this->clkArr;

        auto cart= cartesian_product(std::forward<ARRAYS>(arrays)...);
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
            snapshotTimers(clkArr);

            innerCounter++;
            clkdc.advance(clkArr);
            clkArr.reset();
        }
        clkArr=clkdc;//todo think of the way to have wo clock arrays
    }
}

#endif //DIPLOM_BENCHMARKHANDLER_H
