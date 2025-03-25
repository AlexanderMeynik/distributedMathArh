#pragma once
#ifndef DIPLOM_CHRONO_CLOCK
#define DIPLOM_CHRONO_CLOCK

#include "clockArray.h"
#include <chrono>
/// chronoClock namespace
namespace chronoClock {
    /**
    * Global clock used for time measurement
    */
    using timepointType = std::chrono::system_clock::time_point;
    using timing::locationType;


    /**
    * Formats source location fields for more readability
    * @param location
    */
    locationType getFileState(std::source_location location
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
    doubleType doubleCastChrono(timepointType curr, timepointType prev);

    /**
     * @tparam chrono_duration_type
     */
    template<typename chrono_duration_type=std::milli>
    using chronoClockTemplate = timing::clockArray<int64_t, timepointType,
            &std::chrono::high_resolution_clock::now,
            &getFileState,
            &doubleCastChrono<chrono_duration_type>>;

    using clockType = chronoClockTemplate<std::milli>;
    using symbolType = char;
    extern clockType gClk;


    template<typename to_dur, typename doubleType>
    doubleType doubleCastChrono(timepointType curr, timepointType prev) {
        std::chrono::duration<doubleType, to_dur> ret =
                std::chrono::duration_cast<std::chrono::duration<doubleType, to_dur>>(curr - prev);
        return ret.count();
    }

} // namespace chronoClock

#endif  //DIPLOM_CHRONO_CLOCK
