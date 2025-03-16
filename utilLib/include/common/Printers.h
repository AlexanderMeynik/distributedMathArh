#pragma once

#ifndef DATA_DEDUPLICATION_SERVICE_PRINTERS_H
#define DATA_DEDUPLICATION_SERVICE_PRINTERS_H


#include <limits>


#include <json/json.h>
#include <valarray>
#include <concepts>
/// printUtils namespace
namespace printUtils {
    template<typename Iterator>
    //todo minimal req is bidirectional iterator
    //todo move near printers in utility(include only json)
    Json::Value continuousToJson(Iterator s, Iterator e) {
        Json::Value res;
        int i = 0;
        for (auto it = s; it != e; it++) {
            res["data"][i] = *it;
            i++;
        }
        res["size"] = i;
        return res;
    }

//todo move to  parsers
    template<typename Struct>
    concept continuousStruct=requires(Struct s)
    {
        requires std::random_access_iterator<decltype(std::begin(s))>;
    };

    template<continuousStruct Struct>
    Struct parseCont(Json::Value &val) {
        int size = val["size"].asUInt();

        std::valarray<double> res(size);

        for (int i = 0; i < size; ++i) {
            res[i] = val["data"][i].asDouble();
        }
        return res;
    }


}


#endif //DATA_DEDUPLICATION_SERVICE_PRINTERS_H
