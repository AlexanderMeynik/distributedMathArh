#ifndef DIPLOM_ABSTRACTSUBSRIBER_H
#define DIPLOM_ABSTRACTSUBSRIBER_H
#include <memory>
#include <vector>
#include <cassert>
#include <tuple>
#include <iostream>
#include "DataAcessInteface.h"
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

        virtual void notify(Args ...event)
        {
            auto ss = std::make_shared<Event<Args...>>(this, std::forward<Args>(event)...);
            for (auto* ptr:ss_) {
                ptr->getNotified(ss);//std::forward<Args>(event)...));
            }
        };



        virtual void notifySpec(size_t i,Args ... event)
        {
            auto ss = std::make_shared<Event<Args...>>(this, std::forward<Args>(event)...);
            assert(i<ss_.size());
            ss_[i]->getNotified(ss);///getNotified(new Event<Args...>(this,std::forward<Args>(event)...));
        };


    protected:
        std::vector<AbstractSubsriber<Args...>*> ss_;
        //todo implemnt Subs list get functions

    };
    template<typename ... Args>
    class AbstractSubsriber {
    public:
        void subscribe(AbstractProduser<Args...> *producer)
        {
            producer->sub(this);
        }
        virtual void getNotified(std::shared_ptr<Event<Args...>> event)=0;
        friend class Event<Args ...>;


    private:
        std::vector<AbstractProduser<Args...>*> subs_;
    };
    template<typename T,typename ... Args >
    void printFirst(T elem,Args...args)
    {
        std::cout<< (typeid(elem).name())<<'\n';
    }


    template <typename Arg, typename... Args>
    void doPrint(std::ostream& out, Arg&& arg, Args&&... args)
    {
        out << typeid(arg).name()<<'\t';
        ((out << '\t' << typeid(args).name()), ...);
    }

    template <typename TupleT,char del='\t'>
    void printTupleApply(std::ostream&out,const TupleT& tp) {//todo move to mpre suitable place
        std::apply
                (
                        [&out](const auto& first, const auto&... restArgs)
                        {
                            auto printElem = [&out](const auto &x) {
                                if(!std::is_pointer<decltype(x)>::value) {
                                    out << del << x;
                                }
                            };


                            out << '(';
                            if(!std::is_pointer<decltype(first)>::value) {
                                out << first;
                            }
                            (printElem(restArgs), ...);
                        }, tp
                );
        out<<')';
    }


    template<typename ... Args >
    class Event
    {
       // friend class AbstractSubsriber<Args...>;
    public://todo abstarct producer with another set of arguments
        /*Event(AbstractProduser<Args...>* sender,Args &&...args)
        {
            sender_=sender;
            params_={std::forward<Args>(args)...};
        }
        Event(AbstractProduser<Args...>* sender,Args& ...args)
        {
            sender_=sender;
            params_={args...};
        }*/

        template<typename ... U>
        Event(AbstractProduser<Args...>* sender, U&& ... args)
                : sender_(sender), params_(std::forward<U>(args)...) {}

    //protected:
    AbstractProduser<Args...>*sender_;
        //AbstractProduser<Args...>* sender_;
        std::tuple<Args ...> params_;
    };

}

#endif //DIPLOM_ABSTRACTSUBSRIBER_H
