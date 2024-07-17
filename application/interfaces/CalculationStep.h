//
// Created by Lenovo on 16.07.2024.
//

#ifndef DIPLOM_CALCULATIONSTEP_H
#define DIPLOM_CALCULATIONSTEP_H

#include "../parallelUtils/OpenmpParallelClock.h"
#include "AbstractSubsriber.h"
#include "DataAcessInteface.h"
namespace inter {
    using FloatType=double;
    //todo partial impelemtation;
        template<typename Tr>
        using CLOCK=timing::OpenmpParallelClock<Tr,&omp_get_wtime,&omp_get_thread_num>;//todo notify clock when it gets to be clocked
    class CalculationStep :public core_intrefaces::AbstractProduser<double,double>{

     public:
         virtual ~CalculationStep()=0;
         void perform_calc()
         {
             clock1.tik();
             perform_calculation();
             clock1.tak();
             if(next_)
             {
                next_->perform_calc();
             }
         }
         virtual std::string to_string()
         {
             return "default";
         }
         void setNext(CalculationStep*cc)
         {
             this->next_=cc;
         }

     private:

         CalculationStep* next_;
         CLOCK<FloatType> clock1;
         virtual void perform_calculation()=0;

    };

} // inter

#endif //DIPLOM_CALCULATIONSTEP_H
