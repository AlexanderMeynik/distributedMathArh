#pragma once
#include "common/commonTypes.h"

namespace shared {
    static shared::BenchResVec inline
      ns = {1ul, 2ul, 4ul, 5ul, 8ul, 10ul, 20ul, 40ul, 50ul, 100ul, 200ul, 400ul, 500ul};//, 800ul, 1000ul ,2000ul};
    static shared::BenchResVec inline iter_count =
      {1000ul, 1000ul, 1000ul, 1000ul, 1000ul, 1000ul, 100ul, 100ul, 100ul, 25ul, 25ul, 5ul, 5ul};//, 25ul, 10ul, 2ul};

    size_t inline NIndex(const shared::BenchResultType & result_type)
    {
      return std::find(std::begin(ns),std::end(ns),result_type)-std::begin(ns);
    }
}