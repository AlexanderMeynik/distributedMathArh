
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

BOOST_AUTO_TEST_SUITE_END()
