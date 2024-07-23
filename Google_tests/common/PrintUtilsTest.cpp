

#include "../../application/common/lib.h"

#include "../../application/common/printUtils.h"

#include <gtest/gtest.h>
#include <iomanip>
#include <iostream>


TEST(PrintUtils,test_ios_state_preserve)
{
    std::ostream &out=std::cout;
    auto flags=out.flags();
    {
        IosStatePreserve ios(out);
        out<<std::scientific<<std::setprecision(10);
        ASSERT_NE(out.flags(),flags);
    }


//todo fix(it's not woking)

    ASSERT_EQ(out.flags(),flags);
}
