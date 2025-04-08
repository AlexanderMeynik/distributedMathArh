#pragma once

#include <random>
#include <functional>

#include "common/commonTypes.h"
#include "common/myConcepts.h"

using common_types::FloatType;
namespace generators {

template<typename randomDevice>
struct generator {
  generator() {
    std::random_device rd;
    device = randomDevice(rd());
  }

  template<template<typename ...> typename cont,
      template<typename...> typename distribution, typename... Args>
  requires my_concepts::HasBracketOperator<cont<FloatType>>
  auto Generate(size_t n, Args &...args) {
    auto dist = distribution<FloatType>(args...);
    cont<FloatType> container(2 * n);

    for (size_t i = 0; i < 2 * n; ++i) {
      container[i] = dist(device);
    }
    return container;

  }

 private:
  randomDevice device;
};

extern generator<std::mt19937> gen_mt19937;

template<template<typename ...> typename cont>
std::function<cont<FloatType>(size_t, FloatType, FloatType)>
    normal = [](size_t N, FloatType mean, FloatType sig) {
  return gen_mt19937.Generate<cont, std::normal_distribution, FloatType, FloatType>(N, mean, sig);
};

template<template<typename ...> typename cont>
std::function<cont<FloatType>(size_t, FloatType, FloatType)>
    uniform = [](size_t N, FloatType a, FloatType b) {
  return gen_mt19937.Generate<cont, std::uniform_real_distribution, FloatType, FloatType>(N, a, b);
};

template<template<typename ...> typename cont>
std::function<cont<FloatType>(size_t, FloatType)>
    exponential = [](size_t N, FloatType lam) {
  return gen_mt19937.Generate<cont, std::exponential_distribution, FloatType>(N, lam);
};

//todo generate array of arrays(or use md span)

}

