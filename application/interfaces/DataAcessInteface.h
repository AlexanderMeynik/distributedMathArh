//
// Created by Lenovo on 16.07.2024.
//

#ifndef DIPLOM_DATAACESSINTEFACE_H
#define DIPLOM_DATAACESSINTEFACE_H

#include <unordered_map>
#include <string>
#include <vector>
#include <variant>
namespace core_intrefaces {

    using optinal_val=std::variant<int,double,std::string,std::vector<double>>;

    template<class... Ts>
    struct overload : Ts... {
        using Ts::operator()...;
    };

    template<class... Ts>
    overload(Ts...) -> overload<Ts...>;

    class DataAcessInteface {
    public:
        DataAcessInteface():data_()
        {
            //optinal_val a;
            //holds_alternative(a)
        }
        std::vector<std::vector<double>> &getdat(std::string&&key)
        {
            return data_[std::forward<decltype(key)>(key)];
        }
        optinal_val& getProperty(std::string&key)
        {
            return properties_[key];
        }


    private:
        std::unordered_map<std::string, std::vector<std::vector<double>>> data_;

        std::unordered_map<std::string,optinal_val> properties_;
    };
}

#endif //DIPLOM_DATAACESSINTEFACE_H
