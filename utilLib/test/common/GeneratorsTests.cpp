#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "common/Generator.h"
#include "../../computationalLib/test/GoogleCommon.h"
#include <set>

using namespace generators;
using common_types::FloatType;
using common_types::JsonVariant;



class DistributionsTest : public ::testing::Test {
 public:
  using PType=std::pair<FloatType, FloatType>;
  static PType ComputeMeanAndVariance(const DistributionFunctor &functor,
                                      size_t num_samples,
                                       const std::function<void(FloatType )>& checker
                                       = [](FloatType){})
  {
    std::vector<FloatType> values;
    for (int i = 0; i < num_samples; ++i) {
      values.push_back(functor());
      checker(values.back());
    }
    FloatType sum = 0.0;
    for (auto v : values) {
      sum += v;
    }
    FloatType mean = sum / values.size();
    FloatType variance = 0.0;
    for (auto v : values) {
      variance += (v - mean) * (v - mean);
    }
    variance /= values.size();
    return {mean, variance};
  }

 protected:
  static inline FloatType tol_ = 5;
  static inline FloatType mean_ = 12;
  static inline FloatType dev_ = 1.0;
  static inline FloatType lambda_ = 1.0;
  static inline FloatType a_ = 0.0;
  static inline FloatType b_ = 1.0;
  size_t iter_count_ = 10000;

};


using ArgumentMap= std::unordered_map<std::string, JsonVariant>;


TEST_F(DistributionsTest, NormalDistributionString) {


  auto args = ArgumentMap{{"mean",mean_ },
                          {"dev", dev_}};
  auto functor = ParseFunc("normal", args);
  auto [mean, variance] = ComputeMeanAndVariance(functor, iter_count_);
  
  EXPECT_NEAR_REL(mean_, mean, tol_) << "Mean should be close to 0 for normal distribution";
  EXPECT_NEAR_REL(dev_,variance, tol_) << "Variance should be close to 1 for normal distribution";
}

TEST_F(DistributionsTest, UniformDistributionString) {
  auto args = ArgumentMap{{"a",a_}, {"b", b_}};
  auto functor = ParseFunc("uniform", args);
  std::vector<FloatType> values;

  auto [mean,_]= ComputeMeanAndVariance(functor,iter_count_,
                                        [](FloatType val)
                                        {
                                          EXPECT_GE(val, 0.0) << "Uniform value should be >= 0";
                                          EXPECT_LE(val, 1.0) << "Uniform value should be <= 1";
                                        });
  EXPECT_NEAR_REL((a_+b_)/2, mean, tol_/2) << "Mean should be close to 0.5 for uniform [0,1]";
}

TEST_F(DistributionsTest, ExponentialDistributionString) {
  auto args = ArgumentMap{{"lambda", lambda_}};
  auto functor = ParseFunc("exponential", args);


  auto [mean,_]= ComputeMeanAndVariance(functor,iter_count_,
                                        [](FloatType val)
                                        {
                                          EXPECT_GE(val, 0.0) << "Exponential value should be non-negative";
                                        });
  EXPECT_NEAR_REL(lambda_,mean, tol_) << "Mean should be close to 1 for exponential with lambda=1";
}

TEST_F(DistributionsTest, NormalDistributionEnum) {
  FloatType mean1=2.0;
  FloatType dev1=1.5;
  auto args = ArgumentMap{{"mean", mean1}, {"dev", dev1}};
  auto functor = ParseFunc(DistributionType::NORMAL, args);
  auto [mean, variance] = ComputeMeanAndVariance(functor, iter_count_);
  EXPECT_NEAR_REL(mean1,mean , tol_) << "Mean should be close to 2 for normal distribution";
  EXPECT_NEAR_REL(dev1 * dev1,variance , tol_*2) << "Variance should be close to 2.25 for normal distribution";
}

TEST_F(DistributionsTest, InvalidDistributionType) {
  auto args = ArgumentMap{};
  EXPECT_THROW(ParseFunc("invalid_type", args), shared::InvalidOption)
            << "Should throw InvalidOption for unrecognized distribution type";
}

TEST_F(DistributionsTest, MissingArgument) {
  auto args = ArgumentMap{{"mean", 0.0}};
  EXPECT_THROW(ParseFunc("normal", args), std::out_of_range)
            << "Should throw out_of_range for missing argument";
}

TEST_F(DistributionsTest, WrongArgumentType) {
  auto args = ArgumentMap{{"mean", "not_a_number"}, {"dev", 1.0}};
  EXPECT_THROW(ParseFunc("normal", args), std::bad_variant_access)
            << "Should throw bad_variant_access for incorrect argument type";
}

TEST_F(DistributionsTest, GetNormalGenerator) {
  auto functor = get_normal_generator(mean_, dev_);
  auto [mean, variance] = ComputeMeanAndVariance(functor, iter_count_);
  EXPECT_NEAR_REL(mean_,mean,  tol_) << "Mean should be close to 0 for normal distribution";
  EXPECT_NEAR_REL(dev_,variance,  tol_) << "Variance should be close to 1 for normal distribution";
}

TEST_F(DistributionsTest, GeneratesDifferentNumbers) {
  auto functor = get_normal_generator(mean_, dev_);
  std::set<FloatType> values;
  for (int i = 0; i < iter_count_*100; ++i) {
    values.insert(functor());
  }
  EXPECT_EQ(values.size(), iter_count_*100) << "Generator should produce varied numbers";
}