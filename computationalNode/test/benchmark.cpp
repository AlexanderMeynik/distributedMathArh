#include "fileHandler.h"
#include "benchmarkHandler.h"
#include "common/Generator.h"
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
#include "eigen3/Eigen/Dense"
constexpr auto chararr=std::array<char,3>{'a','b','c'};
constexpr auto intarr=std::array<int,4>{1,2,3,4};
template<typename T>
using dynVec=Eigen::Vector<T,-1>;
int main()
{

    dynVec<FloatType> aa;

    auto a=generators::normal<dynVec>(2ul,0,1);
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
    bh.runThing(nameGenerator, ff, std::array{'a','b','c'}, intarr);



}

////todo depenedecy of computation time from N
////todo integration accuracy/speed depending on Ndots and quadrature
////todo gbench
