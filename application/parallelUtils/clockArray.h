//
// Created by Lenovo on 05.06.2024.
//

#ifndef DIPLOM_CLOCKARRAY_H
#define DIPLOM_CLOCKARRAY_H
#include <iostream>
#include <cstddef>
#include <array>
#include <functional>
template<typename ...Args>
struct functable{
    std::function<void(Args...)> func;
    const char * name;
};
//todo потестить данный класс
template<size_t size,typename T,typename COUNTED,template<typename> typename CLOKC>
class clockArray {
    template<size_t INDEX,void (COUNTED::*action)()>
    void perform_withTimeCalc()
    {
        static_assert(INDEX<size);
        clocks_[INDEX].tik();

        (cmd.*action)();

        clocks_[INDEX].tak();
    };

    void printMetrics(std::ostream &out)
    {
        for (int i = 0; i <clocks_.size() ; ++i) {
            out/*<<clock_names.find(i)->second<<'\t'*/<<clocks_[i].getTime()<<"\t";
        }
        //std::cout<<N_.value()<<'\t'<<Nsym_.value()<<'\n';
    }

private:
    std::array<CLOKC<T>,size> clocks_;
    COUNTED& cmd;
};


#endif //DIPLOM_CLOCKARRAY_H
