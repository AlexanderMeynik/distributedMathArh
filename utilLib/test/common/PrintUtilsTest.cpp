

#include "common/typeCasts.h"

#include "common/printUtils.h"

#include <gtest/gtest.h>
#include <iomanip>
#include <iostream>

TEST(PrintUtils, test_ios_state_preserve) {
    std::ostream &out = std::cout;
    auto flags = out.flags();
    {
        IosStatePreserve ios(out);
        out << std::scientific << std::setprecision(10);
        ASSERT_NE(out.flags(), flags);
    }
    ASSERT_EQ(out.flags(), flags);
}
