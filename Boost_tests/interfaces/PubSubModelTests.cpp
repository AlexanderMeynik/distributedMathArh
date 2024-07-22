
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN  // in only one cpp file

#include "../../application/interfaces/interlib.h"
#include <boost/test/unit_test.hpp>
#include <boost/test/tools/output_test_stream.hpp>
#include <iomanip>
#include <typeinfo>
#include "../BoostTestCommon.h"

#define BOOST_TEST_MODULE printUtils
BOOST_AUTO_TEST_SUITE(pubSub)

    BOOST_AUTO_TEST_CASE(test_notify_io)//todo mock для event
    {
        std::stringstream ss;
        boost::test_tools::output_test_stream buf;
        auto io = new IOSub<double>(buf);

        for (int i = 0; i <10; ++i) {
            auto ptr=std::make_shared<Event<double>>(nullptr, i);
            io->getNotified(ptr);
            printTupleApply(ss,ptr->params_);
            ss<<'\n';
            BOOST_CHECK_EQUAL(ss.str(),buf.str());
        }
    }

    BOOST_AUTO_TEST_CASE(test_io_pub_sub_semantics)//todo integr test
/* Compare with void free_test_function() */
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
        boost::test_tools::output_test_stream buf;
        AbstractProduser<double, std::string> pp;
        int a = 10;
        std::vector<std::shared_ptr<AbstractSubsriber<double, std::string>>> subs;
        for (int i = 0; i < a; ++i) {
            //subs.push_back(std::make_shared<IOSub<double,std::string>>());
            pp.sub(new IOSub<double, std::string>(buf));
        }
        pp.notify(12, "ssss");
        pp.notifySpec(0, 10, "aaaa");//todo подумать над тем, как храним параметры в event(rvalue ссфлка или значения)
        //std::cout<<buf.str();

        BOOST_CHECK_EQUAL(buf.str(), res_buffer);


    }

    BOOST_AUTO_TEST_CASE(test_event_construction)
/* Compare with void free_test_function() */
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

    BOOST_AUTO_TEST_CASE(test_computational_steps)
    {
        auto dd = std::make_shared<DataAcessInteface>();
        IOSub<double> io;
        //std:opti

        InitCalc<double, double> aa(10, 10);
        auto func = [](int i1, int i2, double a, double b) { return std::vector<double>(i1, i2 + i2 * (a * b)); };
        aa.setFunction(func);
        aa.perform_calc(dd, 1, 2);
        int a = 0;
//tpodo add https://en.cppreference.com/w/cpp/thread/promise
    }

BOOST_AUTO_TEST_SUITE_END()
