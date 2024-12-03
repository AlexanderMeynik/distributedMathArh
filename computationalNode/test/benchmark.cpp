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
    std::function<std::string(char, int)>nameGenerator = [](char a, int b) { return a + std::to_string(b); };

    auto ff=[&nameGenerator](benchUtils::clockType&clk,fileUtils::fileHandler&handler,char a,int b){
        using namespace std::chrono_literals;
        auto inner_name="inner"+nameGenerator(a,b);
        handler.upsert(inner_name);
        auto loc=clk.tikLoc();

        std::this_thread::sleep_for(100ms);

        std::cout<<"inside\n";
        clk.tak();
        handler.output(inner_name,clk[loc].time);
        return ;
    };
    benchmarkHandler bh("bench1",{"benhc1"});
    bh.runThing(nameGenerator, ff, chararr, intarr);



}

////todo depenedecy of computation time from N
////todo integration accuracy/speed depending on Ndots and quadrature
////todo gbench
