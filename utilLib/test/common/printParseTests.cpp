#include <sstream>
#include <gtest/gtest.h>

#include "common/Parsers.h"
#include "common/Printers.h"

#include "../../computationalLib/test/GoogleCommon.h"//todo make common
using namespace testCommon;
using namespace printUtils;

using ttype=std::tuple<std::string,meshStorage::MeshCreator,ioFormat,bool,bool>;
class MeshSerializeDeserializeTs : public ::testing::TestWithParam<ttype> {
public:


protected:
    static inline FloatType anotherErr = 1e-14;
};

std::vector<ttype> generateFixture(int nnum=0)
{
    std::vector<ttype> res;

    meshStorage::MeshCreator ms;
    ms.constructMeshes({20ul,20ul},{0.0,10.0,0.0,20.0});

    ms.applyFunction([](auto x,auto y){
       return (x>y)?x:y;
    });
    ms.computeViews();
    ttype a1=ttype{"default",ms,ioFormat::Serializable, true, true};
    ttype a2=ttype{"noDims",ms,ioFormat::Serializable, true, false};
    ttype a3=ttype{"noLims",ms,ioFormat::Serializable, false, true};
    ttype a4=ttype{"noDimsNoLims",ms,ioFormat::Serializable, false, false};

    res.push_back(a1);
    res.push_back(a2);
    res.push_back(a3);
    res.push_back(a4);

    if(nnum==1)
    {
        ttype a5=ttype{"default_HR",ms,ioFormat::HumanReadable, true, true};
        ttype a6=ttype{"noDims_HR",ms,ioFormat::HumanReadable, true, false};
        ttype a7=ttype{"noLims_HR",ms,ioFormat::HumanReadable, false, true};
        ttype a8=ttype{"noDimsNoLims_HR",ms,ioFormat::HumanReadable, false, false};

        res.push_back(a5);
        res.push_back(a6);
        res.push_back(a7);
        res.push_back(a8);
    }

    return res;
}

TEST_P(MeshSerializeDeserializeTs,testJsonSerialization)
{
    using namespace testCommon;
    auto &[_,mesh,io,printDims,printLims]=GetParam();


    auto dimopt=printDims?std::nullopt:std::optional{mesh.dimensions};
    auto limopt=printLims?std::nullopt:std::optional{mesh.limits};

    auto ser= toJson(mesh,printDims,printLims);


    auto deser= fromJson(ser,dimopt,limopt);

    if(!printDims)
    {

        compareArrays(mesh.dimensions,deser.dimensions,arrayEqualComparator<size_t>::call, anotherErr);
    }

    if(!printLims)
    {
        compareArrays(mesh.limits, deser.limits, arrayDoubleComparator<FloatType>::call, anotherErr);
    }
    compareArrays(mesh.data[2], deser.data[2], arrayDoubleComparator<FloatType>::call, anotherErr);


}
INSTANTIATE_TEST_SUITE_P(
        SerDeserJson,
        MeshSerializeDeserializeTs,
        ::testing::ValuesIn(generateFixture()

        ), [](auto&info){return std::get<0>(info.param);});



class MeshPrintReadTs : public ::testing::TestWithParam<ttype> {
public:

    MeshPrintReadTs()
    {
        ss.precision(defaultPrec);
    }

protected:
    static inline FloatType anotherErr = 1e-14;
    static inline std::stringstream ss=std::stringstream();

    void TearDown() override {
        ss.str("");
        ss.clear();
    }
};


INSTANTIATE_TEST_SUITE_P(
        PrintParseTests,
        MeshPrintReadTs,
        ::testing::ValuesIn(generateFixture(0)

        ), [](auto&info){return std::get<0>(info.param);});



TEST_P(MeshPrintReadTs,testOstreamPrint)
{
    using namespace testCommon;
    auto &[_,mesh,io,printDims,printLims]=GetParam();


    auto dimopt=printDims?std::nullopt:std::optional{mesh.dimensions};
    auto limopt=printLims?std::nullopt:std::optional{mesh.limits};

    printMesh(ss,mesh,io,printDims,printLims);


    auto deser=parseMeshFrom(ss,dimopt,limopt);

    if(!printDims)
    {

        compareArrays(mesh.dimensions,deser.dimensions,arrayEqualComparator<size_t>::call, anotherErr);
    }

    if(!printLims)
    {
        compareArrays(mesh.limits, deser.limits, arrayDoubleComparator<FloatType>::call, anotherErr);
    }
    compareArrays(mesh.data[2], deser.data[2], arrayDoubleComparator<FloatType>::call, anotherErr);


}


template<typename Col>
using contFixture=std::tuple<std::string,Col,ioFormat,bool>;
using ContTypes = ::testing::Types<ct::stdVec,ct::EigenVec,ct::meshStorageType>;

using conT=contFixture<ct::stdVec>;
class ParseContTs : public ::testing::TestWithParam<conT> {
public:


protected:
    static inline FloatType anotherErr = 1e-14;

    static inline std::stringstream ss=std::stringstream();

    void TearDown() override {
        ss.str("");
        ss.clear();
    }
};



template<typename Con>
std::vector<contFixture<Con>> generateConFix(int nnum=0)
{

    using value_type = typename Con::value_type;
    using container_template = get_template<Con>;

    std::vector<contFixture<Con>> res;

    auto a=ms::myLinspace<container_template::template type, value_type>(0.0,20.0,100);

    contFixture<Con> a1={"default",a,ioFormat::Serializable,true};
    contFixture<Con> a2={"noSize",a,ioFormat::Serializable,false};

    res.push_back(a1);
    res.push_back(a2);
    if(nnum==1)
    {

    }
    return res;
}

INSTANTIATE_TEST_SUITE_P(
        PrintParseTests,
        ParseContTs,
        ::testing::ValuesIn(generateConFix<ct::stdVec>(0)

        ), [](auto&info){return std::get<0>(info.param);});


TEST_P(ParseContTs,testSerialization)
{
    using namespace testCommon;
    auto &[_,cont,io,printSize]=GetParam();


    auto sizeopt=printSize?std::nullopt:std::optional{cont.size()};

    auto json=continuousToJson(cont,printSize);

    using colT = std::remove_const_t<decltype(cont)>;
    auto deser=parseCont<colT>(json,sizeopt);

    if(!printSize)
    {
        ASSERT_EQ(cont.size(),deser.size());
    }


    compareArrays(cont, cont, arrayDoubleComparator<FloatType>::call, anotherErr);


}


TEST_P(ParseContTs,testPrintWrite)
{
    using namespace testCommon;
    auto &[_,cont,io,printSize]=GetParam();


    auto sizeopt=printSize?std::nullopt:std::optional{cont.size()};

    printSolution(ss,cont,io,printSize);
    using colT = std::remove_const_t<decltype(cont)>;
    auto deser=parseOneDim<colT>(ss,sizeopt);

    if(!printSize)
    {
        ASSERT_EQ(cont.size(),deser.size());
    }


    compareArrays(cont, cont, arrayDoubleComparator<FloatType>::call, anotherErr);


}

