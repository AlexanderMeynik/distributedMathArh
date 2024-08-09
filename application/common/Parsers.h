//
// Created by Lenovo on 09.08.2024.
//

#ifndef DIPLOM_PARSERS_H
#define DIPLOM_PARSERS_H
#include <istream>
#include "../math_core/MeshProcessor.h"
#include <eigen3/Eigen/Dense>




template<class T>
void printCoordinates(std::ostream &out,Eigen::Vector<T , Eigen::Dynamic> &xi) {
    out << "Координаты диполей\n";
    auto N=xi.size()/2;
    for (int i = 0; i <N; ++i) {
        out << xi[i] << '\t' << xi[i+N] << "\n";
    }
}

/*
template<class T>
void printSolution(std::ostream &out, Eigen::IOFormat &format,std::array<Eigen::Vector<T, Eigen::Dynamic>, 2>& solution_);
*/

template <class T>
void printSolutionFormat1(std::ostream &out,std::array<Eigen::Vector<T, Eigen::Dynamic>, 2>& solution_);

template <class T>
void plotCoordinates(std::string name, T ar,std::array<std::vector<T>, 2> &xi);

template <typename T>
std::istream& operator>>(std::istream& in, std::array<std::vector<T>, 2>& xi) {
    size_t size;
    in>>size;
    for (int i = 0; i < 2; ++i) {
        xi[i].resize(size);
        for (int j = 0; j < size; ++j) {
            in >> xi[i][j];
        }
    }
    return in;
}

template <typename T>
std::istream& operator>>(std::istream& in, std::array<Eigen::Vector<T, Eigen::Dynamic>, 2>& sol) {
size_t size;
in>>size;
for (int i = 0; i < 2; ++i) {
sol[i].resize(size);
for (int j = 0; j < size; ++j) {
in >> sol[i][j];
}
}
return in;
}


template<typename Container>
class Parser
{
public:
    Parser() : vals_() {}

    Container vals_;
};

template<typename T>
class Parser<Eigen::Vector<T,-1>>
{
public:
Parser() : size_(0), vals_() {}

Parser(int size) : size_(size), vals_(size) {}

friend std::istream& operator>>(std::istream& in, Parser& pp)
{
    in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::string dummy;
    std::getline(in, dummy);

    pp.vals_.resize(4*pp.size_);
    for (int i = 0; i < 4*pp.size_; ++i) {
        T val;
        in>>val;
        pp.vals_(i)=val;
    }
    return in;
}

Eigen::Vector<T,-1> vals_;
int size_;
};


template<typename T>
class Parser<Eigen::Matrix<T,Eigen::Dynamic,Eigen::Dynamic>>
{
public:
Parser() : size_(0), vals_() {}

Parser(int size) : size_(size), vals_() {}

friend std::istream& operator>>(std::istream& in, Parser& pp)
{
    in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    pp.vals_.resize(4*pp.size_,4*pp.size_);



    for(int i = 0; i <4*pp.size_; ++i) {
        for(int j = 0; j < 4*pp.size_; ++j) {
            in >> pp.vals_(i, j);
        }
    }
    return in;
}

Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> vals_;
int size_;
};


#endif //DIPLOM_PARSERS_H
