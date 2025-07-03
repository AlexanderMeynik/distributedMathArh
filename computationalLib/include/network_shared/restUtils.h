#pragma once

#include <memory>

#include "common/errorHandling.h"
#include "common/myConcepts.h"
#include "network_shared/networkTypes.h"

#include <json/json.h>
#include <curl/curl.h>

/// Namespace for httpr rest utilities
namespace rest_utils {
using my_concepts::HasReserve;
using namespace network_types;

///Forward declare AuthHandler
class AuthHandler;

///@brief return_code,response_body pair
using HttpResult = std::pair<long, std::string>;

/**
 * @brief Concept to check T's constructability from pqxx::row
 * @tparam T
 */
template<typename T>
concept HasJsonConstructor = std::is_constructible_v<T, Json::Value &>;

/**
 * @brief Parses specified continuous struct from Json::Value
 * @note Requires RetType to be constructible from Json::Value
 * @tparam VecT
 * @tparam RetType
 * @param res
 * @return VecT<RetType>
 */
template<template<typename ...> typename VecT, typename RetType>
requires HasJsonConstructor<RetType> && HasReserve<VecT<RetType>>
VecT<RetType> ParseArray(const Json::Value &json);

/**
 * @brief Default curl write callback
 * @param contents
 * @param size
 * @param nmemb
 * @param userp
 */
static size_t WriteCallback(void *contents,
                            size_t size,
                            size_t nmemb,
                            void *userp);

/**
 * @brief Common function to perform http request uning curl
 * @param path
 * @param method
 * @param host
 * @param auth_handler
 * @param data
 * @throws shared::CurlError -  if curl error occurs
 * @throws shared::HttpError - if curl results in return code >=400
 * @return HttpResult
 */
HttpResult PerformCurlRequest(const std::string &path,
                              const std::string &method,
                              const std::string &host,
                              AuthHandler *auth_handler,
                              const std::string &data = "");

/**
 * @brief Lock like class to handler curl initialization and free
 */
class CurlWrapper {
 public:
  CurlWrapper();

  operator bool();

  [[nodiscard]] CURL *Get();

 private:
  std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> curl_;

};

/**
 * @brief Common interface for handling authentication
 * @details Classes that inherit must provide implementation
 * for AddAuth.
 */
class AuthHandler {
 public:
  AuthHandler(bool active = true);

  void SetActive(bool act);

  void AddAuthorization(CURL *curl);

 protected:

  /// Can toggle off authorization
  bool active_;

  virtual void AddAuth(CURL *curl) = 0;
};

/**
 * @brief Used for Basic auth
 */
class BasicAuthHandler : public AuthHandler {

 public:
  BasicAuthHandler(const std::string &user,
                   const std::string &password,
                   bool active = true);

  BasicAuthHandler(const BasicAuthHandler &handle) = default;

 protected:
  std::string user_;
  std::string password_;

  virtual void AddAuth(CURL *curl) override;
};

class JsonAuthHandler : public BasicAuthHandler {
  using BasicAuthHandler::BasicAuthHandler;
 public:
  Json::Value ToJson();

  std::pair<std::string, std::string> Retrive();
};

class HttpRequestService
{
 public:
  HttpRequestService();
  /**
   * @brief Same as the default constructor but set's the provided parameters.
   * @param base_url
   * @param auth_handler
   * @see HttpRequestService::RabbitMQRestService()
   */
  HttpRequestService(const std::string &base_url,
                     AuthHandler *auth_handler);
  HttpRequestService(const HttpRequestService &other) = delete;

  HttpRequestService &operator=(const HttpRequestService &other) = delete;

  /**
   * @brief Sets url's for reuqests
   * @param base_url
   */
  void SetBaseUrl(const std::string &base_url);
  /**
   * @brief Set's url and auth provider
   * @param base_url
   * @param auth_handler
   */
  void SetParams(const std::string &base_url,
                 AuthHandler *auth_handler=nullptr);
  /**
   * @brief Clean up curl
   */
  ~HttpRequestService();
  /**
   * @brief Wrapper to perform curl request
   * @brief Wraps around rest_utils::PerformCurlRequest()
   * Thrown anything this function throws.
   * @param path
   * @param method
   * @param data
   * @return HttpResult
   */
  HttpResult PerformRequest(const std::string &path,
                            const std::string &method,
                            const std::string &data = "");
  /**
   * @brief Wrapper to perform curl request
   * @brief Wraps around rest_utils::PerformCurlRequest()
   * Thrown anything this function throws.
   * @param path
   * @param method
   * @param data
   * @return HttpResult
   */
  HttpResult PerformRequest(const std::string &path,
                            const HttpMethod &method,
                            const std::string &data = "");

  /**
   * @brief Shorthand to retrieve JSON object
   * @param json_str
   * @todo move outside
   * @return
   */
  static Json::Value ParseJson(const std::string &json_str);

 protected:
  std::string base_url_; ///< url to perform request

  AuthHandler *auth_ptr_;///< auth handler
};
}

namespace rest_utils {
template<template<typename ...> typename VecT, typename RetType>
requires HasJsonConstructor<RetType> && HasReserve<VecT<RetType>>
VecT<RetType> ParseArray(const Json::Value &res) {
  VecT<RetType> vec;
  if (res.isArray()) {
    vec.reserve(res.size());
    for (auto &re : res) {
      vec.emplace_back(re);
    }
  }
  return vec;
}
}
