#pragma once

#include <chrono>
#include <string>
#include <optional>
#include <source_location>

#include "common/myConcepts.h"

/// timing namespace
namespace timing {
using my_concepts::is_specialization_of;

/**
* Global clock used for range measurement
*/
using ClockType = typename std::conditional<std::chrono::high_resolution_clock::is_steady,
                                            std::chrono::high_resolution_clock,
                                            std::chrono::steady_clock>::type;

using ClockTimepoint = ClockType::time_point;
using TimepointType = uint64_t;///< default portable time-point type
using LocationType = std::array<std::string, 5>;///< structure to store file data


/**
 * @brief Array printer
 * @tparam T
 * @tparam sz
 * @param out
 * @param arr
 * @return
 */
template<typename T, size_t sz>
requires std::is_floating_point_v<T> or std::is_integral_v<T>
std::ostream &operator<<(std::ostream &out, std::array<T, sz> &arr);

template<typename D>
concept Duration = is_specialization_of<D, std::chrono::duration>::value;

/**
 * @brief Returns now with specified precision
 * @tparam dur
 * @return
 */
template<Duration dur>
TimepointType inline Now() {
  return duration_cast<dur>(std::chrono::system_clock::now().time_since_epoch())
      .count();
}

/**
 * @brief Array comparator
 * @tparam T
 * @tparam sz
 * @param arr1
 * @param arr2
 */
template<typename T, size_t sz>
bool operator==(const std::array<T, sz> &arr1, const std::array<T, sz> &arr2) {
  return std::equal(arr1.begin(), arr1.end(), arr2);
}

/**
 * Comparator for locationType
 */
struct LocationComparator {
  bool operator()(const LocationType &a, const LocationType &b) const;
};

/**
 * @brief Serializes given timepoint to string
 * @note uses localtime_r when forming time
 * @param system_ms
 */
std::string TpToString(TimepointType system_ms);

/**
 * @brief Retrives TimepointType from a given string
 * @note uses mktime to get timepoint representation
 * @param ts
 */
std::optional<TimepointType> StrToTimepoint(std::string_view ts);
}

namespace timing {
template<typename T, size_t sz>
requires std::is_convertible_v<T, std::string>
std::ostream &operator<<(std::ostream &out, std::array<T, sz> &arr) {
  int i = 0;
  for (; i < sz - 1; ++i) {
    out << arr[i] << '\t';
  }
  out << arr[i] << '\n';
  return out;
}
}