#ifndef DIPLOM_PRINTUTILS_H
#define DIPLOM_PRINTUTILS_H

#include <ostream>
#include <iomanip>
#include <limits>//todo used only once for precision defaults

class IosStatePreserve {
public:
    explicit IosStatePreserve(std::ostream &out) : out_(out) {
        flags_ = out.flags();

    }

    ~IosStatePreserve() {
        out_.flags(flags_);
    }

    std::ios_base::fmtflags getFlags() const {
        return flags_;
    }

private:
    std::ios_base::fmtflags flags_;
    std::ostream &out_;
};

//todo redo this completely
//do we actually need this if we can map everything to Eigen types and format them https://eigen.tuxfamily.org/dox/structEigen_1_1IOFormat.html
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



#endif //DIPLOM_PRINTUTILS_H
