#include "amqpRestService.h"
#include <stdexcept>
#include <sstream>

#include "common/errorHandling.h"

namespace amqpCommon {

    RabbitMQRestService::RabbitMQRestService(const std::string& baseUrl,
                                             const std::string& username,
                                             const std::string& password)
            : baseUrl(baseUrl),
            username(username),
            password(password){
        curl_global_init(CURL_GLOBAL_DEFAULT);
    }

    RabbitMQRestService::~RabbitMQRestService() {
        curl_global_cleanup();
    }

    size_t WriteCallback(void *contents,
                         size_t size,
                         size_t nmemb,
                         void *userp) {
        size_t realsize = size * nmemb;
        std::string *buffer = static_cast<std::string*>(userp);
        buffer->append(static_cast<char*>(contents), realsize);
        return realsize;
    }

    std::string RabbitMQRestService::performRequest(const std::string& path,
                                                    const std::string& method,
                                                    const std::string& data,
                                                    bool auth) {
        return performCurlRequest(path,method,baseUrl,username,password,data,auth);
    }

    Json::Value RabbitMQRestService::parseJson(const std::string& jsonStr) {
        Json::Value root;
        Json::CharReaderBuilder builder;
        std::string errs;
        std::istringstream iss(jsonStr);
        if (!Json::parseFromStream(builder, iss, &root, &errs)) {
            throw std::runtime_error("JSON parse error: " + errs);
        }
        return root;
    }

    bool RabbitMQRestService::createQueue(const std::string& vhost,
                                          const std::string& queueName,
                                          const Json::Value& arguments) {
        std::string path = "/api/queues/" + vhost + "/" + queueName;
        Json::Value body;
        body["auto_delete"] = false;
        body["durable"] = true;
        body["arguments"] = arguments;
        std::string data = Json::writeString(Json::StreamWriterBuilder(), body);
        performRequest(path, "PUT", data);
        return true;
    }

    bool RabbitMQRestService::deleteQueue(const std::string& vhost,
                                          const std::string& queueName) {
        std::string path = "/api/queues/" + vhost + "/" + queueName;
        performRequest(path, "DELETE");
        return true;
    }



    Json::Value RabbitMQRestService::getQueueStats(const std::string& vhost,
                                                   const std::string& queueName) {
        std::string path = "/api/queues/" + vhost + "/" + queueName;
        std::string response = performRequest(path, "GET");
        return parseJson(response);
    }

    std::vector<std::string> RabbitMQRestService::listQueues(const std::string& vhost) {
        std::string path = "/api/queues/" + vhost;
        std::string response = performRequest(path, "GET");
        Json::Value j = parseJson(response);
        std::vector<std::string> queues;
        for (const auto& item : j) {
            queues.push_back(item["name"].asString());
        }
        return queues;
    }

    bool RabbitMQRestService::bindQueueToExchange(const std::string& vhost,
                                                  const std::string& queueName,
                                                  const std::string& exchangeName,
                                                  const std::string& routingKey) {
        std::string path = "/api/bindings/" + vhost + "/e/" + exchangeName + "/q/" + queueName;
        Json::Value body;
        body["routing_key"] = routingKey;
        std::string data = Json::writeString(Json::StreamWriterBuilder(), body);
        performRequest(path, "POST", data);
        return true;
    }

    bool RabbitMQRestService::unbindQueueFromExchange(const std::string& vhost,
                                                      const std::string& queueName,
                                                      const std::string& exchangeName,
                                                      const std::string& routingKey) {
        std::string path = "/api/bindings/" + vhost + "/e/" + exchangeName + "/q/" + queueName + "/" + routingKey;
        performRequest(path, "DELETE");
        return true;
    }

    bool RabbitMQRestService::createUser(const std::string& user,
                                         const std::string& pass) {
        std::string path = "/api/users/" + user;
        Json::Value body;
        body["password"] = pass;
        body["tags"] = "worker";
        std::string data = Json::writeString(Json::StreamWriterBuilder(), body);
        performRequest(path, "PUT", data);
        return true;
    }

    bool RabbitMQRestService::deleteUser(const std::string& user) {
        std::string path = "/api/users/" + user;
        performRequest(path, "DELETE");
        return true;
    }

    Json::Value RabbitMQRestService::whoami() {

        std::string path = "/api/whoami";

        std::string response = performRequest(path, "GET");

        return parseJson(response);
    }


    std::string
    performCurlRequest(const std::string &path,
                                            const std::string &method,
                                            const std::string &host,
                                            const std::string &user,
                                            const std::string &password,
                                            const std::string &data,
                                            bool auth) {
        CURL *curl = curl_easy_init();
        if (!curl) {
            throw shared::MyException("Failed to initialize CURL");//todo
        }

        std::string fullUrl = host + path;
        curl_easy_setopt(curl, CURLOPT_URL, fullUrl.c_str());

        if(auth) {
            curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
            curl_easy_setopt(curl, CURLOPT_USERNAME, user.c_str());
            curl_easy_setopt(curl, CURLOPT_PASSWORD, password.c_str());
        }

        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method.c_str());

        struct curl_slist *headers = nullptr;
        if (!data.empty()) {
            headers = curl_slist_append(headers, "Content-Type: application/json");
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
            curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data.size());
        }

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        std::string responseBody;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBody);

        CURLcode res = curl_easy_perform(curl);

        long httpCode = 0;
        if (res == CURLE_OK) {
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            throw shared::curlError(std::string(curl_easy_strerror(res)));
        }

        if (httpCode >= 400) {
            throw shared::httpError(httpCode);
        }

        return responseBody;
    }
}