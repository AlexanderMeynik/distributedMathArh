
#include "../../application/interfaces/interlib.h"
#include <iomanip>
#include <typeinfo>

#include <gtest/gtest.h>
#include <iomanip>


#define BOOST_TEST_MODULE printUtils

TEST(pub_sub_model,test_pub_sub_io)
{
    std::stringstream ss;
    testing::internal::CaptureStdout();
    auto io = new IOSub<double>();

    for (int i = 0; i <10; ++i) {
        auto ptr=std::make_shared<Event<double>>(nullptr, i);
        io->getNotified(ptr);
        printTupleApply(ss,ptr->params_);
        ss<<'\n';
        ASSERT_EQ(ss.str(),testing::internal::GetCapturedStdout());
    }
}
TEST(pub_sub_model,test_all_sub_notified)//todo mock для event
{
    std::stringstream ss;
    testing::internal::CaptureStdout();
    auto io = new IOSub<double>();

    for (int i = 0; i <10; ++i) {
        auto ptr=std::make_shared<Event<double>>(nullptr, i);
        io->getNotified(ptr);
        printTupleApply(ss,ptr->params_);
        ss<<'\n';
        ASSERT_EQ(ss.str(),testing::internal::GetCapturedStdout());
    }
}

TEST(pub_sub_model,test_io_pub_sub_semantic)//todo mock для event
{
    std::string res_buffer = "(12\tssss)\n"
                              "(12\tssss)\n"
                              "(12\tssss)\n"
                              "(12\tssss)\n"
                              "(12\tssss)\n"
                              "(12\tssss)\n"
                              "(12\tssss)\n"
                              "(12\tssss)\n"
                              "(12\tssss)\n"
                              "(12\tssss)\n"
                              "(10\taaaa)\n";
    testing::internal::CaptureStdout();

    AbstractProduser<double, std::string> pp;
    int a = 10;
    std::vector<std::shared_ptr<AbstractSubsriber<double, std::string>>> subs;
    for (int i = 0; i < a; ++i) {
        //subs.push_back(std::make_shared<IOSub<double,std::string>>());
        pp.sub(new IOSub<double, std::string>());
    }
    pp.notify(12, "ssss");
    pp.notifySpec(0, 10, "aaaa");//todo подумать над тем, как храним параметры в event(rvalue ссфлка или значения)
    //std::cout<<buf.str();

    ASSERT_EQ(res_buffer,testing::internal::GetCapturedStdout());

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
    aa.perform_calc(dd, 1, 2);
    int a = 0;
}


