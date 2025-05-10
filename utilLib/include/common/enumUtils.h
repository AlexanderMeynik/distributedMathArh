#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <iosfwd>
#include "errorHandling.h"

namespace enum_utils {
using shared::InvalidOption;
/**
 * @brief Structure to store enum mapping
 * @tparam Enum
 */
template<typename Enum> requires std::is_enum_v<Enum>
struct EnumMapping {
  Enum value;
  std::string str;
};
template<typename Enum> requires std::is_enum_v<Enum>
using MapStrToEnum = std::unordered_map<std::string, Enum>;

template<typename Enum> requires std::is_enum_v<Enum>
using MapEnumToStr = std::unordered_map<Enum, std::string>;

/**
 * @brief Generate map from Str to enum value
 * @tparam Enum
 * @param mappings
 */
template<typename Enum>
requires std::is_enum_v<Enum>
MapStrToEnum<Enum> createStrToEnumMap(const std::vector<EnumMapping<Enum>> &mappings);

/**
 * Generate map from enum value to Str
 * @tparam Enum
 * @param mappings
 * @return
 */
template<typename Enum>
requires std::is_enum_v<Enum>
MapEnumToStr<Enum> createEnumToStrMap(const std::vector<EnumMapping<Enum>> &mappings);

/**
 * @brief P
 * @tparam Enum
 * @param str
 * @param map
 * @return
 */
template<typename Enum>
requires std::is_enum_v<Enum>
Enum inline strToEnum(const std::string &str, const std::unordered_map<std::string, Enum> &map) {
  auto it = map.find(str);
  if (it == map.end()) {
    throw InvalidOption(str);
  }
  return it->second;
}

template<typename Enum>
requires std::is_enum_v<Enum>
std::string inline enumToStr(Enum value, const std::unordered_map<Enum, std::string> &map) {
  auto it = map.find(value);
  return it->second;
}

}

namespace enum_utils {
template<typename Enum>
requires std::is_enum_v<Enum>
MapStrToEnum<Enum> createStrToEnumMap(const std::vector<EnumMapping<Enum>> &mappings) {
  MapStrToEnum<Enum> map;
  for (const auto &mapping : mappings) {
    map[mapping.str] = mapping.value;
  }
  return map;
}

template<typename Enum>
requires std::is_enum_v<Enum>
MapEnumToStr<Enum> createEnumToStrMap(const std::vector<EnumMapping<Enum>> &mappings) {
  MapEnumToStr<Enum> map;
  for (const auto &mapping : mappings) {
    map[mapping.value] = mapping.str;
  }
  return map;
}
}