#pragma once

#include <string>
#include <memory>

#include <json/json.h>
#include <curl/curl.h>
#include <unordered_map>

#include "common/errorHandling.h"
/// Namespace for httpr rest utilities
namespace rest_utils {
class AuthHandler;

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
 * @return response_body
 */
std::string PerformCurlRequest(const std::string &path,
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
 *
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
  Json::Value ToJson() {
    Json::Value res;
    res["user"] = user_;
    res["password"] = password_;
    return res;
  }

  std::pair<std::string, std::string> Retrive() {
    return {user_, password_};
  }
};
}
