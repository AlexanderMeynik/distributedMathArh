
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN  // in only one cpp file

#include "../../application/interfaces/interlib.h"
#include <boost/test/unit_test.hpp>

#include <iomanip>
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
        DataAcessInteface dd;
        IOSub<double,double> io;
        Event<DataAcessInteface&,double,double> aa(&io,dd,1.0,2.0);
        //Event<double,double>* aa=new Event<double,double>(&io,1.0,2.0);
        //InitCalc<double,double> aa(1000,1000);
        //aa.perform_calc(dd,1.0,1.2);


    }

    BOOST_AUTO_TEST_CASE( test_computational_steps )
/* Compare with void free_test_function() */
    {
        DataAcessInteface dd;
        IOSub<double> io;

        //InitCalc<double,double> aa(1000,1000);
        //aa.perform_calc(dd,1.0,1.2);


    }

BOOST_AUTO_TEST_SUITE_END()
