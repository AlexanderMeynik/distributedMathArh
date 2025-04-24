#include "common/Generator.h"

namespace generators {
thread_local generator<std::mt19937> gen_mt19937 = generator<std::mt19937>();

DistributionFunctor ParseFunc(const DistributionType &distribution_type,
                              const std::unordered_map<std::string, JsonVariant> &args) {
  switch (distribution_type) {
    case DistributionType::NORMAL:
      return get_normal_generator
          (
              get<FloatType>(args.at("mean")),
              get<FloatType>(args.at("dev"))
          );
    case DistributionType::UNIFORM:
      return get_uniform_generator
          (
              get<FloatType>(args.at("a")),
              get<FloatType>(args.at("b"))
          );
    case DistributionType::EXPONENTIAL:
      return get_exponential_generator(
          get<FloatType>(args.at("lambda"))
      );
    default:return []() { return 1.0; };
  }
}

DistributionFunctor ParseFunc(const std::string &type,
                              const std::unordered_map<std::string, JsonVariant> &args) {
  if (!stringToDistributionType.contains(type)) {
    throw shared::InvalidOption(type);
  }
  return ParseFunc(stringToDistributionType.at(type), args);
}
}