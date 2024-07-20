//
// Created by Lenovo on 17.07.2024.
//

#ifndef DIPLOM_INITCALC_H
#define DIPLOM_INITCALC_H

#include "CalculationStep.h"

namespace inter {
    template<typename ... Args>
    class InitCalc: public CalculationStep<Args ...>{
    public:
        using CalculationStep<Args ...>::CalculationStep;
       // ~InitCalc() override = default;

        std::string to_string() override {
            return "init_"+CalculationStep<Args ...>::to_string();
        }

    protected:
        void perform_calculation(std::shared_ptr<DataAcessInteface> dat,Args...args) override {
            std::vector<std::vector<double>> temp;
            temp.resize(this->count_);
            for (int i = 0; i < this->count_; ++i) {
                temp[i]=std::vector<double>(this->start_,i);
            }
            dat->getdat(this->to_string())=temp;
            this->notify(dat,args...);
        }

    protected:

    };

}

#endif //DIPLOM_INITCALC_H
