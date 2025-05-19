#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <iosfwd>
#include "errorHandling.h"

namespace enum_utils {
using shared::InvalidOption;

/**
 * @brief Structure to store enum mapping to some other class
 * @tparam Enum
 */
template<typename Enum,typename To> requires std::is_enum_v<Enum>
struct EnumDoubleMapping {
  Enum value;
  To to;
};
/**
 * @brief Structure to store enum mapping
 * @tparam Enum
 */
template<typename Enum> requires std::is_enum_v<Enum>
using EnumMapping=EnumDoubleMapping<Enum,std::string>;


template<typename Enum,typename From> requires std::is_enum_v<Enum>
using MapFromTypeToEnum = std::unordered_map<From, Enum>;

template<typename Enum,typename To> requires std::is_enum_v<Enum>
using MapEnumToType = std::unordered_map<Enum, To>;


/**
 * Generate map from From to Enum
 * @tparam Enum
 * @tparam From
 * @param mappings
 */
template<typename Enum,typename From>
requires std::is_enum_v<Enum>
MapFromTypeToEnum<Enum, From>
CreateFromTypeToEnumMap(const std::vector<EnumDoubleMapping<Enum,From>> &mappings);

/**
 * @brief Generate map from Enum to To type map
 * @tparam Enum
 * @tparam To
 * @param mappings
 */
template<typename Enum,typename To>
requires std::is_enum_v<Enum>
MapEnumToType<Enum, To> CreateEnumToTypeMap(const std::vector<EnumDoubleMapping<Enum,To>> &mappings);


/**
 * @brief Generate map from Str to enum value
 * @tparam Enum
 * @param mappings
 */
template<typename Enum>
requires std::is_enum_v<Enum>
MapFromTypeToEnum<Enum, std::string>
    CreateStrToEnumMap(const std::vector<EnumMapping<Enum>> &mappings);

/**
 * Generate map from enum value to Str
 * @tparam Enum
 * @param mappings
 * @return
 */
template<typename Enum>
requires std::is_enum_v<Enum>
MapEnumToType<Enum, std::string> CreateEnumToStrMap(const std::vector<EnumMapping<Enum>> &mappings);


/**
 * @brief Casts From type value to enum
 * @tparam Enum
 * @tparam From
 * @param str
 * @param map
 * @return Enum value
 */
template<typename Enum,typename From>
requires std::is_enum_v<Enum>
Enum inline FromToEnum(const From str,
                       const MapFromTypeToEnum<Enum,From> &map) {
  auto it = map.find(str);
  if (it == map.end()) {
    throw InvalidOption(str);
  }
  return it->second;
}

/**
 * @brief Casts Enum Value To typed value
 * @tparam Enum
 * @tparam To
 * @param value
 * @param map
 * @return To value
 */
template<typename Enum,typename To>
requires std::is_enum_v<Enum>
To inline EnumToType(Enum value,
                     const std::unordered_map<Enum, To> &map) {
  auto it = map.find(value);
  return it->second;
}

/**
 * @brief Cast string to enum
 * @tparam Enum
 * @param str
 * @param map
 * @return Enum value
 */
template<typename Enum>
requires std::is_enum_v<Enum>
Enum inline StrToEnum(const std::string &str,
                      const std::unordered_map<std::string, Enum> &map) {
  return FromToEnum(str,map);
}

/**
 * @brief Casts enum value to string
 * @tparam Enum
 * @param value
 * @param map
 * @return std::string
 */
template<typename Enum>
requires std::is_enum_v<Enum>
std::string inline EnumToStr(Enum value,
                             const std::unordered_map<Enum, std::string> &map) {
  return EnumToType(value,map);
}

}

namespace enum_utils {


template<typename Enum,typename From>
requires std::is_enum_v<Enum>
MapFromTypeToEnum<Enum, From>
CreateFromTypeToEnumMap(const std::vector<EnumDoubleMapping<Enum,From>> &mappings)
{
  MapFromTypeToEnum<Enum, From> map;
  for (const auto &mapping : mappings) {
    map[mapping.to] = mapping.value;
  }
  return map;
}

template<typename Enum,typename To>
requires std::is_enum_v<Enum>
MapEnumToType<Enum, To> CreateEnumToTypeMap(const std::vector<EnumDoubleMapping<Enum,To>> &mappings)
{
  MapEnumToType<Enum, To> map;
  for (const auto &mapping : mappings) {
    map[mapping.value] = mapping.to;
  }
  return map;
}

template<typename Enum>
requires std::is_enum_v<Enum>
MapFromTypeToEnum<Enum, std::string> CreateStrToEnumMap(const std::vector<EnumMapping<Enum>> &mappings) {
  return CreateFromTypeToEnumMap(mappings);
}

template<typename Enum>
requires std::is_enum_v<Enum>
MapEnumToType<Enum, std::string> CreateEnumToStrMap(const std::vector<EnumMapping<Enum>> &mappings) {
  return CreateEnumToTypeMap(mappings);
}
}