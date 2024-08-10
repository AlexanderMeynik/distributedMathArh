#ifndef DIPLOM_PRINTUTILS_H
#define DIPLOM_PRINTUTILS_H

#include <exception>
#include <tuple>
#include <cassert>
#include <vector>
#include <memory>
#include <iostream>
#include <ostream>
#include <iomanip>
#include <limits>

class IosStatePreserve {
public:
    explicit IosStatePreserve(std::ostream &out) : out_(out) {
        flags_ = out.flags();
        //out_=out;

    }

    ~IosStatePreserve() {
        out_ << std::setiosflags(flags_);
        //out_.set(flags_);
    }

    std::ios_base::fmtflags flags_;
private:
    std::ostream &out_;
};

template<typename T>
struct scientificNumberType {
    explicit scientificNumberType(T number, int decimalPlaces) : number(number), decimalPlaces(decimalPlaces) {}

    T number;
    int decimalPlaces;
};

template<typename T>
scientificNumberType<T> scientificNumber(T t, int decimalPlaces = std::numeric_limits<T>::max_digits10) {
    return scientificNumberType<T>(t, decimalPlaces);
}

template<typename T>
std::ostream &operator<<(std::ostream &os, const scientificNumberType<T> &n) {
    double numberDouble = n.number;
    char c = (n.number > 0) ? '+' : '-';

    int eToThe = 0;
    for (; numberDouble > 9; ++eToThe) {
        numberDouble /= 10;
    }

    // memorize old state
    std::ios oldState(nullptr);
    oldState.copyfmt(os);

    os << std::fixed << std::setprecision(n.decimalPlaces) << numberDouble << "e" << c << eToThe;

    // restore state
    os.copyfmt(oldState);

    return os;
}

template<typename T>
std::ostream &operator<<(std::ostream &os, const scientificNumberType<T> &&n) {
    double numberDouble = n.number;
    char c = (n.number > 0) ? '+' : '-';

    int eToThe = 0;
    for (; numberDouble > 9; ++eToThe) {
        numberDouble /= 10;
    }

    // memorize old state
    std::ios oldState(nullptr);
    oldState.copyfmt(os);

    os << std::fixed << std::setprecision(n.decimalPlaces) << numberDouble << "e" << c << eToThe;

    // restore state
    os.copyfmt(oldState);

    return os;
}


template<typename T, typename ... Args>
void printFirst(T elem, Args...args) {
    std::cout << (typeid(elem).name()) << '\n';
}

template<typename Arg, typename... Args>
void doPrint(std::ostream &out, Arg &&arg, Args &&... args) {
    out << typeid(arg).name() << '\t';
    ((out << '\t' << typeid(args).name()), ...);
}

template<typename TupleT, char del = '\t'>
void printTupleApply(std::ostream &out, const TupleT &tp) {
    std::apply
            (
                    [&out](const auto &first, const auto &... restArgs) {
                        auto printElem = [&out](const auto &x) {
                            if (!std::is_pointer<decltype(x)>::value) {
                                out << del << x;
                            }
                        };


                        out << '(';
                        if (!std::is_pointer<decltype(first)>::value) {
                            out << first;
                        }
                        (printElem(restArgs), ...);
                    }, tp
            );
    out << ')';
}

#endif //DIPLOM_PRINTUTILS_H
