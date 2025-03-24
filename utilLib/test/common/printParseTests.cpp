#include <sstream>
#include <gtest/gtest.h>

#include "common/Parsers.h"
#include "common/Printers.h"

#include "../../computationalLib/test/GoogleCommon.h"//todo make common
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