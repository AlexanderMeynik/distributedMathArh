
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
                         std::optional<std::string>path=std::nullopt):
                         benchmarkName(name),
                         fh(path.has_value()?fu::getNormalAbs(path.value()):""),
                            clkArr()
        {
        }

        template<typename...ARRAYS>
        using tupleType=std::tuple<typename ARRAYS::value_type...>;

        template<typename...ARRAYS>
        constexpr auto runThing(ARRAYS...arrays,const std::function<std::string(typename ARRAYS::value_type ...)>&ff){
            auto cart= cartesian_product(std::forward<ARRAYS>(arrays)...);
            auto size=std::tuple_size<typename decltype(cart)::value_type>{};
            size_t innerCounter=0;
            for (auto&tuple:cart) {

                auto itername=benchmarkName+std::apply(ff,tuple);
                std::cout<<itername<<'\n';
                innerCounter++;
               /* std::cout<<ff(benchmarkName,*//*tuple.*//*)*/
            }
        }


    private:
        fu::fileHandler fh;
        std::string benchmarkName;
        tu::chronoClockTemplate<std::milli> clkArr;
    };
}

#endif //DIPLOM_BENCHMARKHANDLER_H
