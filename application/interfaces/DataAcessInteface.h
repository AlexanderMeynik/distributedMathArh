//
// Created by Lenovo on 16.07.2024.
//

#ifndef DIPLOM_DATAACESSINTEFACE_H
#define DIPLOM_DATAACESSINTEFACE_H

#include <unordered_map>
#include <string>
#include <vector>
namespace core_intrefaces {
    class DataAcessInteface {
    public:
        DataAcessInteface():data_()
        {}
        std::vector<std::vector<double>> &getdat(std::string&key)
        {
            return data_[key];
        }

    private:
        std::unordered_map<std::string, std::vector<std::vector<double>>> data_;
    };
}

#endif //DIPLOM_DATAACESSINTEFACE_H
