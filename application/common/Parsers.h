
#ifndef DIPLOM_PARSERS_H
#define DIPLOM_PARSERS_H

#include <istream>
#include <eigen3/Eigen/Dense>
#include <filesystem>
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <ostream>
#include <fstream>
#include <iomanip>
#include <filesystem>


#include <cassert>

char parseChar(std::istream &in);

int getConfSize(std::string &filename);

template<class T, template<typename> typename Container>
std::vector<Container<T>> parseConf2(std::string &filename) {
    std::ifstream in(filename);
    char c = parseChar(in);
    assert(c == 'C');
    int Nconf;
    in >> Nconf;
    std::vector<Container<T>> avec(Nconf);
    std::vector<int> Nvec(Nconf);
    for (int j = 0; j < Nconf; ++j) {

        int N;
        in >> N;
        Nvec[j] = N;
        avec[j].resize(2 * N);
        //avec[j] = Container<T>(2*N,0);

        if (in.peek() == 'l') {
            in.get();
            T lim[4];
            in >> lim[0] >> lim[1] >> lim[2] >> lim[3];
            T step[2] = {(lim[1] - lim[0]) / (N - 1), (lim[3] - lim[2]) / (N - 1)};
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
            T lim[4];
            in >> lim[0] >> lim[1] >> lim[2] >> lim[3];
            T start[2] = {lim[0], lim[2]};
            T step[2] = {(lim[1] - lim[0]) / (N1 - 1), (lim[3] - lim[2]) / (N2 - 1)};
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

template<class T>
std::vector<std::array<std::vector<T>, 2>> parseConf(std::string &filename) {
    std::ifstream in(filename);
    char c = parseChar(in);
    assert(c == 'C');
    int Nconf;
    in >> Nconf;
    std::vector<std::array<std::vector<T>, 2>> avec(Nconf);
    std::vector<int> Nvec(Nconf);
    for (int j = 0; j < Nconf; ++j) {

        int N;
        in >> N;
        Nvec[j] = N;
        avec[j] = std::array<std::vector<T>, 2>({std::vector<T>(N, 0), std::vector<T>(N, 0)});

        if (in.peek() == 'l') {
            in.get();
            T lim[4];
            in >> lim[0] >> lim[1] >> lim[2] >> lim[3];
            T step[2] = {(lim[1] - lim[0]) / (N - 1), (lim[3] - lim[2]) / (N - 1)};
            for (int i = 0; i < N; ++i) {

                avec[j][0][i] = lim[0];
                avec[j][1][i] = lim[2];
                lim[0] += step[0];
                lim[2] += step[1];
            }
        } else if (in.peek() == 'g') {
            int N1 = 0;
            in.get();
            in >> N1;
            int N2 = N / N1;
            T lim[4];
            in >> lim[0] >> lim[1] >> lim[2] >> lim[3];
            T start[2] = {lim[0], lim[2]};
            T step[2] = {(lim[1] - lim[0]) / (N1 - 1), (lim[3] - lim[2]) / (N2 - 1)};
            for (int i = 0; i < N1; ++i) {
                for (int k = 0; k < N2; ++k) {
                    avec[j][0][i * N2 + k] = lim[0];
                    avec[j][1][i * N2 + k] = lim[2];
                    lim[2] += step[1];
                }
                lim[2] = start[1];
                lim[0] += step[0];
            }
        } else {
            for (int i = 0; i < N; ++i) {
                in >> avec[j][0][i];
            }
            c = parseChar(in);
            if (c != '\n') {
                if (c == '\r' && in.peek() != '\n') {
                    errno = -1;
                    std::cout << "Errno=" << errno;
                }
            }

            for (int i = 0; i < N; ++i) {
                in >> avec[j][1][i];
            }
        }
    }
    return avec;
}


template<class T>
void printCoordinates(std::ostream &out, Eigen::Vector<T, Eigen::Dynamic> &xi) {
    out << "Координаты диполей\n";
    auto N = xi.size() / 2;
    for (int i = 0; i < N; ++i) {
        out << xi[i] << '\t' << xi[i + N] << "\n";
    }
}

/*
template<class T>
void printSolution(std::ostream &out, Eigen::IOFormat &format,std::array<Eigen::Vector<T, Eigen::Dynamic>, 2>& solution_);
*/

template<class T>
void printSolutionFormat1(std::ostream &out, std::array<Eigen::Vector<T, Eigen::Dynamic>, 2> &solution_);

template<class T>
void plotCoordinates(std::string name, T ar, std::array<std::vector<T>, 2> &xi);

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


#endif //DIPLOM_PARSERS_H
