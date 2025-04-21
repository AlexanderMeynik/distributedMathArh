#include "parallelUtils/chronoClock.h"

#include <sstream>
/// chrono_clock namespace
namespace chrono_clock {

clockType gClk{};

locationType GetFileState(std::source_location location) {
  std::string name = location.function_name();
  auto id = name.find(' ');
  auto id2 = name.find('(');

  std::string fname = location.file_name();
  std::stringstream ss;
  ss << std::this_thread::get_id();
  return {name.substr(id + 1, id2 - id - 1),
          std::to_string(location.line()),
          std::to_string(location.column()),
          fname.substr(fname.rfind('/') + 1),
          ss.str()};
}// namespace chronoClock

}