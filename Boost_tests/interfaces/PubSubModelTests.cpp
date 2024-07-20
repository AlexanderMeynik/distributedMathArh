
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN  // in only one cpp file

#include "../../application/interfaces/interlib.h"
#include <boost/test/unit_test.hpp>

#include <iomanip>
#include <typeinfo>
#define BOOST_TEST_MODULE printUtils
BOOST_AUTO_TEST_SUITE( printUtils )


    BOOST_AUTO_TEST_CASE( test_sub_workflow )
/* Compare with void free_test_function() */
    {
        AbstractProduser<double,std::string> pp;
        int a=10;
        std::vector<std::shared_ptr<AbstractSubsriber<double,std::string>>> subs;
        for (int i = 0; i < a; ++i) {
            //subs.push_back(std::make_shared<IOSub<double,std::string>>());
            pp.sub(new IOSub<double,std::string>());
        }
        pp.notify(12,"ssss");
        pp.notifySpec(0,10,"aaaa");//todo подумать над тем, как храним параметры в event(rvalue ссфлка или значения)



    }

    BOOST_AUTO_TEST_CASE( test_event_construction )
/* Compare with void free_test_function() */
    {
        auto dd=std::make_shared<DataAcessInteface>();
       // IOSub<std::shared_ptr<DataAcessInteface>,double,double> io;

        auto io=std::make_shared<AbstractProduser<std::shared_ptr<DataAcessInteface>,double,double>>();
        std::cout<< typeid(io).name()<<'\n';
        Event aa(io.get(),dd,1.0,2.0);//args are deduced using io
        //Event<double,double>* aa=new Event<double,double>(&io,1.0,2.0);
        //InitCalc<double,double> aa(1000,1000);
        //aa.perform_calc(dd,1.0,1.2);


    }

    BOOST_AUTO_TEST_CASE( test_computational_steps )
/* Compare with void free_test_function() */
    {
        auto dd=std::make_shared<DataAcessInteface>();
        IOSub<double> io;

        InitCalc<double,double> aa(10,10);
        aa.perform_calc(dd,1.0,1.2);
        int a=0;

    }

BOOST_AUTO_TEST_SUITE_END()
