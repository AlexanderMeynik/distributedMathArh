#ifndef DIPLOM_ABSTRACTSUBSRIBER_H
#define DIPLOM_ABSTRACTSUBSRIBER_H
#include <memory>
#include <vector>
#include <cassert>
#include <tuple>
#include <iostream>
#include "DataAcessInteface.h"
#include <exception>
namespace core_intrefaces {

    template<typename ... Args>
    class AbstractSubsriber;
    template<typename ... Args>
    class Event;


    template<typename ... Args>
    class IProduser {
    public:
        virtual ~IProduser() = default;
        virtual void sub(AbstractSubsriber<Args...>* ss) = 0;
        virtual void unsub(AbstractSubsriber<Args...>* ss) = 0;
        virtual void unsub(size_t i) = 0;
    };

    template<typename ... Args>

    class AbstractProduser: public IProduser< Args ...>{
    public:
        virtual std::string to_string()
        {
             return typeid(this).name()+std::to_string((unsigned long long)(void**)this);;
        }
        void sub(AbstractSubsriber<Args...>*ss)
        {
            ss_.push_back(ss);
        }

        void unsub(AbstractSubsriber<Args...>*ss)
        {
            erase_if(ss_,[&ss](auto *elem){return elem==ss;});
        }

        void unsub(size_t i)
        {
            if(i>=ss_.size())
            {
                throw std::out_of_range("Argument "+std::to_string(i)+"is out of range");
            }
            ss_.erase(ss_.begin()+i);
        }

        virtual void notify(Args ...event)
        {
            auto ss = std::make_shared<Event<Args...>>(this, std::forward<Args>(event)...);
            for (auto* ptr:ss_) {
                ptr->getNotified(ss);//std::forward<Args>(event)...));
            }
        };
        size_t getIndex(AbstractSubsriber<Args...>*ss) const
        {
            auto it=std::find_if(ss_.begin(), ss_.end(),[ss](auto *a){return ss==a;});
            return (it==ss_.end())?-1:it-ss_.begin();
        }

        bool isPresent(AbstractSubsriber<Args...>*ss) const {
            return getIndex(ss)!=-1;
        }

        [[nodiscard]] size_t get_size() const
        {
            return ss_.size();
        }



        virtual void notifySpec(size_t i,Args ... event)
        {
            auto ss = std::make_shared<Event<Args...>>(this, std::forward<Args>(event)...);
            assert(i<ss_.size());
            ss_[i]->getNotified(ss);
        };

    protected:
        std::vector<AbstractSubsriber<Args...>*> ss_;
    };
    template<typename ... Args>
    class AbstractSubsriber {
    public:
        [[nodiscard]] std::string to_string() const
        {
            return typeid(this).name()+std::to_string((unsigned long long)(void**)this);
        }
        void subscribe(AbstractProduser<Args...> *producer)
        {
            producer->sub(this);
        }
        void unsubsribe(AbstractProduser<Args...> *producer)
        {
            producer->unsub(this);
        }
        virtual void getNotified(std::shared_ptr<Event<Args...>> event) { };
        friend class Event<Args ...>;


    private:
        std::vector<AbstractProduser<Args...>*> subs_;
    };


    template<typename ... Args >
    class Event
    {
    public://todo abstarct producer with another set of arguments

        virtual bool operator==(const Event<Args...>& another) const
        {
            return this->params_==another.params_&& this->sender_==another.sender_;
        }

        template<typename ... U>
        Event(AbstractProduser<Args...>* sender, U&& ... args)
                : sender_(sender), params_(std::forward<U>(args)...) {}

        AbstractProduser<Args...>*sender_;
        std::tuple<Args ...> params_;
    };

}

#endif //DIPLOM_ABSTRACTSUBSRIBER_H
