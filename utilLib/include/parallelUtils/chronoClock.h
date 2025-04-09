#pragma once

#include "clockArray.h"
#include <chrono>
/// chronoClock namespace
namespace chrono_clock {
/**
* Global clock used for time measurement
*/

using ClockType = typename std::conditional<std::chrono::high_resolution_clock::is_steady,
                                            std::chrono::high_resolution_clock,
                                            std::chrono::steady_clock >::type ;

using TimepointType = ClockType::time_point/*::chrono::system_clock::time_point*/;
using timing::locationType;


/**
* Formats source location fields for more readability
* @param location
*/
locationType GetFileState(std::source_location location
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
doubleType DoubleCastChrono(TimepointType curr, TimepointType prev);

/**
 * @tparam chrono_duration_type
 */
template<typename chrono_duration_type=std::milli>
using ChronoClockTemplate = timing::clockArray<int64_t, TimepointType,
                                               &ClockType::now,
                                               &GetFileState,
                                               &DoubleCastChrono<chrono_duration_type>>;

using clockType = ChronoClockTemplate<std::milli>;
extern clockType gClk;

template<typename to_dur, typename doubleType>
doubleType DoubleCastChrono(TimepointType curr, TimepointType prev) {
  std::chrono::duration<doubleType, to_dur> ret =
      std::chrono::duration_cast<std::chrono::duration<doubleType, to_dur>>(curr - prev);
  return ret.count();
}

} // namespace chronoClock

