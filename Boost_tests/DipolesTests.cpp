#include "../application/commonFunctions/lib.h"

#include <boost/test/unit_test.hpp>
#include "../application/printUtils.h"
#include <iomanip>
#define BOOST_TEST_MODULE dipolesTest
BOOST_AUTO_TEST_SUITE( printUtils )


    BOOST_AUTO_TEST_CASE( test_ios_state_preserve23 )
/* Compare with void free_test_function() */
    {
        std::ostream &out=std::cout;
        auto flags=out.flags();
        {
            IosStatePreserve ios(out);
            out<<std::scientific<<std::setprecision(10);
            BOOST_TEST(out.flags()!=flags);
        }
        BOOST_TEST(out.flags()=flags);


    }

BOOST_AUTO_TEST_SUITE_END()
