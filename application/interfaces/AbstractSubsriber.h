//
// Created by Lenovo on 16.07.2024.
//

#ifndef DIPLOM_ABSTRACTSUBSRIBER_H
#define DIPLOM_ABSTRACTSUBSRIBER_H
#include <memory>
#include <vector>
#include <cassert>
#include <tuple>
namespace core_intrefaces {

    template<typename ... Args>
    class AbstractSubsriber;
    template<typename ... Args>
    class Event;
    template<typename ... Args>
    class AbstractProduser {
    public:
        virtual std::string to_string()
        {
            return "default";
        }
        void sub(AbstractSubsriber<Args...>*ss)
        {
            ss_.push_back(ss);
        }

        void unsub(AbstractSubsriber<Args...>*ss)
        {
            std::remove_if(ss_.begin(), ss_.end(),[&ss](auto *elem){return elem==ss;});
        }

        void unsub(size_t i)
        {
            assert(i<ss_.size());
            ss_.erase(ss_.begin()+i);
        }

        void notify(Args ...event)
        {
            for (auto* ptr:ss_) {
                ptr->getNotified(new Event<Args...>(this,event...));//std::forward<Args>(event)...));
            }
        };



        void notifySpec(size_t i,Args&&... event)
        {
            assert(i<ss_.size());
            ss_[i]->getNotified(new Event<Args...>(this,event...));///getNotified(new Event<Args...>(this,std::forward<Args>(event)...));
        };


    private:
        std::vector<AbstractSubsriber<Args...>*> ss_;

    };
    template<typename ... Args>
    class AbstractSubsriber {
    public:
        void subscribe(AbstractProduser<Args...> *producer)
        {
            producer->sub(this);
        }
        virtual void getNotified(Event<Args...>*event)=0;
        friend class Event<Args ...>;


    private:
        std::vector<AbstractProduser<Args...>*> subs_;
    };
    template<typename ... Args >
    class Event
    {
       // friend class AbstractSubsriber<Args...>;
    public:
        Event(AbstractProduser<Args...>* sender,Args &&...args)
        {
            sender_=sender;
            params_={std::forward<Args>(args)...};
        }
        Event(AbstractProduser<Args...>* sender,Args ...args)
        {
            sender_=sender;
            params_={args...};
        }
    //protected:
        AbstractProduser<Args...>* sender_;
        std::tuple<Args ...> params_;
    };
}

#endif //DIPLOM_ABSTRACTSUBSRIBER_H
