//
// Created by Lenovo on 17.07.2024.
//

#ifndef DIPLOM_IOSUB_H
#define DIPLOM_IOSUB_H

#include <iostream>
#include "AbstractSubsriber.h"
#include "common/printUtils.h"

namespace core_intrefaces {


    template<typename ...Args>

    class IOSub : public AbstractSubsriber<Args...> {
    public:
        explicit IOSub<Args...>(std::ostream &out = std::cout) : out_(out) {
        }

        void getNotified(std::shared_ptr<Event<Args...>> event) override {

            // std::apply([this](auto&&... args) {((
            //     out_ << args << '\t'), ...);}, event->params_);
            printTupleApply(out_, event->params_);
            out_ << '\n';
        }

        //protected://todo неплохой костыль дял теста
        std::ostream &out_;
    };
}


#endif //DIPLOM_IOSUB_H
