#pragma once

#include <random>
#include <functional>
#include <thread>

#include "common/commonTypes.h"
#include "common/myConcepts.h"

using common_types::FloatType;
namespace generators {

/**
 * @brief Interface for distribution functions handling
 * @tparam randomDevice
 */
template<typename randomDevice>
struct generator {
  generator() {
    std::random_device rd;
    auto seed = rd() ^ std::hash<std::thread::id>{}(std::this_thread::get_id());
    device = randomDevice(seed);
  }


  template<template<typename...> typename distribution, typename... Args>
  auto GetGenFunction(Args&&... args) {
    return [this,args...]() mutable
    {
      return distribution<FloatType>(std::forward<Args>(args)...)(device);
    };
  }

 private:
  randomDevice device;
};

/// Thread local generator object
extern thread_local generator<std::mt19937> gen_mt19937;


/**
 * @brief Shorthand generator getter for normal distribution
 * @param mean
 * @param dev
 */
auto inline get_normal_generator(FloatType mean, FloatType dev) {
  return gen_mt19937.GetGenFunction<std::normal_distribution>(mean, dev);
}

/**
 * @brief Shorthand generator getter for uniform real distribution
 * @param a
 * @param b
 */
auto inline get_uniform_generator(FloatType a, FloatType b) {
  return gen_mt19937.GetGenFunction<std::uniform_real_distribution>(a, b);
}

/**
 * @brief Shorthand generator getter for exponential distribution
 * @param lambda
 * @return
 */
auto inline get_exponential_generator(FloatType lambda) {
  return gen_mt19937.GetGenFunction<std::exponential_distribution>(lambda);
}



}

