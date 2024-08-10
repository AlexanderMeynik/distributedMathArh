//
// Created by Lenovo on 16.07.2024.
//

#ifndef DIPLOM_CALCULATIONSTEP_H
#define DIPLOM_CALCULATIONSTEP_H

#include "../parallelUtils/OpenmpParallelClock.h"
#include "AbstractSubsriber.h"
#include "DataAcessInteface.h"

namespace inter {
    using core_intrefaces::DataAcessInteface;
    using core_intrefaces::Event;
    using FloatType = double;
    template<typename Tr>
    using CLOCK = timing::OpenmpParallelClock<Tr, &omp_get_wtime, &omp_get_thread_num>;

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
            clock1.tik();
            perform_calculation(dat, args...);
            clock1.tak();
            std::string vv = "clock" + this->to_string() + std::to_string(start_);
            dat->getProperty(vv) = clock1.aggregate<double>();
            /*if(next_)
            {
               next_->perform_calc();
            }*/
        }

        virtual std::string to_string() {
            return "CalcStep" + std::to_string(start_) + "_" + std::to_string(count_) + "_";
        }

    protected:
        CLOCK<FloatType> clock1;

        virtual void perform_calculation(std::shared_ptr<DataAcessInteface> dat, Args...args) = 0;

        int start_;
        int count_;
        bool terminal;
        CalculationStep *prev_;
        std::vector<CalculationStep *> next_;//todo кто владеет
    };

} // inter

#endif //DIPLOM_CALCULATIONSTEP_H
