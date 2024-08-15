#include <iostream>
#include "nested_template.h"
#include <type_traits>
int main(int argc, char** argv)
{
    nested<int> aa;
    auto v=nested<int>::solution<std::array<std::vector<int>, 2>>{};
    v.solve();
    std::cout<<typeid(v.sol_).name()<<'\n';
}