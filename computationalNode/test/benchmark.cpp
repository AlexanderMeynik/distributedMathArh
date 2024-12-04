#include "fileHandler.h"
#include "benchmarkHandler.h"
#include "common/Generator.h"
#include <iostream>
#include <utility>
#include <tuple>
#include "computationalLib/math_core/Dipoles.h"

using fileUtils::fileHandler;
std::filesystem::path dir("becnhmarkData");
using  benchUtils::benchmarkHandler;
void func(int a,char b,uint c)
{
    std::cout<<a<<'\n'<<b<<'\n'<<c<<'\n';
}


#include "eigen3/Eigen/Dense"
//todo dipoles solve(create 2 variants)
//todo dipoles function create/generate + 4 confs and N
//todo integration methods metrics
/*std::valarray<FloatType> solve()*/
constexpr auto chararr=std::array<char,3>{'a','b','c'};
constexpr auto intarr=std::array<int,4>{1,2,3,4};
int main()
{
    auto confNum=10;
    auto N=10;

    FloatType arange=1e-6;
    auto sig=arange * sqrt(2);
    std::vector<std::valarray<FloatType>> coordinates;
    coordinates.resize(confNum);
    for (auto&it:coordinates) {
        it=generators::normal<std::valarray>(N,0.0,arange);
    }

    dipoles::Dipoles dipoles1;

    for (size_t i = 0; i < confNum; ++i) {
        dipoles1.setNewCoordinates(coordinates[i]);
        dipoles1.solve();
    }


    auto a=generators::normal<std::valarray>(2ul,0,1);
    std::function<std::string(char, int,double)>nameGenerator = [](char a, int b,double c) { return a + std::to_string(b)+"_"+ std::to_string(c); };

    auto ff=[&nameGenerator](benchUtils::clockType&clk,fileUtils::fileHandler&handler,char a,int b,double c){
        using namespace std::chrono_literals;
        auto inner_name="inner"+nameGenerator(a,b,c);
        handler.upsert(inner_name);
        auto loc=clk.tikLoc();

        std::this_thread::sleep_for(100ms);

        clk.tak();
        handler.output(inner_name,clk[loc].time);
        return ;
    };
    benchmarkHandler bh("bench1",{"benhc1"});
    bh.runThing(nameGenerator, ff, std::array{'a','b','c'}, intarr,std::array{1.0,2.0});



}

////todo depenedecy of computation time from N
////todo integration accuracy/speed depending on Ndots and quadrature
////todo gbench
