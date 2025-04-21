#pragma once

#include <array>
#include <ostream>
#include <map>
#include <stack>
#include <source_location>
#include <thread>
#include <concepts>

template<typename T, size_t sz>
requires std::is_floating_point_v<T> or std::is_integral_v<T>
std::ostream &operator<<(std::ostream &out, std::array<T, sz> &arr);

//todo make more verbose and move
template<typename T, size_t sz>
bool operator==(const std::array<T, sz> &arr1, const std::array<T, sz> &arr2) {
  return std::equal(arr1.begin(), arr1.end(), arr2);
}
/// timing namespace
namespace timing {
using locationType = std::array<std::string, 5>;

/**
 * Comparator for locationType
 */
struct LocationComparator {
  bool operator()(const locationType& a, const locationType& b) const {
    constexpr size_t sz = 5;
    for (size_t i = 0; i < sz; i++) {
      if (a[i] != b[i])
        return b[i] > a[i];
    }
    return false;
  }
};

template<typename OutType, typename inType, inType(*timeGetter)(), locationType (*sourceTypeConverter)(
    std::source_location location),
    OutType(*timeConverter)(inType curr, inType prev)> requires std::is_floating_point_v<OutType>
    or std::is_integral_v<OutType>
class clockArray;

/**
  * @tparam OutType double type tha will be printed
  * @tparam inType Type that timeGetter return
  * @tparam timeGetter function that return current time
  * @tparam sourceTypeConverter function that converts source location to inner representation of it
  * @tparam timeConverter a function that casts time difference to type
  * @brief Clock array template class
 */
template<typename OutType, typename inType, inType(*timeGetter)(), locationType (*sourceTypeConverter)(
    std::source_location location),
    OutType(*timeConverter)(inType curr, inType prev)> requires std::is_floating_point_v<OutType>
    or std::is_integral_v<OutType>
class clockArray {
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
  void ResetTimer(const locationType &location) {
    const GuardType kGuard{s_mutex_};
    //auto id = sourceTypeConverter(location);

    if (timers_.contains(location)) {
      timers_.erase(location);
    }
  }

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
  locationType TikLoc(const std::source_location &location
  = std::source_location::current()) {
    Tik(location);
    return sourceTypeConverter(location);
  }

  /**
   * @param location
   * @return pair of std::source_location, location_type
   */
  std::pair<std::source_location, locationType> TikPair(const std::source_location &location
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
             const clockArray<OutType, inType, timeGetter, sourceTypeConverter, timeConverter> &ts) {
    out << "Function name\tThread\tLine\tTime\n";
    for (auto &val : ts)
      out << val.first[0] << '\t' << val.first[4] << '\t'
          << val.first[3] << ":" << val.first[1]
          << '\t' << val.second << '\n';
    return out;
  }

  auto &operator[](const locationType &loc) {
    return timers_[loc];
  }

  [[nodiscard]] bool contains(const locationType &loc) const {
    return timers_.contains(loc);
  }

  /**
   * @brief adds up all timers from other timer current timer
   * @param other
   */
  void advance(clockArray &other) {

    for (auto &otherclk : other) {
      if (this->contains(otherclk.first)) {
        this->timers_[otherclk.first].time += otherclk.second.time;
        this->timers_[otherclk.first].count += otherclk.second.count;
      } else {
        this->timers_[otherclk.first] = otherclk.second;
      }
    }
  }

 private:
  std::map<locationType, timeStore,LocationComparator> timers_;
  std::map<locationType, inType, LocationComparator> start_ing_timers_;
  std::stack<locationType> to_tak_;

  static inline std::mutex s_mutex_;
  using GuardType = std::lock_guard<std::mutex>;
};

}

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

namespace timing {
template<typename OutType, typename inType, inType (*timeGetter)(), locationType (*sourceTypeConverter)(
    std::source_location), OutType (*timeConverter)(inType, inType)>
requires std::is_floating_point_v<OutType> or std::is_integral_v<OutType>void
clockArray<OutType, inType, timeGetter, sourceTypeConverter, timeConverter>::Tak(
    const std::source_location &location) {

  auto new_time = (*timeGetter)();
  const GuardType kGuard{s_mutex_};
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

template<typename OutType, typename inType, inType (*timeGetter)(), locationType (*sourceTypeConverter)(
    std::source_location), OutType (*timeConverter)(inType, inType)>
requires std::is_floating_point_v<OutType> or std::is_integral_v<OutType>void
clockArray<OutType, inType, timeGetter, sourceTypeConverter, timeConverter>::Tik(
    const std::source_location &location) {
  const GuardType kGuard{s_mutex_};
  auto id = sourceTypeConverter(location);
  start_ing_timers_[id] = timeGetter();

  to_tak_.push(id);
}

template<typename OutType, typename inType, inType (*timeGetter)(), locationType (*sourceTypeConverter)(
    std::source_location), OutType (*timeConverter)(inType, inType)>
requires std::is_floating_point_v<OutType> or std::is_integral_v<OutType>void
clockArray<OutType, inType, timeGetter, sourceTypeConverter, timeConverter>::Reset() {
  const GuardType kGuard{s_mutex_};
  if (!to_tak_.empty()) {
    throw std::logic_error("Missing Tak statements for Tik ones");
  }
  this->timers_.clear();
  this->start_ing_timers_.clear();
}
}
