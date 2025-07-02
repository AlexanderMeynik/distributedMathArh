#pragma once

#include "ClockArray.h"

/// chronoClock namespace
namespace chrono_clock {

using namespace timing;
/**
* Formats source location fields for more readability
* @param location
*/
LocationType GetFileState(std::source_location location
= std::source_location::current());

/**
 * Cast timepointType difference to double
 * @tparam to_dur
 * @tparam doubleType
 * @param curr
 * @param prev
 * @return
 */
template<typename to_dur = std::milli, typename doubleType>
doubleType DoubleCastChrono(ClockTimepoint curr, ClockTimepoint prev);

/**
 * @tparam chrono_duration_type
 */
template<typename chrono_duration_type=std::milli>
using ChronoClockTemplate = timing::ClockArray<int64_t, ClockTimepoint,
                                               &ClockType::now,
                                               &GetFileState,
                                               &DoubleCastChrono<chrono_duration_type>>;

template<typename to_dur, typename doubleType>
doubleType DoubleCastChrono(ClockTimepoint curr, ClockTimepoint prev) {
  std::chrono::duration<doubleType, to_dur> ret =
      std::chrono::duration_cast<std::chrono::duration<doubleType, to_dur>>(curr - prev);
  return ret.count();
}

} // namespace chronoClock

