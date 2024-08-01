
#include "../../application/interfaces/interlib.h"
#include <iomanip>
#include <typeinfo>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <iomanip>
#include "mocks.h"

using ::testing::_;
using ::testing::A;
using ::testing::Return;
using ::testing::AtLeast;
using ::testing::Exactly;


TEST(pub_sub_model_test, producer_sub_method_call) {
    MockAbstractProduser<double, std::string> pp;

    auto ssub=std::make_shared<AbstractSubsriber<double, std::string>>();

    {
        EXPECT_CALL(pp, sub(testing::_)).Times(1);
        ssub->subscribe(&pp);
    }

    {
        EXPECT_CALL(pp, unsub(testing::_)).Times(1);//todo mock these
        ssub->unsubsribe(&pp);
    }

}

TEST(pub_sub_model_test, io_pub_sub_semantic_Test) {
    AbstractProduser<double, std::string> pp;
    MockAbstractSubsriber<double, std::string> ssub;
    ssub.subscribe(&pp);
    ASSERT_TRUE(pp.isPresent(&ssub));

    EXPECT_CALL(ssub, getNotified(EventHasParams(12.0, std::string("ssss")))).Times(1);
    pp.notify(12.0, "ssss");
}

TEST(pub_sub_model_test, unsibscribe_semantic_test) {
    AbstractProduser<double, std::string> pp;
    MockAbstractSubsriber<double, std::string> ssub;
    MockAbstractSubsriber<double, std::string> ssub2;
    ssub.subscribe(&pp);
    ssub2.subscribe(&pp);
    {

        EXPECT_CALL(ssub, getNotified(EventHasParams(12.0, std::string("ssss")))).Times(1);
        EXPECT_CALL(ssub2, getNotified(EventHasParams(12.0, std::string("ssss")))).Times(1);

        pp.notify(12.0, "ssss");
    }
    ssub2.unsubsribe(&pp);
    ASSERT_TRUE(!pp.isPresent(&ssub2));

    {
        EXPECT_CALL(ssub, getNotified(EventHasParams(1.0, std::string("ssss")))).Times(1);
        EXPECT_CALL(ssub2, getNotified(testing::_)).Times(0);
        pp.notify(1.0, "ssss");
    }

}

TEST(pub_sub_model,test_pub_sub_io)
{
    std::stringstream ss;
    std::stringstream  res;
    auto io = new IOSub<double>(res);

    for (int i = 0; i <10; ++i) {
        auto ptr=std::make_shared<Event<double>>(nullptr, i);
        io->getNotified(ptr);
        printTupleApply(ss,ptr->params_);
        ss<<'\n';
        EXPECT_EQ(ss.str(),res.str());
    }
}

TEST(pub_sub_model,test_all_sub_notified)//todo mock для event
{
    std::stringstream ss;
    std::stringstream  res;
    IOSub<double> io(res);


    /*ON_CALL(io, getNotified(testing::_)).WillByDefault(testing::Invoke([&io](std::shared_ptr<Event<double>> event) {
        printTupleApply(io.out_,event->params_);
        io.out_<<'\n';
    }));*/

   // EXPECT_CALL(io,getNotified(testing::_)).Times(10);
    for (int i = 0; i <10; ++i) {
        auto ptr=std::make_shared<MockEvent<double>>(nullptr, i);
        io.getNotified(ptr);




        printTupleApply(ss,ptr->params_);
        //ss<<'\n';
        ss<<'\n';
        EXPECT_EQ(ss.str(),res.str());
    }
}

TEST(pub_sub_model,test_io_pub_sub_semantic)
{
    const int spec_index=4;

    AbstractProduser<double, std::string> pp;
    std::vector<std::unique_ptr<MockIOSub<double, std::string>>> mock_subs;

    int a = 10;
    for (int i = 0; i < a; ++i) {
        auto iootr=std::make_unique<MockIOSub<double, std::string>>();
        pp.sub(iootr.get());

        EXPECT_CALL(*iootr, getNotified(testing::_)).Times(1);
        mock_subs.push_back(std::move(iootr));

    }


    pp.notify(12, "ssss");

    EXPECT_CALL(*mock_subs[spec_index].get(), getNotified(EventHasParams(10.0, std::string("aaaa")))).Times(1);


    pp.notifySpec(spec_index, 10, "aaaa");
}
TEST(pub_sub_model,test_event_construction)//todo сделать
{
    auto dd = std::make_shared<DataAcessInteface>();
    // IOSub<std::shared_ptr<DataAcessInteface>,double,double> io;

    auto io = std::make_shared<AbstractProduser<std::shared_ptr<DataAcessInteface>, double, double>>();
    std::cout << typeid(io).name() << '\n';
    Event aa(io.get(), dd, 1.0, 2.0);//args are deduced using io
    //Event<double,double>* aa=new Event<double,double>(&io,1.0,2.0);
    //InitCalc<double,double> aa(1000,1000);
    //aa.perform_calc(dd,1.0,1.2);



}

TEST(computation_step,test_init)//todo сделать
{
    auto dd = std::make_shared<DataAcessInteface>();
    IOSub<double> io;
    //std:opti

    InitCalc<double, double> aa(10, 10);
    auto func = [](int i1, int i2, double a, double b) { return std::vector<double>(i1, i2 + i2 * (a * b)); };
    aa.setFunction(func);
    aa.perform_calc(dd, 1, 2);//todo понять чё тестим
    int a = 0;
}

int main(int argc, char **argv)
{//todo cmake+gtestmain
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);


    return RUN_ALL_TESTS();
}

