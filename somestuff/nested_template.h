
#ifndef NESTED_TEMPLATE
#define NESTED_TEMPLATE
#include<array>
#include<vector>
#include <array>
#include <vector>
#include <iostream>
#include <eigen3/Eigen/Dense> // Assuming you have Eigen library installed

template<typename T>
class nested
{
public:
    T val = T{};

    // Primary template declaration
    template<typename CONT>
    struct solution//todo порпобовать сделать це при помощи "https://www.reddit.com/r/cpp/comments/aoidsi/what_is_the_solution_for_same_function_parameters/"
    {
        using soltype = CONT;
        void solve();
        soltype sol_;
    };
};
/*template<typename T>
template<typename CONT>
struct nested<T>::solution
{
    void solve()
    {
        sol_[0] = std::vector<T>(1, 3);
        sol_[1] = std::vector<T>(1, 4);
    }

    std::array< T,2> sol_;
};*/
// Specialization for std::array
template<typename T>
//template<typename InnerType, std::size_t N>
struct nested<T>::solution
//struct nested<T>::solution<std::array<std::vector<T>,2>>
{
    void solve()
    {
        sol_[0] = std::vector<T>(1, 3);
        sol_[1] = std::vector<T>(1, 4);
    }

    std::array< T,2> sol_;
};

template<typename T>
//template<typename InnerType, std::size_t N>
struct nested<T>::solution
{
    void solve()
    {
       sol_=T()+2;
    }

    std::array< T,2> sol_;
};

/*// Specialization for std::vector
template<typename T>
template<typename InnerType>
struct nested<T>::solution<std::vector, InnerType>
{
    void solve()
    {
        sol_.push_back(val);
    }

    std::vector<InnerType> sol_;
};

// Specialization for Eigen::Matrix
template<typename T>
template<int Rows, int Cols>
struct nested<T>::solution<Eigen::Matrix, T, std::integral_constant<int, Rows>, std::integral_constant<int, Cols>>
{
    void solve()
    {
        sol_.setConstant(Rows, Cols, val);
    }

    Eigen::Matrix<T, Rows, Cols> sol_;
};

*/

#endif