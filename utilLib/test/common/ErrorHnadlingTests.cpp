#include <gtest/gtest.h>
#include "common/errorHandling.h"
#include "testingUtils/GoogleCommon.h"


using namespace shared;
using namespace test_common;

using Type = std::tuple<std::string, std::shared_ptr<MyException>, std::string>;
class ExceptionTypeTs:public  ::testing::TestWithParam<Type>
{

};


std::vector<Type> ts=
    {
        Type(DemangledName<InvalidOption>(), std::make_unique<InvalidOption>("some_option"), "Option some_option does not exist!"),
        Type(DemangledName<outOfRange>(), std::make_unique<outOfRange>(5, 0, 10), "Value 5 is out of range[0,10]!"),
        Type(DemangledName<zeroSize>(), std::make_unique<zeroSize>("my_collection"), "Zero collection my_collection size!"),
        Type(DemangledName<ioError>(), std::make_unique<ioError>("file not found"), "Io error state:file not found!"),
        Type(DemangledName<mismatchedSizes>(), std::make_unique<mismatchedSizes>(3, 4), "Mismatched container sizes for input : a.size = 3, b.size() = 4 !"),
        Type(DemangledName<invalidSizes2>(), std::make_unique<invalidSizes2>(0, -1), "Invalid sizes sz1 = 0, sz2 = -1 received!"),
        Type(DemangledName<RowDivisionError>(), std::make_unique<RowDivisionError>(10, 3), "The total collection size 10 cannot be evenly divided by the specified number of rows 3."),
        Type(DemangledName<HttpError>(), std::make_unique<HttpError>(404, "Not Found"), "HTTP error: code 404 , reason \"Not Found\" !"),
        Type(DemangledName<CurlError>(), std::make_unique<CurlError>("some curl error"), "Curl error: \"some curl error\" !"),
        Type(DemangledName<SQL_ERROR>(), std::make_unique<SQL_ERROR>("error message", "SELECT * FROM table", "HY000"), "SQL Error: error message. Query: SELECT * FROM table. SQL State: HY000!"),
        Type(DemangledName<Already_Connected>(), std::make_unique<Already_Connected>("serv", "database1"), "Service serv is already connected to database1!"),
        Type(DemangledName<Broken_Connection>(), std::make_unique<Broken_Connection>("serv", "database1"), "Service serv is unable to connect to database1!"),
    };

INSTANTIATE_TEST_SUITE_P(
    TestThrowStringConstruction, ExceptionTypeTs,
    testing::ValuesIn(ts),
    FirstValueTuplePrinter<ExceptionTypeTs>);

TEST_P(ExceptionTypeTs, TestValidMessageFormation) {

  auto &[_,exception,valid_message]= this->GetParam();
  try {
    throw *exception;
  } catch (const shared::MyException& e) {
    std::string_view message=e.what();
    EXPECT_STR_CONTAINS(e.what(),valid_message);
  }
}
