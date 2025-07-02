#pragma once

#include <ostream>
#include <map>
#include <stack>
#include <mutex>

#include "parallelUtils/timingUtils.h"

/// timing namespace
namespace timing {

template<typename OutType, typename inType, inType(*timeGetter)(), LocationType (*sourceTypeConverter)(
    std::source_location location),
    OutType(*timeConverter)(inType curr, inType prev)> requires std::is_floating_point_v<OutType>
    or std::is_integral_v<OutType>
class ClockArray;

/**
  * @tparam OutType double type tha will be printed
  * @tparam inType Type that timeGetter return
  * @tparam timeGetter function that return current time
  * @tparam sourceTypeConverter function that converts source location to inner representation of it
  * @tparam timeConverter a function that casts time difference to type
  * @brief Clock array template class
 */
template<typename OutType, typename inType, inType(*timeGetter)(), LocationType (*sourceTypeConverter)(
    std::source_location location),
    OutType(*timeConverter)(inType curr, inType prev)> requires std::is_floating_point_v<OutType>
    or std::is_integral_v<OutType>
class ClockArray {
 public:
  struct timeStore {
    OutType time;
    size_t count;

    friend std::ostream &operator<<(std::ostream &out, const timeStore &ts) {
      out << ts.time;
      return out;
    }
  };

  /**
   * Resets timers and converted double values
   */
  void Reset();

  /**
   * @brief resets timer value for one timer
   */
  void ResetTimer(const LocationType &location);

  /**
   * This function starts new calculation section
   * @param location source location of call
   * @attention You must mirror every Tik like call with Tak
   */
  void Tik(const std::source_location &location
  = std::source_location::current());

  /**
   * Finishes timing for specified section and calculation double value for time
   * @param location
   */
  void Tak(const std::source_location &location
  = std::source_location::current());

  /**
   * This function returns it's source location to chain several
   * compute sections into one.
   * Code example:
   * \code
   * auto source =clk.TikLoc() //get recorded location
   * some_func();
   * clk.Tak();        //record function time
   * someOtherFunc();  //won't record
   * clk.Tik(source);   //continue recording for second function
   * some_func2();
   * clk.Tak();    //record function time
   * \endcode
   * @param location
   * @return source location
   */
  LocationType TikLoc(const std::source_location &location
  = std::source_location::current()) {
    Tik(location);
    return sourceTypeConverter(location);
  }

  /**
   * @param location
   * @return pair of std::source_location, location_type
   */
  std::pair<std::source_location, LocationType> TikPair(const std::source_location &location
  = std::source_location::current()) {
    return std::make_pair(location, TikLoc(location));
  }

  decltype(auto) begin() const {
    return timers_.begin();
  }

  decltype(auto) end() const {
    return timers_.end();
  }

  auto cbegin() const {
    return timers_.cbegin();
  }

  auto cend() const {
    return timers_.cend();
  }

  friend std::ostream &
  operator<<(std::ostream &out,
             const ClockArray<OutType, inType, timeGetter, sourceTypeConverter, timeConverter> &ts) {
    out << "Function name\tThread\tLine\tTime\n";
    for (auto &val : ts)
      out << val.first[0] << '\t' << val.first[4] << '\t'
          << val.first[3] << ":" << val.first[1]
          << '\t' << val.second << '\n';
    return out;
  }

  auto &operator[](const LocationType &loc) {
    return timers_[loc];
  }

  [[nodiscard]] bool contains(const LocationType &loc) const;

  /**
   * @brief adds up all timers from other timer current timer
   * @param other
   */
  void advance(ClockArray &other);

 private:
  std::map<LocationType, timeStore, LocationComparator> timers_;
  std::map<LocationType, inType, LocationComparator> start_ing_timers_;
  std::stack<LocationType> to_tak_;

  static inline std::mutex s_mutex_;
  using GuardType = std::scoped_lock<std::mutex>;
};

}

