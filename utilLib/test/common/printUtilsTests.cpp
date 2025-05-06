

#include <iostream>
#include <iomanip>
#include <sstream>
#include <gtest/gtest.h>

#include "common/printUtils.h"

using namespace print_utils;

class IoManipulators : public ::testing::Test {

 protected:
  std::ostream &out_ = std::cout;
};

TEST_F(IoManipulators, test_ios_state_preserve) {
  auto original_flags = out_.flags();
  {
    IosStatePreserve ios(out_);
    out_ << std::scientific << std::setprecision(10);
    ASSERT_NE(out_.flags(), original_flags);
  }
  ASSERT_EQ(out_.flags(), original_flags);
}

TEST_F(IoManipulators, test_ios_state_scientific_precision) {

  auto original_prec = out_.precision();
  size_t new_prec = 6;
  {
    IosStateScientific ios(out_, new_prec);
    ASSERT_EQ(out_.precision(), new_prec);

  }
  ASSERT_EQ(out_.precision(), original_prec);
}

TEST_F(IoManipulators, test_ios_state_scientific_out_of_range) {
  auto original_flags = out_.flags();
  auto original_prec = out_.precision();

  size_t prec = std::numeric_limits<double>::digits10 * 200;

  ASSERT_THROW([&]() { IosStateScientific ios(out_, prec); }(), shared::outOfRange);

  ASSERT_EQ(out_.flags(), original_flags);
  ASSERT_EQ(out_.precision(), original_prec);
}

TEST_F(IoManipulators, test_nested_guards) {
  auto original_flags = out_.flags();
  auto original_prec = out_.precision();
  {
    IosStatePreserve outer(out_);
    out_ << std::hex << std::showbase;
    {
      IosStateScientific inner(out_, 4);
      ASSERT_EQ(out_.precision(), 4);
      ASSERT_TRUE(out_.flags() & std::ios::scientific);
    }

    ASSERT_EQ(out_.precision(), original_prec);
    ASSERT_TRUE(out_.flags() & std::ios::hex);
  }
  ASSERT_EQ(out_.flags(), original_flags);
}

TEST_F(IoManipulators, test_non_copyable) {
  ASSERT_TRUE(!std::is_copy_constructible_v<IosStatePreserve>) << "Guards should not be copyable";
}

TEST_F(IoManipulators, testDelimParseSucess) {
  std::string del = "sssdwds";
  std::istringstream ss(del);
  ASSERT_TRUE(ParseDelim(ss, del));
}

TEST_F(IoManipulators, testDelimParseError) {
  std::string del = "sssdwds";
  std::istringstream ss(del);
  del[1] = 'j';
  ASSERT_FALSE(ParseDelim(ss, del));
}

TEST_F(IoManipulators, testDelimParseUnableToRead) {
  std::istringstream ss("s");
  ASSERT_FALSE(ParseDelim(ss, "sss"));
}

class TestLookup : public ::testing::Test,
                   public ::testing::WithParamInterface<std::tuple<EigenPrintFormats, size_t>> {
};

TEST_P(TestLookup, test_printEnumToFormat) {

  auto [format, index] = GetParam();
  ASSERT_TRUE(operator==(PrintEnumToFormat(format), kEnumTo.at(index)));
}

INSTANTIATE_TEST_SUITE_P(
    lookupTests,
    TestLookup,
    ::testing::Values(
        std::make_tuple(EigenPrintFormats::BASIC_ONE_DIMENSIONAL_VECTOR, 0),
        std::make_tuple(EigenPrintFormats::VECTOR_FORMAT_1, 1),
        std::make_tuple(EigenPrintFormats::MATRIX_FORMAT, 2),
        std::make_tuple(EigenPrintFormats::MATRIX_FORMAT_1, 3)
    ), [](auto &info) { return std::to_string(std::get<1>(info.param)); });

class FormatsIo : public ::testing::Test,
                  public ::testing::WithParamInterface<std::tuple<IoFormat, std::string>> {
 protected:
  static inline std::stringstream ss_ = std::stringstream();

  void TearDown() override {
    ss_.str("");
    ss_.clear();
  }
};

TEST_P(FormatsIo, test_ioFormat_ostream_operator) {

  auto [io, str] = GetParam();
  ss_ << io;
  std::string str2;
  ss_ >> str2;
  ASSERT_EQ(str2, str);
}

TEST_P(FormatsIo, test_ioFormat_istream_operator) {
  auto [io, str] = GetParam();
  ss_ << str;
  IoFormat fmt;
  ss_ >> fmt;
  ASSERT_EQ(io, fmt);
}

INSTANTIATE_TEST_SUITE_P(
    IoFormatIo,
    FormatsIo,
    ::testing::Values(
        std::make_tuple(IoFormat::SERIALIZABLE, "Serializable"),
        std::make_tuple(IoFormat::HUMAN_READABLE, "HumanReadable")
    ), [](auto &info) { return std::get<1>(info.param); });

TEST_F(FormatsIo, test_ioFormat_istream_operator_throw) {
  ss_.str("str");
  IoFormat fmt;
  ASSERT_THROW([&]() { ss_ >> fmt; }(), shared::InvalidOption);
}

TEST_F(FormatsIo, test_eformat_io) {
  auto initial = kEnumTo.at(2);
  ss_ << initial;

  auto res_str = ss_.str();

  EFormat retrieved;
  ss_ >> retrieved;

  ASSERT_TRUE(operator==(initial, retrieved));
}
