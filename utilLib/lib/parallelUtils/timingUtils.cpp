#include <sstream>
#include <iomanip>
#include <cmath>
#include "parallelUtils/timingUtils.h"

namespace timing {
std::string TpToString(timing::TimepointType system_ms) {

  using namespace std::chrono;

  microseconds ms{system_ms};

  system_clock::time_point tp{
      duration_cast<system_clock::duration>(ms)
  };

  auto epoch_us = duration_cast<microseconds>(tp.time_since_epoch()).count();
  time_t secs = epoch_us / 1'000'000;
  int frac = epoch_us % 1'000'000;

  std::tm tm{};
  localtime_r(&secs, &tm);

  std::ostringstream os;
  os << std::put_time(&tm, "%Y-%m-%d %H:%M:%S")
     << '.' << std::setw(6) << std::setfill('0') << frac;
  return os.str();
}
std::optional<TimepointType> StrToTimepoint(std::string_view ts) {
  std::tm tm{};
  int micros = 0;

  auto dot = ts.find('.');

  if (std::sscanf(ts.data(),
                  "%d-%d-%d %d:%d:%d.%d",
                  &tm.tm_year, &tm.tm_mon, &tm.tm_mday,
                  &tm.tm_hour, &tm.tm_min, &tm.tm_sec,
                  &micros) != 7)
    return std::nullopt;

  std::string_view frac_part = ts.substr(dot + 1);
  micros *= std::pow(10, 6 - frac_part.size());

  tm.tm_year -= 1900;
  tm.tm_mon -= 1;

  time_t secs = mktime(&tm);
  uint64_t total_us = uint64_t(secs) * 1'000'000 + micros;

  using namespace std::chrono;
  auto sys_dur = microseconds(total_us);

  return uint64_t(duration_cast<microseconds>(sys_dur).count());
}

bool LocationComparator::operator()(const LocationType &a, const LocationType &b) const {
  constexpr size_t sz = 5;
  for (size_t i = 0; i < sz; i++) {
    if (a[i] != b[i])
      return b[i] > a[i];
  }
  return false;
}
}
