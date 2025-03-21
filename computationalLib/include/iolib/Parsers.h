#pragma once
#ifndef DIPLOM_PARSERS_H
#define DIPLOM_PARSERS_H

#include <iostream>

#include <algorithm>
#include <numeric>
#include <fstream>
#include <iomanip>


#include "plotUtils/MeshCreator.h"
#include "common/myConcepts.h"

char parseChar(std::istream &in);

int getConfSize(std::string &filename);

template<typename Container>
requires commonDeclarations::HasBracketsNested<Container>
Container parseDipoleCoordinates(const std::string &filename) {
    using val=typename Container::value_type::value_type;
    std::ifstream in(filename);
    char c = parseChar(in);
    if(c!='C')
    {
        throw std::invalid_argument("Missing letter \'C\' at start of configuration file");
    }

    int Nconf;
    in >> Nconf;
    Container avec(Nconf);
    std::vector<int> Nvec(Nconf);
    for (int j = 0; j < Nconf; ++j) {

        int N;
        in >> N;
        Nvec[j] = N;
        avec[j].resize(2 * N);


        if (in.peek() == 'l') {
            in.get();
            val lim[4];
            in >> lim[0] >> lim[1] >> lim[2] >> lim[3];
            val step[2] = {(lim[1] - lim[0]) / (N - 1), (lim[3] - lim[2]) / (N - 1)};
            for (int i = 0; i < N; ++i) {

                avec[j][i] = lim[0];
                avec[j][i + N] = lim[2];
                lim[0] += step[0];
                lim[2] += step[1];
            }
        } else if (in.peek() == 'g') {
            int N1 = 0;
            in.get();
            in >> N1;
            int N2 = N / N1;
            val lim[4];
            in >> lim[0] >> lim[1] >> lim[2] >> lim[3];
            val start[2] = {lim[0], lim[2]};
            val step[2] = {(lim[1] - lim[0]) / (N1 - 1), (lim[3] - lim[2]) / (N2 - 1)};
            for (int i = 0; i < N1; ++i) {
                for (int k = 0; k < N2; ++k) {
                    avec[j][i * N2 + k] = lim[0];
                    avec[j][i * N2 + k + N] = lim[2];
                    lim[2] += step[1];
                }
                lim[2] = start[1];
                lim[0] += step[0];
            }
        } else {
            for (int i = 0; i < N; ++i) {
                in >> avec[j][i];
            }
            c = parseChar(in);
            if (c != '\n') {
                if (c == '\r' && in.peek() != '\n') {
                    errno = -1;
                    std::cout << "Errno=" << errno;
                }
            }

            for (int i = 0; i < N; ++i) {
                in >> avec[j][i + N];
            }
        }
    }
    in.close();
    return avec;
}





template<typename T>
std::istream &operator>>(std::istream &in, std::array<std::vector<T>, 2> &xi) {
    size_t size;
    in >> size;
    for (int i = 0; i < 2; ++i) {
        xi[i].resize(size);
        for (int j = 0; j < size; ++j) {
            in >> xi[i][j];
        }
    }
    return in;
}

template<typename T>
std::istream &operator>>(std::istream &in, std::array<Eigen::Vector<T, Eigen::Dynamic>, 2> &sol) {
    size_t size;
    in >> size;
    for (int i = 0; i < 2; ++i) {
        sol[i].resize(size);
        for (int j = 0; j < size; ++j) {
            in >> sol[i][j];
        }
    }
    return in;
}


template<typename Container>
class Parser {
public:
    Parser() : vals_() {}

    Container vals_;
};


/**
 * @begin Eigen vector specialization for Parser
 * @tparam T
 */
template<typename T>
class Parser<Eigen::Vector<T, -1>> {
public:
    Parser() : size_(0), vals_() {}

    explicit Parser(int size) : size_(size), vals_() {}

    friend std::istream &operator>>(std::istream &in, Parser &pp) {
        in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::string dummy;
        std::getline(in, dummy);

        pp.vals_.resize(4 * pp.size_);
        for (int i = 0; i < 4 * pp.size_; ++i) {
            T val;
            in >> val;
            pp.vals_(i) = val;
        }
        return in;
    }

    Eigen::Vector<T, -1> vals_;
    int size_;
};

/**
 * @begin Standart vector specialization for Parser
 * @tparam T
 */
template<typename T>
class Parser<std::vector<T>> {
public:
    Parser() : size_(0), vals_() {}

    explicit Parser(int size) : size_(size), vals_() {}

    friend std::istream &operator>>(std::istream &in, Parser &pp) {
        in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::string dummy;
        std::getline(in, dummy);

        pp.vals_.resize(4 * pp.size_);
        for (int i = 0; i < 4 * pp.size_; ++i) {
            T val;
            in >> val;
            pp.vals_[i] = val;
        }
        return in;
    }

    std::vector<T> vals_;
    int size_;
};

/**
 * @begin Eigen vector specialization for Parser
 * @tparam T
 */
template<typename T>
class Parser<Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>> {
public:
    Parser() : size_(0), vals_() {}

    explicit Parser(int size) : size_(size), vals_() {}

    friend std::istream &operator>>(std::istream &in, Parser &pp) {
        in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        pp.vals_.resize(4 * pp.size_, 4 * pp.size_);


        for (int i = 0; i < 4 * pp.size_; ++i) {
            for (int j = 0; j < 4 * pp.size_; ++j) {
                in >> pp.vals_(i, j);
            }
        }
        return in;
    }

    Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> vals_;
    int size_;
};


using meshStorage::MeshCreator;
/**
 * @begin Mesh creator specialization for Parser
 * @tparam T
 */
template<>
class Parser<MeshCreator> {
public:
    Parser() : vals_() {}

    Parser(int size) : vals_() {}

    friend std::istream &operator>>(std::istream &in, Parser &pp) {
        using namespace meshStorage;
        std::string dummy;
        std::getline(in, dummy);
        std::getline(in, dummy);
        std::getline(in, dummy);

        auto n=pp.vals_.dimensions;

        pp.vals_.constructMeshes();

        co::meshStorageType m = pp.vals_.data[0];

        mdSpanType span=mdSpanType
                (&(m[0]),n[0],n[1]);

        for (int i = 0; i < span.extent(1); ++i) {
            FloatType temp = 0;
            in >> temp;
            for (int j = 0; j < span.extent(0); ++j) {
                FloatType val;
                in >> val;
                m[j*span.extent(1)+i]=val;

            }
        }


        pp.vals_.data[2]=m;
        return in;
    }

    MeshCreator vals_;

};



#endif //DIPLOM_PARSERS_H
