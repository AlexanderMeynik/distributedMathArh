#ifndef DATA_DEDUPLICATION_SERVICE_CLOCKARRAY_H
#define DATA_DEDUPLICATION_SERVICE_CLOCKARRAY_H

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
    static inline auto cmp = [](const locationType &a, const locationType &b) -> bool {
        constexpr size_t sz = 5;
        for (int i = 0; i < sz; i++) {
            if (a[i] != b[i])
                return b[i] > a[i];
        }
        return false;
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
        void reset();


        /**
         * This function starts new calculation section
         * @param location source location of calle
         * @attention You must mirror every tik like call with
         * @ref timing::ClockArray< T, T2, timeGetter, src_to_loc_type, double_cast >::tak "tak()"
         */
        void tik(const std::source_location &location
        = std::source_location::current());


        /**
         * Finishes timing for specified section and calculation double value for time
         * @param location
         */
        void tak(const std::source_location &location
        = std::source_location::current());

        /**
         * This function returns it's source location to chain several
         * compute sections into one.
         * @details Example:
         * @details auto source =clk.tikLoc();
         * @details some_func();
         * @details clk.tak();
         * @details someOtherFunc();
         * @details clk.tik(source);
         * @details some_func();
         * @details clk.tak();
         * @param location
         * @return source location
         */
        locationType tikLoc(const std::source_location &location
        = std::source_location::current()) {
            tik(location);
            return sourceTypeConverter(location);
        }

        /**
         * @param location
         * @return pair of std::source_location, location_type
         */
        std::pair<std::source_location, locationType> tikPair(const std::source_location &location
        = std::source_location::current()) {
            return std::make_pair(location, tikLoc(location));
        }

        decltype(auto) begin() const {
            return timers.begin();
        }

        decltype(auto) end() const {
            return timers.end();
        }

        auto cbegin() const {
            return timers.cbegin();
        }

        auto cend() const {
            return timers.cend();
        }

        friend std::ostream &
        operator<<(std::ostream &out,
                   const clockArray<OutType, inType, timeGetter, sourceTypeConverter, timeConverter> &ts) {
            out << "Function name\tThread\tLine\tTime\n";
            for (auto &val: ts)
                out << val.first[0] << '\t' << val.first[4] << '\t'
                    << val.first[3] << ":" << val.first[1]
                    << '\t' << val.second << '\n';
            return out;
        }


        auto &operator[](const locationType &loc) {
            return timers[loc];
        }

        [[nodiscard]] bool contains(const locationType &loc) const {
            return timers.contains(loc);
        }

        /**
         * @brief adds up all timers from other timer current timer
         * @param other
         */
        void advance(clockArray &other) {

            for (auto &otherclk: other) {
                if (this->contains(otherclk.first)) {
                    this->timers[otherclk.first].time += otherclk.second.time;
                    this->timers[otherclk.first].count += otherclk.second.count;
                } else {
                    this->timers[otherclk.first] = otherclk.second;
                }
            }
        }

    private:
        std::map<locationType, timeStore, decltype(cmp)> timers;
        std::map<locationType, inType, decltype(cmp)> startIngTimers;
        std::stack<locationType> toTak;

        static inline std::mutex s_mutex;
        using guardType = std::lock_guard<std::mutex>;
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
    clockArray<OutType, inType, timeGetter, sourceTypeConverter, timeConverter>::tak(
            const std::source_location &location) {
        const guardType guard{s_mutex};
        auto id = (*sourceTypeConverter)(location);
        if (toTak.empty() || toTak.top()[0] != id[0]) {
            std::string msg = "No paired tik statement found in queue\t"
                              "Tak values" + id[3] + ":" + id[1] + "\t"
                              + id[0] + '\t' + id[2];
            throw std::logic_error(msg);
        }
        id[1] = toTak.top()[1];
        id[2] = toTak.top()[2];
        toTak.pop();
        auto res = timeConverter((*timeGetter)(), startIngTimers[id]);
        if (!timers.contains(id)) {
            timers[id] = {res, 1};
        } else {
            timers[id].time += res;
            timers[id].count++;
        }
    }

    template<typename OutType, typename inType, inType (*timeGetter)(), locationType (*sourceTypeConverter)(
            std::source_location), OutType (*timeConverter)(inType, inType)>
    requires std::is_floating_point_v<OutType> or std::is_integral_v<OutType>void
    clockArray<OutType, inType, timeGetter, sourceTypeConverter, timeConverter>::tik(
            const std::source_location &location) {
        const guardType guard{s_mutex};
        auto id = sourceTypeConverter(location);
        startIngTimers[id] = timeGetter();

        toTak.push(id);
    }

    template<typename OutType, typename inType, inType (*timeGetter)(), locationType (*sourceTypeConverter)(
            std::source_location), OutType (*timeConverter)(inType, inType)>
    requires std::is_floating_point_v<OutType> or std::is_integral_v<OutType>void
    clockArray<OutType, inType, timeGetter, sourceTypeConverter, timeConverter>::reset() {
        if (toTak.empty()) {
            throw std::logic_error("Missing tak statements for tik ones");
            //todo serialize
        }
        this->timers.clear();
        this->startIngTimers.clear();
    }
}


#endif // DATA_DEDUPLICATION_SERVICE_CLOCKARRAY_H
