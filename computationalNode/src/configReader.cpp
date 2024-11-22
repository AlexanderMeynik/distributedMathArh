#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include <eigen3/Eigen/Dense>

template<typename T>
auto write(std::ostream &os, T value) -> std::size_t {
    const auto pos = os.tellp();
    os.write(reinterpret_cast<const char *>(&value), sizeof(value));
    return static_cast<std::size_t>(os.tellp() - pos);
}

constexpr auto t_value = static_cast<std::uint8_t>('T');
constexpr auto f_value = static_cast<std::uint8_t>('F');

template<>
auto write(std::ostream &os, bool value) -> std::size_t {
    const auto pos = os.tellp();
    const auto tmp = (value) ? t_value : f_value;
    os.write(reinterpret_cast<const char *>(&tmp), sizeof(tmp));
    return static_cast<std::size_t>(os.tellp() - pos);
}

template<>
auto write(std::ostream &os, const std::string &value) -> std::size_t {
    const auto pos = os.tellp();
    const auto len = static_cast<std::uint32_t>(value.size());
    os.write(reinterpret_cast<const char *>(&len), sizeof(len));
    if (len > 0)
        os.write(value.data(), len);
    return static_cast<std::size_t>(os.tellp() - pos);
}

std::vector<std::vector<Eigen::Vector<long double, 2>>> parseConf(std::string &filename) {
    std::ifstream in(filename);
    char c = in.get();
    assert(c == 'C');
    int Nconf;
    in >> Nconf;
    std::vector<std::vector<Eigen::Vector<long double, 2>>> avec(Nconf);
    std::vector<int> Nvec(Nconf);
    for (int j = 0; j < Nconf; ++j) {
        int N;
        in >> N;
        Nvec[j] = N;
        avec[j] = std::vector<Eigen::Vector<long double, 2>>(N, {0, 0});
        for (int i = 0; i < N; ++i) {
            //in>>a[i][0];
            in >> avec[j][i][0];
        }
        c = in.get();
        if (c != '\n') {
            if (c == '\r' && in.peek() != '\n') {
                errno = -1;
                std::cout << "Errno=" << errno;
            }
        }

        for (int i = 0; i < N; ++i) {
            //in>>a[i][1];
            in >> avec[j][i][1];
        }
    }
    return avec;
}

int main(int argc, char *argv[]) {
    std::string filename = "config.txt";
    if (argc == 2) {
        filename = argv[1];
    }

    /*std::ifstream in(filename);
    char c=in.get();
    assert(c=='C');
    int Nconf;
    in>>Nconf;
    std::vector<std::vector<Eigen::Vector<long double,2>>>avec(Nconf);
    std::vector<int>Nvec(Nconf);
    for (int j = 0; j < Nconf; ++j) {
        int N;
        in>>N;
        Nvec[j]=N;
        avec[j]=std::vector<Eigen::Vector<long double,2>>(N,{0,0});
        for (int i = 0; i < N; ++i) {
            //in>>a[i][0];
            in>>avec[j][i][0];
        }
        auto c=in.get();
        if(c!='\n')
        {
            if(c=='\r'&&in.peek()!='\n') {
                errno = -1;
                std::cout << "Errno=" << errno;
            }
        }

        for (int i = 0; i < N; ++i) {
            //in>>a[i][1];
            in>>avec[j][i][1];
        }
    }*/
    auto avec = parseConf(filename);
    for (auto &elem: avec) {
        std::cout << elem.size() << "\n";
        for (auto &j: elem) {
            std::cout << j[0] << "\t" << j[1] << "\n";
        }
    }

    /*char  line[100];
    while(in.getline(line,100))
    {
        std::cout<<line<<"\n";
    }*/
}