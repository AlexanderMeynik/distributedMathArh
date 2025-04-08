#pragma once

#include <string>
#include <memory>

#include <curl/curl.h>

#include "common/errorHandling.h"

class authHandler;

/**
 * @brief Default curl write callback
 * @param contents
 * @param size
 * @param nmemb
 * @param userp
 */
static size_t writeCallback(void *contents,
                            size_t size,
                            size_t nmemb,
                            void *userp);

/**
 * @brief Common function to perform http request uning curl
 * @param path
 * @param method
 * @param host
 * @param authHandler
 * @param data
 */
std::string performCurlRequest(const std::string &path,
                               const std::string &method,
                               const std::string &host,
                               authHandler *authHandler,
                               const std::string &data = "");

/**
 * @brief Lock like class to handler curl initializtion and free
 */
class curlWrapper {
public:
    curlWrapper();

    operator bool();

    [[nodiscard]] CURL *get();

private:
    std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> m_curl;

};

/**
 * @brief Common interface for handling authentication
 * @details Classes that inherit must provide implementation
 * for addAuth.
 *
 */
class authHandler {
public:
    authHandler(bool active = true);

    void setActive(bool act);

    void addAuthorization(CURL *curl);

protected:

    /// Can toggle off authorization
    bool m_active;

    virtual void addAuth(CURL *curl) = 0;
};


/**
 * @brief Used for Basic auth
 */
class basicAuthHandler : public authHandler {

public:
    basicAuthHandler(const std::string &user,
                     const std::string &password,
                     bool active = true);

    basicAuthHandler(const basicAuthHandler &hndl) = default;

protected:
    std::string m_user;
    std::string m_password;

    virtual void addAuth(CURL *curl) override;
};