namespace timing {
template<typename OutType, typename inType, inType (*timeGetter)(), LocationType (*sourceTypeConverter)(
    std::source_location), OutType (*timeConverter)(inType, inType)>
requires std::is_floating_point_v<OutType> or std::is_integral_v<OutType>void
ClockArray<OutType, inType, timeGetter, sourceTypeConverter, timeConverter>::Tak(
    const std::source_location &location) {

  auto new_time = (*timeGetter)();
  GuardType kGuard{s_mutex_};
  auto id = (*sourceTypeConverter)(location);
  if (to_tak_.empty() || to_tak_.top()[0] != id[0]) {
    std::string msg = "No paired Tik statement found in queue\t"
                      "Tak values" + id[3] + ":" + id[1] + "\t"
        + id[0] + '\t' + id[2];
    throw std::logic_error(msg);
  }
  id[1] = to_tak_.top()[1];
  id[2] = to_tak_.top()[2];
  to_tak_.pop();
  auto res = timeConverter(new_time, start_ing_timers_[id]);
  if (!timers_.contains(id)) {
    timers_[id] = {res, 1};
  } else {
    timers_[id].time += res;
    timers_[id].count++;
  }
}

template<typename OutType, typename inType, inType (*timeGetter)(), LocationType (*sourceTypeConverter)(
    std::source_location), OutType (*timeConverter)(inType, inType)>
requires std::is_floating_point_v<OutType> or std::is_integral_v<OutType>void
ClockArray<OutType, inType, timeGetter, sourceTypeConverter, timeConverter>::Tik(
    const std::source_location &location) {
  GuardType kGuard{s_mutex_};
  auto id = sourceTypeConverter(location);
  start_ing_timers_[id] = timeGetter();

  to_tak_.push(id);
}

template<typename OutType, typename inType, inType (*timeGetter)(), LocationType (*sourceTypeConverter)(
    std::source_location), OutType (*timeConverter)(inType, inType)>
requires std::is_floating_point_v<OutType> or std::is_integral_v<OutType>void
ClockArray<OutType, inType, timeGetter, sourceTypeConverter, timeConverter>::Reset() {
  GuardType k_guard{s_mutex_};
  if (!to_tak_.empty()) {
    throw std::logic_error("Missing Tak statements for Tik ones");
  }
  this->timers_.clear();
  this->start_ing_timers_.clear();
}

template<typename OutType, typename inType, inType (*timeGetter)(), LocationType (*sourceTypeConverter)(std::source_location), OutType (*timeConverter)(
    inType,
    inType)>
requires std::is_floating_point_v<OutType>
    or std::is_integral_v<OutType>void ClockArray<OutType,
                                                  inType,
                                                  timeGetter,
                                                  sourceTypeConverter,
                                                  timeConverter>::ResetTimer(const LocationType &location) {
  const GuardType kGuard{s_mutex_};

  if (timers_.contains(location)) {
    timers_.erase(location);
  }
}
template<typename OutType, typename inType, inType (*timeGetter)(), LocationType (*sourceTypeConverter)(std::source_location), OutType (*timeConverter)(
    inType,
    inType)>
requires std::is_floating_point_v<OutType>
    or std::is_integral_v<OutType>bool ClockArray<OutType,
                                                  inType,
                                                  timeGetter,
                                                  sourceTypeConverter,
                                                  timeConverter>::contains(const LocationType &loc) const {
  return timers_.contains(loc);
}
template<typename OutType, typename inType, inType (*timeGetter)(), LocationType (*sourceTypeConverter)(std::source_location), OutType (*timeConverter)(
    inType,
    inType)>
requires std::is_floating_point_v<OutType>
    or std::is_integral_v<OutType>void ClockArray<OutType,
                                                  inType,
                                                  timeGetter,
                                                  sourceTypeConverter,
                                                  timeConverter>::advance(ClockArray &other) {

  for (auto &other_clk : other) {
    if (this->contains(other_clk.first)) {
      this->timers_[other_clk.first].time += other_clk.second.time;
      this->timers_[other_clk.first].count += other_clk.second.count;
    } else {
      this->timers_[other_clk.first] = other_clk.second;
    }
  }
}

}