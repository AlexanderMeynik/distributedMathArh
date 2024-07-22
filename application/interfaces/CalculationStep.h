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
    using FloatType=double;
    //todo partial impelemtation;
        template<typename Tr>
        using CLOCK=timing::OpenmpParallelClock<Tr,&omp_get_wtime,&omp_get_thread_num>;//todo notify clock when it gets to be clocked
    template<typename ... Args>
class CalculationStep :public core_intrefaces::AbstractProduser<std::shared_ptr<DataAcessInteface>,Args...>{

     public:
        using core_intrefaces::AbstractProduser<std::shared_ptr<DataAcessInteface>,Args...>::AbstractProduser;
        CalculationStep(int a,int b)
        {
            start_=a;
            count_=b;
        }

         //virtual ~CalculationStep()=0;
         void perform_calc(std::shared_ptr<DataAcessInteface> dat,Args...args)
         {
             clock1.tik();
             perform_calculation(dat,args...);
             clock1.tak();
             std::string vv="clock"+this->to_string()+std::to_string(start_);
             dat->getProperty(vv)=clock1.aggregate<double>();
             /*if(next_)
             {
                next_->perform_calc();
             }*/
         }
         virtual std::string to_string()
         {
             return "CalcStep"+std::to_string(start_)+"_"+std::to_string(count_)+"_";
         }

    protected:
         //CalculationStep* next_;
         CLOCK<FloatType> clock1;
         virtual void perform_calculation(std::shared_ptr<DataAcessInteface> dat,Args...args)=0;
         int start_;
         int count_;

    };

} // inter

#endif //DIPLOM_CALCULATIONSTEP_H
