#include "benchmarkHandler.h"

namespace benchUtils {

    void benchmarkHandler::snapshotTimers(clk1&clk,const std::string&preprint,const std::string&delim) {
        for (auto &val:clk) {

            std::string name=ddpath/(val.first[3] + "_" + val.first[1]);
            fh.upsert(name);
            fh.output(name,preprint);
            fh.output(name,val.second.time);
            fh.output(name,delim);
        }
    }


    benchmarkHandler::benchmarkHandler(std::string_view name, std::optional<std::string> path):
    benchmarkName(name),
    fh(path.has_value()?fu::getNormalAbs(path.value()):""),
    clkArr()
    {

    }
}
