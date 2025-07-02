#include <sstream>
#include <gtest/gtest.h>

#include "common/Parsers.h"
#include "common/Printers.h"

#include "testingUtils/GoogleCommon.h"

using namespace test_common;
using namespace print_utils;

using Ttype = std::tuple<std::string, mesh_storage::MeshCreator, IoFormat, bool, bool>;

class MeshSerializeDeserializeTs : public ::testing::TestWithParam<Ttype> {
 public:

 protected:
  static inline FloatType another_err_ = 1e-14;
};

std::vector<Ttype> GenerateFixture(int nnum = 0) {
  std::vector<Ttype> res;

  mesh_storage::MeshCreator ms;
  ms.ConstructMeshes({8ul, 15ul}, {2, 10.0, 5, 20.0});

  ms.ApplyFunction([](auto x, auto y) {
    return x + y * 100;
  });

  Ttype a1 = Ttype{"default", ms, IoFormat::SERIALIZABLE, true, true};
  Ttype a2 = Ttype{"noDims", ms, IoFormat::SERIALIZABLE, true, false};
  Ttype a3 = Ttype{"noLims", ms, IoFormat::SERIALIZABLE, false, true};
  Ttype a4 = Ttype{"noDimsNoLims", ms, IoFormat::SERIALIZABLE, false, false};

  res.push_back(a1);
  res.push_back(a2);
  res.push_back(a3);
  res.push_back(a4);

  if (nnum == 1) {
    Ttype a5 = Ttype{"default_HR", ms, IoFormat::HUMAN_READABLE, true, true};
    Ttype a6 = Ttype{"noDims_HR", ms, IoFormat::HUMAN_READABLE, true, false};
    Ttype a7 = Ttype{"noLims_HR", ms, IoFormat::HUMAN_READABLE, false, true};
    Ttype a8 = Ttype{"noDimsNoLims_HR", ms, IoFormat::HUMAN_READABLE, false, false};

    res.push_back(a5);
    /*res.push_back(a6);
    res.push_back(a7);
    res.push_back(a8);*/
  }

  return res;
}

TEST_P(MeshSerializeDeserializeTs, testJsonSerialization) {
  using namespace test_common;
  auto &[_, mesh, io, print_dims, print_lims] = GetParam();

  auto dimopt = print_dims ? std::nullopt : std::optional{mesh.dimensions_};
  auto limopt = print_lims ? std::nullopt : std::optional{mesh.limits_};

  auto ser = ToJson(mesh, print_dims, print_lims);

  auto deser = FromJson(ser, dimopt, limopt);

  if (!print_dims) {

    CompareArrays(mesh.dimensions_, deser.dimensions_, arrayEqualComparator<size_t>::call, another_err_);
  }

  if (!print_lims) {
    CompareArrays(mesh.limits_, deser.limits_, arrayDoubleComparator<FloatType>::call, another_err_);
  }
  CompareArrays(mesh.data_[2], deser.data_[2], arrayDoubleComparator<FloatType>::call, another_err_);

}

INSTANTIATE_TEST_SUITE_P(
    SerDeserJson,
    MeshSerializeDeserializeTs,
    ::testing::ValuesIn(GenerateFixture()

    ), [](auto &info) { return std::get<0>(info.param); });

class MeshPrintReadTs : public ::testing::TestWithParam<Ttype> {
 public:

  MeshPrintReadTs() {
    ss_.precision(kDefaultPrec + 1);
  }

 protected:
  static inline FloatType another_err_ = 1e-14;
  static inline std::stringstream ss_ = std::stringstream();

  void TearDown() override {
    ss_.str("");
    ss_.clear();
  }
};

INSTANTIATE_TEST_SUITE_P(
    PrintParseTests,
    MeshPrintReadTs,
    ::testing::ValuesIn(GenerateFixture(1)

    ), [](auto &info) { return std::get<0>(info.param); });

