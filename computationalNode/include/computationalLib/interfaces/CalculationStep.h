
#ifndef DIPLOM_CALCULATIONSTEP_H
#define DIPLOM_CALCULATIONSTEP_H

#include "AbstractSubsriber.h"
#include "DataAcessInteface.h"
#include "parallelUtils/commonDeclarations.h"

namespace inter {
    using core_intrefaces::DataAcessInteface;
    using core_intrefaces::Event;
    using FloatType = double;

    using commonDeclarations::gClk;

    template<typename ... Args>
    class CalculationStep : public core_intrefaces::AbstractProduser<std::shared_ptr<DataAcessInteface>, Args...> {

    public:
        using core_intrefaces::AbstractProduser<std::shared_ptr<DataAcessInteface>, Args...>::AbstractProduser;

        CalculationStep(int a, int b) {
            start_ = a;
            count_ = b;
            terminal = false;
            next_ = {};
            prev_ = nullptr;
        }

        void setPrev(CalculationStep *prev) {
            prev_ = prev;
        }

        void apendNext(CalculationStep *next) {
            next_.push_back(next);//todo bfs logick
        }

        void perform_calc(std::shared_ptr<DataAcessInteface> dat, Args...args) {
            auto loc=gClk.tikLoc();
            perform_calculation(dat, args...);
            gClk.tak();
            std::string vv = "clock" + this->to_string() + std::to_string(start_);
            dat->getProperty(vv) = (double)gClk[loc].time;
            /*if(next_)
            {
               next_->perform_calc();
            }*/
        }

        virtual std::string to_string() {
            return "CalcStep" + std::to_string(start_) + "_" + std::to_string(count_) + "_";
        }

    protected:

        virtual void perform_calculation(std::shared_ptr<DataAcessInteface> dat, Args...args) = 0;

        int start_;
        int count_;
        bool terminal;
        CalculationStep *prev_;
        std::vector<CalculationStep *> next_;//todo кто владеет
    };

} // inter

#endif //DIPLOM_CALCULATIONSTEP_H
