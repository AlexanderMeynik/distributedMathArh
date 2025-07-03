#include "network_shared/restUtils.h"
/// Namespace for httpr rest utilities
namespace rest_utils {
size_t WriteCallback(void *contents,
                     size_t size,
                     size_t nmemb,
                     void *userp) {
  size_t realsize = size * nmemb;
  auto *buffer = static_cast<std::string *>(userp);
  buffer->append(static_cast<char *>(contents), realsize);
  return realsize;
}

HttpResult
PerformCurlRequest(const std::string &path,
                   const std::string &method,
                   const std::string &host,
                   AuthHandler *auth_handler,
                   const std::string &data) {

  CurlWrapper curl_wrapper;
  if (!curl_wrapper.Get()) {
    throw shared::CurlError("Failed to initialize CURL");
  }

  std::string full_url = host + path;
  curl_easy_setopt(curl_wrapper.Get(), CURLOPT_URL, full_url.c_str());

  if(auth_handler) {
    auth_handler->AddAuthorization(curl_wrapper.Get());
  }

  curl_easy_setopt(curl_wrapper.Get(), CURLOPT_CUSTOMREQUEST, method.c_str());

  struct curl_slist *headers = nullptr;
  if (!data.empty()) {
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl_wrapper.Get(), CURLOPT_POSTFIELDS, data.c_str());
    curl_easy_setopt(curl_wrapper.Get(), CURLOPT_POSTFIELDSIZE, data.size());
  }

  curl_easy_setopt(curl_wrapper.Get(), CURLOPT_HTTPHEADER, headers);

  std::string response_body;
  curl_easy_setopt(curl_wrapper.Get(), CURLOPT_WRITEFUNCTION, WriteCallback);
  curl_easy_setopt(curl_wrapper.Get(), CURLOPT_WRITEDATA, &response_body);

  CURLcode res = curl_easy_perform(curl_wrapper.Get());

  long http_code = 0;
  if (res == CURLE_OK) {
    curl_easy_getinfo(curl_wrapper.Get(), CURLINFO_RESPONSE_CODE, &http_code);
  }

  curl_slist_free_all(headers);

  if (res != CURLE_OK) {
    throw shared::CurlError(std::string(curl_easy_strerror(res)));
  }

  if (http_code >= 400) {
    throw shared::HttpError(http_code, response_body);
  }

  return {http_code, std::move(response_body)};
}

CurlWrapper::CurlWrapper() : curl_(curl_easy_init(), &curl_easy_cleanup) {
  if (!curl_)
    throw std::runtime_error("CURL initialization failed");
}

CurlWrapper::operator bool() {
  return curl_.get();
}

CURL *CurlWrapper::Get() {
  return curl_.get();
}

void AuthHandler::SetActive(bool act) {
  active_ = act;
}

AuthHandler::AuthHandler(bool active) : active_(active) {}

void AuthHandler::AddAuthorization(CURL *curl) {
  if (active_) {
    AddAuth(curl);
  }
}

BasicAuthHandler::BasicAuthHandler(const std::string &user, const std::string &password, bool active) :
    AuthHandler(active),
    user_(user),
    password_(password) {}

void BasicAuthHandler::AddAuth(CURL *curl) {
  if (!curl) {
    throw shared::CurlError("Curl object in null");
  }
  if (active_) {
    curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    curl_easy_setopt(curl, CURLOPT_USERNAME, user_.c_str());
    curl_easy_setopt(curl, CURLOPT_PASSWORD, password_.c_str());
  }
}
Json::Value JsonAuthHandler::ToJson() {
  Json::Value res;
  res["user"] = user_;
  res["password"] = password_;
  return res;
}
std::pair<std::string, std::string> JsonAuthHandler::Retrive() {
  return {user_, password_};
}

HttpRequestService::HttpRequestService() {
  curl_global_init(CURL_GLOBAL_DEFAULT);
}
HttpRequestService::HttpRequestService(const std::string &base_url, AuthHandler *auth_handler) :
    base_url_(base_url),
    auth_ptr_(auth_handler) {

  curl_global_init(CURL_GLOBAL_DEFAULT);
}

void HttpRequestService::SetBaseUrl(const std::string &base_url) {
  base_url_ = base_url;
}

void HttpRequestService::SetParams(const std::string &base_url, AuthHandler *auth_handler) {
  base_url_ = base_url;
  auth_ptr_ = auth_handler;
}

HttpRequestService::~HttpRequestService() {
  curl_global_cleanup();
}

HttpResult HttpRequestService::PerformRequest(const std::string &path,
                                              const std::string &method,
                                              const std::string &data) {
  return PerformCurlRequest(path, method, base_url_, auth_ptr_, data);
}

HttpResult HttpRequestService::PerformRequest(const std::string &path,
                                              const HttpMethod &method,
                                              const std::string &data) {
  return PerformRequest(path, ToString(method), data);
}

Json::Value HttpRequestService::ParseJson(const std::string &json_str) {
  Json::Value root;
  Json::CharReaderBuilder builder;
  std::string errs;
  std::istringstream iss(json_str);
  if (!Json::parseFromStream(builder, iss, &root, &errs)) {
    throw std::runtime_error("JSON parse error: " + errs);
  }
  return root;
}
}
