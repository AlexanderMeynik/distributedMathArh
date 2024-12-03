#include "fileHandler.h"
#include "benchmarkHandler.h"
#include <iostream>
#include <utility>
#include <tuple>
using fileUtils::fileHandler;
std::filesystem::path dir("becnhmarkData");
using  benchUtils::benchmarkHandler;
void func(int a,char b,uint c)
{
    std::cout<<a<<'\n'<<b<<'\n'<<c<<'\n';
}

/*template<typename ... Args>
constexpr auto getIndForTuple(std::tuple<Args...>&t)
{
    return std::index_sequence_for<Args...>{};
}*/

/*
template<typename ... Args,typename calee_>
constexpr auto expand(std::tuple<Args...>&t,const calee_&call)
{
    constexpr auto seq=std::index_sequence_for<Args...>{};
    return call(get<seq>(call)...);
}*/
constexpr auto chararr=std::array<char,3>{'a','b','c'};
constexpr auto intarr=std::array<int,4>{1,2,3,4};
int main()
{
    std::tuple<int,char,uint> tuple{0,69,2};

    constexpr auto seq= std::make_index_sequence<10>{};
    std::apply(func,tuple);
    /*constexpr auto indFor=getIndForTuple(tuple);
    func(*//*indFor...*//*get<indFor>(tuple)...)*/
    /*constexpr auto sz=seq.size();
    constexpr auto sz2=indFor.size();*/
    benchmarkHandler bh("bench1",{"benhc1"});
    bh.runThing<decltype(chararr), decltype(intarr)>(chararr,intarr,[](char a,int b){return a+std::to_string(b);});

    /*fileHandler fh(dir);
    for (size_t i = 0; i < 10; ++i) {
        std::string name="ostream_"+std::to_string(i);
        fh.upsert(name,std::ios_base::app);
        *//*fh.open(name,std::ios_base::app);*//*
        for (size_t j = 0; j < 10; ++j) {
            fh.output<false>(name,name);
            fh.output<false>(name,j);
            fh.output<false>(name,'\n');

        }
    }
    fh.closeFiles();*/


}
//
//#include "parallelUtils/commonDeclarations.h"
//#include <concepts>
//#include <fstream>
//#include <utility>
//#include <vector>
//#include <iostream>
//#include <unordered_set>
//#include <unordered_map>
//#include <filesystem>
//#include <memory>
//#include <cstring>
//#include "fileHandler.h"
//
///**
// * @brief Struct for std::array checks
// */
//template<typename>
//struct is_std_array : std::false_type {
//};
///**
// * @brief Struct for std::array checks
// */
//template<typename T, std::size_t N>
//struct is_std_array<std::array<T, N>> : std::true_type {
//};
///**
// * @brief Struct for std::array checks
// */
//template<typename T, std::size_t N>
//struct is_std_array<const std::array<T, N>> : std::true_type {
//};
//
///**
// *  concept std_array
// */
//template<typename T>
//concept std_array = is_std_array<T>::value;
//
//template<typename...ARRAYS>
//constexpr auto cartesian_product (ARRAYS...arrays)
//{
//    using type = std::tuple<typename ARRAYS::value_type...>;
//
//    constexpr std::size_t N = (1 * ... * arrays.size());
//
//    std::array<std::size_t,sizeof...(arrays)> dims { arrays.size()... };
//    for (std::size_t i=1; i<dims.size(); ++i)  { dims[i] *= dims[i-1]; }
//
//    return [&] ()
//    {
//        std::array<type, N> result;
//
//        for (std::size_t i=0; i<result.size(); ++i)
//        {
//            [&]<std::size_t... Is>(std::index_sequence<Is...>)
//            {
//                auto idx = std::make_tuple ( ( (i*std::get<Is>(dims)) / N) % arrays.size() ...);
//                result[i] = std::make_tuple (arrays[std::get<Is>(idx)]...);
//
//            }(std::make_index_sequence<sizeof...(arrays)>{});
//        }
//
//        return result;
//    }();
//}
//
//
//
//
//using namespace commonDeclarations;
//fs::path pparent="../..";
//std::string parent_path = pparent/"testDirectories/";
//std::string new_dir_prefix = pparent/"testDirectoriesRes/";
//std::vector<fs::path> from_dirs = {"images",
//                                   "res",
//                                   "res"};
//std::vector<fs::path> to_dirs(from_dirs.size(), "");
//constexpr std::array<int,6> indx= {0,1,2,3,4,5};
//constexpr std::array<int,10> multipliers={2,4,8,16,64,256,512,1024,2048,4096};
//std::filesystem::path dir("becnhmarkData");
//
//std::unordered_map<std::string,std::shared_ptr<std::ofstream>>timerOstreams;
//
//bool upsert(const std::string& filename,std::unordered_map<std::string,std::shared_ptr<std::ofstream>>&map)
//{
//    if(!map.contains(filename))
//    {
//        map[filename]=std::make_shared<std::ofstream>();
//        map[filename]->open(filename);
//    }
//    return map[filename]->is_open();
//}
//#include <functional>
///*template<typename _Ret,typename ... _Arguments>
//struct myExecutor
//{
//    _Ret operator()(_Arguments...arguments)
//    {
//        return exe(std::forward<_Arguments>(arguments)...);
//    }
//
//    std::function<_Ret(_Arguments& ...)> exe;
//    std::tuple<_Arguments...> args;
//};*/
//#include <optional>
//template<typename ... Arguments>
//class bencmark
//{
//public:
//    using type = std::tuple<typename Arguments::value_type...>;
//    bencmark<Arguments>():name(), localClk({}),
//    itercallback(/*[](Arguments...arguments)
//    {return "";}*/std::nullopt),benchmarkIteration(std::nullopt)
//    {}
//    bencmark<Arguments>(std::string name,const std::function<std::string(type&&)>&
//    func,const std::function<int(type&&)>&
//    func2):name(std::move(name)),localClk({}),itercallback(func),benchmarkIteration(func2){
//
//    };
//    void prepareAndRun(Arguments...arguments)
//    {
//
//        auto a=cartesian_product(arguments...);
//        if(!itercallback.has_value()||!benchmarkIteration.has_value())
//        {
//            throw std::logic_error("Callbacks must be initialized");
//        }
//        for (auto&elem:a) {
//            auto string=name+itercallback(arguments...);
//            benchmarkIteration(arguments...);
//        }
//    }
//
//    /*template<typename Ret,typename ... Arguments2>
//    std::vector<Ret> run(std::function<>)
//    {
//
//    }*/
//
//
//    std::string name;
//    clockType localClk;
//    std::optional<std::function<std::string(type&&)>> itercallback;
//    std::optional<std::function<int(type&&)>> benchmarkIteration;
//};
//
//
//std::vector<std::string>ss
//        {
//                dir/"bench_timers.txt",
//        };
//
//
//int ii=0;
//
//
//void tablePrint(const std::string &name, const std::string &value, int multip, size_t hashNum);
//
//int main(int argc, char *argv[]) {
//
////todo depenedecy of computation time from N
////todo integration accuracy/speed depending on Ndots and quadrature
////todo gbench
//    for (int i = 0; i < from_dirs.size(); i++) {
//        to_dirs[i] = getNormalAbs((new_dir_prefix / from_dirs[i]));
//        from_dirs[i] = getNormalAbs(parent_path / from_dirs[i]);
//    }
//
//    if(!fs::exists(dir))
//    {
//        fs::create_directories(dir);
//    }
//    std::ofstream timers(ss[0]);
//    timers<<"\n";
//
//
//
//
//    constexpr auto entries2 = cartesian_product(indx, multipliers);
//
//    for (auto& elem:entries2) {
//        auto [hashNum,multip]=elem;
//        /*auto hashNum=elem[0];
//        auto multip=elem[1];*/
//
//        gClk.tik();
//        auto segment_size=multip;
//
//
//
//
//
//
//        timers << dbName << '\n' << gClk << "\n";
//
//        for (auto &val:gClk) {
//            std::string name=dir/(val.first[3] + "_" + val.first[1]);
//            tablePrint(name,std::to_string((int)val.second.time),multip,hashNum);
//        }
//
//        gClk.reset();
//
//    }
//
//    for (auto &ptr:timerOstreams) {
//        ptr.second->close();
//    }
//
//    timers.close();
//
//}
//
//void tablePrint(const std::string &name, const std::string &value, int multip, size_t hashNum) {
//    auto res=upsert(name,timerOstreams);
//    auto osPtr=timerOstreams[name];
//
//
//    if(multip==multipliers[0])
//    {
//        if(hashNum==0) {
//            const char * aa="Hash\\Segment size";
//            osPtr->write(aa, strlen(aa));
//            osPtr->write(lineTab,1);
//            for (auto &i2: multipliers) {
//                osPtr->operator<<(i2);
//                osPtr->write(lineTab,1);
//            }
//        }
//        osPtr->write(lineEnd,1);
//        osPtr->write(hashFunctionName[hashNum], strlen(hashFunctionName[hashNum]));
//        osPtr->write(lineTab,1);
//    }
//    osPtr->write(value.c_str(),value.size());
//    osPtr->write(lineTab,1);
//}
