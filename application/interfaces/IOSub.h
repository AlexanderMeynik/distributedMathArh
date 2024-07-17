//
// Created by Lenovo on 17.07.2024.
//

#ifndef DIPLOM_IOSUB_H
#define DIPLOM_IOSUB_H

#include <iostream>
#include "AbstractSubsriber.h"

namespace core_intrefaces {
template<typename ...Args>

    class IOSub :public AbstractSubsriber<Args...>{
    public:
        void getNotified(Event<Args...> *event) override {

            std::cout<<event->sender_->to_string()<<'\t';
            //auto size=sizeof ... (Args);

            std::apply([](auto&&... args) {((std::cout << args << '\t'), ...);}, event->params_);
            std::cout<<'\n';
            delete event;
            /*for (int i = 0; i < size; ++i) {
                std::cout<<event->params_.
            }*/
        }

    };
}


#endif //DIPLOM_IOSUB_H
