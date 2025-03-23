


#include <iostream>
#include <iomanip>
#include <sstream>
#include <gtest/gtest.h>


#include "common/printUtils.h"

using namespace printUtils;



class IoManipulators : public ::testing::Test {

protected:
    std::ostream &out = std::cout;
};

TEST_F(IoManipulators, test_ios_state_preserve) {
    auto originalFlags = out.flags();
    {
        IosStatePreserve ios(out);
        out << std::scientific << std::setprecision(10);
        ASSERT_NE(out.flags(), originalFlags);
    }
    ASSERT_EQ(out.flags(), originalFlags);
}



TEST_F(IoManipulators, test_ios_state_scientific_precision) {

    auto originalPrec = out.precision();
    size_t newPrec = 6;
    {
        IosStateScientific ios(out, newPrec);
        ASSERT_EQ(out.precision(), newPrec);

    }
    ASSERT_EQ(out.precision(),originalPrec);
}

TEST_F(IoManipulators, test_ios_state_scientific_out_of_range) {
    auto originalFlags = out.flags();
    auto originalPrec = out.precision();

    size_t prec=std::numeric_limits<double>::digits10*200;

    ASSERT_THROW([&](){IosStateScientific ios(out,prec);}(),shared::outOfRange);

    ASSERT_EQ(out.flags(), originalFlags);
    ASSERT_EQ(out.precision(), originalPrec);
}


TEST_F(IoManipulators, test_nested_guards) {
    auto originalFlags = out.flags();
    auto originalPrec = out.precision();
    {
        IosStatePreserve outer(out);
        out << std::hex << std::showbase;
        {
            IosStateScientific inner(out, 4);
            ASSERT_EQ(out.precision(), 4);
            ASSERT_TRUE(out.flags() & std::ios::scientific);
        }

        ASSERT_EQ(out.precision(), originalPrec);
        ASSERT_TRUE(out.flags() & std::ios::hex);
    }
    ASSERT_EQ(out.flags(), originalFlags);
}

TEST_F(IoManipulators, test_non_copyable) {
    ASSERT_TRUE(!std::is_copy_constructible_v<IosStatePreserve>)<<"Guards should not be copyable";
}

class TestLookup : public ::testing::Test,
        public ::testing::WithParamInterface<std::tuple<EigenPrintFormats,size_t>>{};

TEST_P(TestLookup, test_printEnumToFormat) {

    auto [format,index]=GetParam();
    ASSERT_TRUE(operator==(printEnumToFormat(format),enumTo.at(index)));
}

INSTANTIATE_TEST_SUITE_P(
        lookupTests,
        TestLookup,
        ::testing::Values(
                std::make_tuple(EigenPrintFormats::BasicOneDimensionalVector,0),
                std::make_tuple(EigenPrintFormats::VectorFormat1,1),
                std::make_tuple(EigenPrintFormats::MatrixFormat,2),
                std::make_tuple(EigenPrintFormats::MatrixFormat1,3)
        ), [](auto&info){return std::to_string(std::get<1>(info.param));});


class formatsIo : public ::testing::Test,
        public ::testing::WithParamInterface<std::tuple<ioFormat,std::string>>
{
    protected:
    static inline std::stringstream ss=std::stringstream();

    void TearDown() override {
        ss.str("");
        ss.clear();
    }
};

TEST_P(formatsIo, test_ioFormat_ostream_operator) {

    auto [io,str]=GetParam();
    ss<<io;
    std::string str2;
    ss>>str2;
    ASSERT_EQ(str2, str);
}

TEST_P(formatsIo, test_ioFormat_istream_operator) {
    auto [io,str]=GetParam();
    ss<<str;
    ioFormat fmt;
    ss >> fmt;
    ASSERT_EQ(io, fmt);
}

INSTANTIATE_TEST_SUITE_P(
        IoFormatIo,
        formatsIo,
        ::testing::Values(
                std::make_tuple(ioFormat::Serializable,"Serializable"),
                std::make_tuple(ioFormat::HumanReadable,"HumanReadable")
        ), [](auto&info){return std::get<1>(info.param);});

TEST_F(formatsIo, test_ioFormat_istream_operator_throw) {
    ss.str("str");
    ioFormat fmt;
    ASSERT_THROW([&](){ss >> fmt;}(), shared::InvalidOption);
}

TEST_F(formatsIo, test_eformat_io) {
    auto initial=enumTo.at(2);
    ss << initial;

    auto resStr=ss.str();

    EFormat  retrieved;
    ss>>retrieved;

    ASSERT_TRUE(operator==(initial, retrieved));
}
