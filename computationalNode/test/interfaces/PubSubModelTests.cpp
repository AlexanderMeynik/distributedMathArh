
#include "computationalLib/interfaces/interlib.h"
#include <iomanip>
#include <typeinfo>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <iomanip>
#include "mocks.h"
#include "common/printUtils.h"

using ::testing::_;
using ::testing::A;
using ::testing::Return;
using ::testing::AtLeast;
using ::testing::Exactly;


TEST(pub_sub_model_test, producer_sub_method_call) {
    MockAbstractProduser<double, std::string> pp;
    auto ssub = std::make_shared<AbstractSubsriber<double, std::string>>();
    EXPECT_CALL(pp, sub(testing::_)).Times(1);
    ssub->subscribe(&pp);

}

TEST(pub_sub_model_test, producer_unsub_method_call) {
    MockAbstractProduser<double, std::string> pp;

    auto ssub = std::make_shared<AbstractSubsriber<double, std::string>>();
    EXPECT_CALL(pp, unsub(testing::_)).Times(1);
    ssub->unsubsribe(&pp);

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


TEST(pub_sub_model_test, unsibsribe_size_t_sucessfull) {
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
    pp.unsub(1);
    ASSERT_TRUE(!pp.isPresent(&ssub2));

    {
        EXPECT_CALL(ssub, getNotified(EventHasParams(1.0, std::string("ssss")))).Times(1);
        EXPECT_CALL(ssub2, getNotified(testing::_)).Times(0);
        pp.notify(1.0, "ssss");
    }

}


TEST(pub_sub_model_test, unsibsribe_size_t_out_of_range) {
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
    EXPECT_THROW(pp.unsub(6), std::out_of_range);

}


TEST(pub_sub_model, test_pub_sub_io) {
    std::stringstream ss;
    std::stringstream res;
    auto io = std::make_shared<IOSub<double>>(res);

    for (int i = 0; i < 10; ++i) {
        auto ptr = std::make_shared<Event<double>>(nullptr, i);
        io->getNotified(ptr);
        printTupleApply(ss, ptr->params_);
        ss << '\n';
        EXPECT_EQ(ss.str(), res.str());
    }
}

TEST(pub_sub_model, test_all_sub_notified)//todo убрать
{
    std::stringstream ss;
    std::stringstream res;
    IOSub<double> io(res);

    for (int i = 0; i < 10; ++i) {
        auto ptr = std::make_shared<MockEvent<double>>(nullptr, i);
        io.getNotified(ptr);

        printTupleApply(ss, ptr->params_);
        //ss<<'\n';
        ss << '\n';
        EXPECT_EQ(ss.str(), res.str());
    }
}

TEST(pub_sub_model, test_io_pub_sub_semantic) {
    const int spec_index = 4;

    AbstractProduser<double, std::string> pp;
    std::vector<std::unique_ptr<MockIOSub<double, std::string>>> mock_subs;

    int a = 10;
    for (int i = 0; i < a; ++i) {
        auto iootr = std::make_unique<MockIOSub<double, std::string>>();
        pp.sub(iootr.get());

        EXPECT_CALL(*iootr, getNotified(testing::_)).Times(1);
        mock_subs.push_back(std::move(iootr));

    }


    pp.notify(12, "ssss");

    EXPECT_CALL(*mock_subs[spec_index].get(), getNotified(EventHasParams(10.0, std::string("aaaa")))).Times(1);


    pp.notifySpec(spec_index, 10, "aaaa");
}


TEST(data_acess_interface, test_data_set) {
    std::string inti("init string");
    auto dd = std::make_shared<DataAcessInteface>();
    std::vector<std::vector<double>> sample(100, std::vector<double>(100, 1));
    dd->getdat(inti) = sample;

    ASSERT_TRUE(dd->isPresent(inti));
    EXPECT_TRUE(dd->getdat(inti).size() == sample.size());
    size_t sz = sample.size();
    for (int idx = 0; idx < sz; idx++) {
        EXPECT_TRUE(dd->getdat(inti)[idx].size() == sample[idx].size());
        size_t sz2 = sample[idx].size();
        for (int i = 0; i < sz2; ++i) {
            SCOPED_TRACE("Indexes : i1 = " + std::to_string(idx) + " i2 = " + std::to_string(i) +
                         '\t' + std::to_string(dd->getdat(inti)[idx][i]) + "!=" + std::to_string(sample[idx][i]));
            EXPECT_TRUE(dd->getdat(inti)[idx][i]  == sample[idx][i]);
        }
    }

}


TEST(computation_step, test_init)//todo сделать
{
    std::shared_ptr<MockDataAcessInteface> dd = std::make_shared<MockDataAcessInteface>();
    //std::string duummy=typeid(*dd.get()).name();
   // std::cout<<duummy;
    InitCalc<double, double> aa(10, 10);

    auto func = [](int i1, int i2, double a, double b) { return std::vector<double>(i1, i2 + i2 * (a * b)); };
    aa.setFunction(func);




    aa.perform_calc(dd, 1, 2);
    EXPECT_CALL(*dd,getdat(testing::An<std::string&&>())).Times(1);//todo Uninteresting mock function call - returning default value.
   // Function call: getdat("init_CalcStep10_10_")


    //EXPECT_CALL(*dd.get(),getdat(aa.to_string())).Times(1);

}

int main(int argc, char **argv) {//todo cmake+gtestmain
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);


    return RUN_ALL_TESTS();
}

