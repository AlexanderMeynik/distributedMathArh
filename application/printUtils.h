//
// Created by Lenovo on 28.03.2024.
//

#ifndef DIPLOM_PRINTUTILS_H
#define DIPLOM_PRINTUTILS_H
#include <iostream>
#include <ostream>
#include <iomanip>
class IosStatePreserve
{
public:
    IosStatePreserve(std::ostream&out):out_(out)
    {
        flags_=out.flags();
        //out_=out;

    }
    ~IosStatePreserve()
    {
        out_.setf(flags_);
    }

private:
    std::ostream &out_;
    std::ios_base::fmtflags flags_;
};
template<typename T>
struct scientificNumberType
{
    explicit scientificNumberType(T number, int decimalPlaces) : number(number), decimalPlaces(decimalPlaces) {}

    T number;
    int decimalPlaces;
};

template<typename T>
scientificNumberType<T> scientificNumber(T t, int decimalPlaces)
{
    return scientificNumberType<T>(t, decimalPlaces);
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const scientificNumberType<T>& n)
{
    double numberDouble = n.number;
    char c=(n.number>0)?'+':'-';

    int eToThe = 0;
    for(; numberDouble > 9; ++eToThe)
    {
        numberDouble /= 10;
    }

    // memorize old state
    std::ios oldState(nullptr);
    oldState.copyfmt(os);

    os << std::fixed << std::setprecision(n.decimalPlaces) << numberDouble << "e"<<c << eToThe;

    // restore state
    os.copyfmt(oldState);

    return os;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const scientificNumberType<T>&& n)
{
    double numberDouble = n.number;
    char c=(n.number>0)?'+':'-';

    int eToThe = 0;
    for(; numberDouble > 9; ++eToThe)
    {
        numberDouble /= 10;
    }

    // memorize old state
    std::ios oldState(nullptr);
    oldState.copyfmt(os);

    os << std::fixed << std::setprecision(n.decimalPlaces) << numberDouble << "e"<<c << eToThe;

    // restore state
    os.copyfmt(oldState);

    return os;
}


#endif //DIPLOM_PRINTUTILS_H