TEST_P(MeshPrintReadTs, testOstreamPrint) {
  using namespace test_common;
  auto &[_, mesh, io, print_dims, print_lims] = GetParam();

  auto dimopt = print_dims ? std::nullopt : std::optional{mesh.dimensions_};
  auto limopt = print_lims ? std::nullopt : std::optional{mesh.limits_};

  PrintMesh(ss_, mesh, io, print_dims, print_lims);
  auto deser = ParseMeshFrom(ss_, io, dimopt, limopt);

  if (!print_dims) {
    CompareArrays(mesh.dimensions_, deser.dimensions_, arrayEqualComparator<size_t>::call, another_err_);
  }

  if (!print_lims) {
    CompareArrays(mesh.limits_, deser.limits_, arrayDoubleComparator<FloatType>::call, another_err_);
  }
  CompareArrays(mesh.data_[2], deser.data_[2], arrayDoubleComparator<FloatType>::call, another_err_);

}

template<typename Col>
using ContFixture = std::tuple<std::string, Col, IoFormat, bool>;
using ContTypes = ::testing::Types<ct::StdVec, ct::EigenVec, ct::MeshStorageType>;

using ConT = ContFixture<ct::StdVec>;

class ParseContTs : public ::testing::TestWithParam<ConT> {
 public:
  ParseContTs() {
    ss_.precision(kDefaultPrec + 1);
  }

 protected:
  static inline FloatType another_err_ = 1e-14;

  static inline std::stringstream ss_ = std::stringstream();

  void TearDown() override {
    ss_.str("");
    ss_.clear();
  }
};

template<typename Con>
std::vector<ContFixture<Con>> GenerateConFix(int nnum = 0) {

  using ValueType = typename Con::value_type;
  using ContainerTemplate = get_template<Con>;

  std::vector<ContFixture<Con>> res;

  auto a = ms::MyLinspace<ContainerTemplate::template Type, ValueType>(0.0, 20.0, 100);

  ContFixture<Con> a1 = {"default", a, IoFormat::SERIALIZABLE, true};
  ContFixture<Con> a2 = {"noSize", a, IoFormat::SERIALIZABLE, false};

  res.push_back(a1);
  res.push_back(a2);
  if (nnum == 1) {
    ContFixture<Con> a3 = {"humanReadable", a, IoFormat::HUMAN_READABLE, true};
    res.push_back(a3);
  }
  return res;
}

INSTANTIATE_TEST_SUITE_P(
    PrintParseTests,
    ParseContTs,
    ::testing::ValuesIn(GenerateConFix<ct::StdVec>(1)

    ), [](auto &info) { return std::get<0>(info.param); });

TEST_P(ParseContTs, testSerialization) {
  using namespace test_common;
  auto &[_, cont, io, print_size] = GetParam();

  auto sizeopt = print_size ? std::nullopt : std::optional{cont.size()};

  auto json = ContinuousToJson(cont, print_size);

  using ColT = std::remove_const_t<decltype(cont)>;
  my_concepts::isOneDimensionalContinuous auto deser = JsonToContinuous<ColT>(json, sizeopt);

  if (!print_size) {
    ASSERT_EQ(cont.size(), deser.size());
  }

  CompareArrays(cont, cont, arrayDoubleComparator<FloatType>::call, another_err_);
}

TEST_P(ParseContTs, testSolutionPrintParse) {
  using namespace test_common;
  auto &[_, cont, io, print_size] = GetParam();

  auto sizeopt = print_size ? std::nullopt : std::optional{cont.size()};

  PrintSolution(ss_, cont, io, print_size);
  using ColT = std::remove_const_t<decltype(cont)>;

  my_concepts::isOneDimensionalContinuous auto deser = ParseSolution<ColT>(ss_, io, sizeopt);

  if (!print_size) {
    ASSERT_EQ(cont.size(), deser.size());
  }

  CompareArrays(cont, deser, arrayDoubleComparator<FloatType>::call, another_err_);

}

TEST_P(ParseContTs, testCoordsPrintParse) {
  using namespace test_common;
  auto &[_, cont, io, print_size] = GetParam();

  auto sizeopt = print_size ? std::nullopt : std::optional{cont.size()};

  PrintCoordinates(ss_, cont, io, print_size);
  using ColT = std::remove_const_t<decltype(cont)>;
  my_concepts::isOneDimensionalContinuous auto deser = ParseCoordinates<ColT>(ss_, io, sizeopt);

  if (!print_size) {
    ASSERT_EQ(cont.size(), deser.size());
  }

  CompareArrays(cont, deser, arrayDoubleComparator<FloatType>::call, another_err_);

}

