#pragma once

#include <unordered_map>
#include <string>
#include <vector>
#include <variant>

namespace core_intrefaces {

    using optinal_val = std::variant<bool, int, double, std::string, std::vector<double>>;

    template<class... Ts>
    struct overload : Ts ... {
        using Ts::operator()...;
    };

    template<class... Ts>
    overload(Ts...) -> overload<Ts...>;

    /**
     * Data acess interface todo переосмыслить и переделать
     */
    class DataAcessInteface {
    public:
        DataAcessInteface() : data_{}, properties_{} {
            //optinal_val a;
            //holds_alternative(a)
        }

        virtual std::vector<std::vector<double>> &getdat(std::string &key) {
            return data_[key];
        }

        virtual std::vector<std::vector<double>> &getdat(std::string &&key) {
            return data_[key];
        }

        virtual optinal_val &getProperty(std::string &key) {
            return properties_[key];
        }

        virtual optinal_val &getProperty(std::string &&key) {
            return properties_[key];
        }

        virtual bool isPresent(std::string &key) {
            return data_.contains(key);
        }


        virtual bool isPPresent(std::string &key) {
            return properties_.contains(key);
        }

    protected:
        std::unordered_map<std::string, std::vector<std::vector<double>>> data_;

        std::unordered_map<std::string, optinal_val> properties_;
    };
}

