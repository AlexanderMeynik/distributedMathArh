#include "restUtils.h"


size_t writeCallback(void *contents,
                     size_t size,
                     size_t nmemb,
                     void *userp) {
    size_t realsize = size * nmemb;
    auto *buffer = static_cast<std::string *>(userp);
    buffer->append(static_cast<char *>(contents), realsize);
    return realsize;
}

std::string
performCurlRequest(const std::string &path,
                   const std::string &method,
                   const std::string &host,
                   authHandler *authHandler,
                   const std::string &data) {

    curlWrapper curlWrapper;
    if (!curlWrapper.get()) {
        throw shared::curlError("Failed to initialize CURL");
    }

    std::string fullUrl = host + path;
    curl_easy_setopt(curlWrapper.get(), CURLOPT_URL, fullUrl.c_str());

    authHandler->addAuthorization(curlWrapper.get());


    curl_easy_setopt(curlWrapper.get(), CURLOPT_CUSTOMREQUEST, method.c_str());

    struct curl_slist *headers = nullptr;
    if (!data.empty()) {
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curlWrapper.get(), CURLOPT_POSTFIELDS, data.c_str());
        curl_easy_setopt(curlWrapper.get(), CURLOPT_POSTFIELDSIZE, data.size());
    }

    curl_easy_setopt(curlWrapper.get(), CURLOPT_HTTPHEADER, headers);

    std::string responseBody;
    curl_easy_setopt(curlWrapper.get(), CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curlWrapper.get(), CURLOPT_WRITEDATA, &responseBody);

    CURLcode res = curl_easy_perform(curlWrapper.get());

    long httpCode = 0;
    if (res == CURLE_OK) {
        curl_easy_getinfo(curlWrapper.get(), CURLINFO_RESPONSE_CODE, &httpCode);
    }

    curl_slist_free_all(headers);

    if (res != CURLE_OK) {
        throw shared::curlError(std::string(curl_easy_strerror(res)));
    }

    if (httpCode >= 400) {
        throw shared::httpError(httpCode);
    }

    return responseBody;
}

curlWrapper::curlWrapper() : m_curl(curl_easy_init(), &curl_easy_cleanup) {
    if (!m_curl)
        throw std::runtime_error("CURL initialization failed");
}

curlWrapper::operator bool() {
    return m_curl.get();
}

CURL *curlWrapper::get() {
    return m_curl.get();
}

void authHandler::setActive(bool act) {
    m_active = act;
}

authHandler::authHandler(bool active):m_active(active) {}

void authHandler::addAuthorization(CURL *curl) {
    if(m_active)
    {
        addAuth(curl);
    }
}

basicAuthHandler::basicAuthHandler(const std::string &user, const std::string &password, bool active):
        authHandler(active),
        m_user (user),
        m_password(password)
{}

void basicAuthHandler::addAuth(CURL *curl) {
    if(!curl)
    {
        throw shared::curlError("Curl object in null");
    }
    if (m_active) {
        curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
        curl_easy_setopt(curl, CURLOPT_USERNAME, m_user.c_str());
        curl_easy_setopt(curl, CURLOPT_PASSWORD, m_password.c_str());
    }
}


