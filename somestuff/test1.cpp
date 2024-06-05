#include <iostream>
#include <functional>
template <typename T>
constexpr T min(T a, T b)
{
    return a < b ? a : b;
}
template <typename T, typename... Args>
constexpr T min(T a, Args... args)
{
    return min(a, min(args...));
}

template <typename T, typename... Args>
T sum(T a, Args... args)
{
    if constexpr (sizeof...(args) == 0)
        return a;
    else
        return a + sum(args...);
}


template<typename, typename>
struct func_pair;
template<typename R1, typename... A1,
        typename R2, typename... A2>
struct func_pair<R1(A1...), R2(A2...)>
{
    std::function<R1(A1...)> f;
    std::function<R2(A2...)> g;
};
bool twice_as(int a, int b)
{
    return a >= b*2;
}
double sum_and_div(int a, int b, double c)
{
    return (a + b) / c;
}
int main()
{

    func_pair<bool(int, int), double(int, int, double)> funcs{
            twice_as, sum_and_div };

    funcs.f(42, 12);
    funcs.g(42, 12, 10.0);

}
