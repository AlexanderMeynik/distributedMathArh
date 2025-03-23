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

std::vector<ttype> generateFixture()
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
        lookupTests,
        MeshSerializeDeserializeTs,
        ::testing::ValuesIn(generateFixture()

        ), [](auto&info){return std::get<0>(info.param);});

