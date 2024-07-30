#ifndef DIPLOM_MOCKS_H
#define DIPLOM_MOCKS_H

#include <gmock/gmock.h>
#include "../../application/interfaces/interlib.h"

template<typename ... Args>
class MockEvent : public core_intrefaces::Event<Args...> {
public:
    MockEvent(core_intrefaces::AbstractProduser<Args...>* sender, Args&& ... args)
            : core_intrefaces::Event<Args...>(sender, std::forward<Args>(args)...) {}
    MOCK_METHOD(void, mockMethod, (), ());
};

template<typename T1, typename T2>
class MockAbstractProduser : public core_intrefaces::AbstractProduser<T1, T2> {
public:
    MOCK_METHOD(void, notify, (T1 event1, T2 event2), (override));
    MOCK_METHOD(void, notifySpec, (size_t i, T1 event1, T2 event2), (override));
};

template<typename ... Args>
class MockAbstractSubsriber : public core_intrefaces::AbstractSubsriber<Args...> {
public:
    MOCK_METHOD(void, getNotified, (std::shared_ptr<core_intrefaces::Event<Args...>> event), (override));
};

template<typename ... Args>
class MockIOSub : public core_intrefaces::IOSub<Args...> {
public:
    MockIOSub(std::ostream &out = std::cout) : core_intrefaces::IOSub<Args...>(out) {}

    MOCK_METHOD(void, getNotified, (std::shared_ptr<core_intrefaces::Event<Args...>> event), (override));
};

#endif //DIPLOM_MOCKS_H
