#ifndef DIPLOM_MOCKS_H
#define DIPLOM_MOCKS_H
#include <iosfwd>
#include <gmock/gmock.h>
#include "../../application/interfaces/interlib.h"
using core_intrefaces::printTupleApply;
template<typename ... Args>
class MockEvent : public core_intrefaces::Event<Args...> {
public:
    MockEvent(core_intrefaces::AbstractProduser<Args...>* sender, Args&& ... args)
            : core_intrefaces::Event<Args...>(sender, std::forward<Args>(args)...) {}
    MOCK_METHOD(void, mockMethod, (), ());
};

template<typename ... Args>
class MockAbstractProduser : public core_intrefaces::AbstractProduser<Args...> {
public:
    //MOCK_METHOD(void, notify, (Args...), (override));
    //MOCK_METHOD(void, notifySpec, (size_t, Args...), (override));

    MOCK_METHOD(void, sub, (core_intrefaces::AbstractSubsriber<Args...>*), (override));
    MOCK_METHOD(void, unsub, (core_intrefaces::AbstractSubsriber<Args...>*), (override));
};

template<typename ... Args>
class MockAbstractSubsriber : public core_intrefaces::AbstractSubsriber<Args...> {
public:
    MOCK_METHOD(void, getNotified, (std::shared_ptr<core_intrefaces::Event<Args...>> event), (override));
};

template<typename ... Args>
class MockIOSub : public core_intrefaces::IOSub<Args...> {
public:
    //using core_intrefaces::IOSub<Args...>::AbstractSubsriber;
    MockIOSub(std::ostream &out = std::cout) : core_intrefaces::IOSub<Args...>(out) {}

    MOCK_METHOD(void, getNotified, (std::shared_ptr<core_intrefaces::Event<Args...>> event), (override));





};


// Custom matcher to check Event parameters
//todo проверить и починить
template<typename... Args>
class EventMatcher : public testing::MatcherInterface<std::shared_ptr<core_intrefaces::Event<Args...>>> {
public:
    EventMatcher(Args... expectedArgs)
            : expectedParams_(std::make_tuple(expectedArgs...)) {}

    bool MatchAndExplain(std::shared_ptr<core_intrefaces::Event<Args...>> event,
                         testing::MatchResultListener* listener) const override {
        if (!event) {
            if (!listener->stream())
            {
                std::cout << "event is null";
                return false;
            }
            *listener << "event is null";

            return false;
        }


        if (event->params_ != expectedParams_) {
            if (!listener->stream()) {
                std::cout << "event parameters don't match. Expected: ";
                printTupleApply(std::cout, expectedParams_);
                std::cout << ", but got: ";
                printTupleApply(std::cout, event->params_);
                std::cout<<'\n';
                return false;
            }
            *listener << "event parameters don't match. Expected: ";
            printTupleApply(*listener->stream(), expectedParams_);
            *listener << ", but got: ";
            printTupleApply(*listener->stream(), event->params_);
            return false;

        }
        return true;
    }

    void DescribeTo(std::ostream* os) const override {
        if (os) {
            *os << "event with parameters ";
            printTupleApply(*os, expectedParams_);
        }
    }

private:
    std::tuple<Args...> expectedParams_;
};

template<typename... Args>
testing::Matcher<std::shared_ptr<core_intrefaces::Event<Args...>>> EventHasParams(Args... args) {
    return MakeMatcher(new EventMatcher<Args...>(args...));
}

#endif //DIPLOM_MOCKS_H
