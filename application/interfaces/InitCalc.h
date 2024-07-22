#ifndef DIPLOM_INITCALC_H
#define DIPLOM_INITCALC_H

#include "CalculationStep.h"
#include <optional>
namespace inter {
    template<typename ... Args>
    class InitCalc: public CalculationStep<Args ...>{
    public:
        using CalculationStep<Args ...>::CalculationStep;

        std::string to_string() override {
            return "init_"+CalculationStep<Args ...>::to_string();
        }
        void setFunction(std::function<std::vector<double>(int,int,Args ...)>&&func)
        {
            source_=std::forward<decltype(func)>(func);
        }

    protected:
        void perform_calculation(std::shared_ptr<DataAcessInteface> dat,Args...args) override {
            if(source_.has_value())
            {
                std::vector<std::vector<double>> temp;//tododo floattype
                temp.resize(this->count_);
                for (int i = 0; i < this->count_; ++i) {
                    temp[i]=source_.value()(this->start_,i,args...);
                }
                dat->getdat(this->to_string())=temp;
                this->notify(dat,args...);
                return;
            }
            std::vector<std::vector<double>> temp;//tododo floattype
            temp.resize(this->count_);
            for (int i = 0; i < this->count_; ++i) {
                temp[i]=std::vector<double>(this->start_,i);
            }
            dat->getdat(this->to_string())=temp;
            this->notify(dat,args...);
        }

        std::optional<std::function<std::vector<double>(int,int,Args ...)>> source_;

    };

}

#endif //DIPLOM_INITCALC_H
