#include "amqpRestService.h"
#include <stdexcept>
#include <sstream>

namespace amqpCommon {

    RabbitMQRestService::RabbitMQRestService(const std::string& baseUrl,
                                     const std::string& username,
                                     const std::string& password)
            : baseUrl(baseUrl) {
        curl = curl_easy_init();
        if (!curl) throw std::runtime_error("Failed to initialize libcurl");
        authHeader = "Authorization: Basic " + std::string(curl_easy_escape(curl, (username + ":" + password).c_str(), 0));
    }

    RabbitMQRestService::~RabbitMQRestService() {
        curl_easy_cleanup(curl);
    }

    size_t RabbitMQRestService::writeCallback(void* contents,
                                          size_t size, size_t nmemb,
                                          std::string* userp) {
        size_t realsize = size * nmemb;
        userp->append((char*)contents, realsize);
        return realsize;
    }

    std::string RabbitMQRestService::performRequest(const std::string& url,
                                                const std::string& method,
                                                const std::string& data) {
        std::string response;
        CURLcode res;

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        struct curl_slist* headers = curl_slist_append(nullptr, authHeader.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        if (method == "PUT") curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
        else if (method == "DELETE") curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
        else if (method == "POST") {
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
        }

        res = curl_easy_perform(curl);
        curl_slist_free_all(headers);

        if (res != CURLE_OK) throw std::runtime_error("Request failed: " + std::string(curl_easy_strerror(res)));

        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        if (http_code >= 400) throw std::runtime_error("HTTP error: " + std::to_string(http_code));

        return response;
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
        std::string url = baseUrl + "/api/queues/" + vhost + "/" + queueName;
        Json::Value body;
        body["auto_delete"] = false;
        body["durable"] = true;
        body["arguments"] = arguments;
        performRequest(url, "PUT", Json::writeString(Json::StreamWriterBuilder(), body));
        return true;
    }

    bool RabbitMQRestService::deleteQueue(const std::string& vhost,
                                      const std::string& queueName) {
        std::string url = baseUrl + "/api/queues/" + vhost + "/" + queueName;
        performRequest(url, "DELETE");
        return true;
    }

    bool RabbitMQRestService::sendStartEventLoopRequest(const std::string& workerId,
                                                    const std::string& queueName) {
        std::string url = baseUrl + "/api/exchanges/%2F/control/publish";
        Json::Value body;
        body["properties"] = Json::Value::null;
        body["routing_key"] = "start_event_loop";
        body["payload"] = "Start event loop for " + queueName + " by " + workerId;
        body["payload_encoding"] = "string";
        performRequest(url, "POST", Json::writeString(Json::StreamWriterBuilder(), body));
        return true;
    }

    Json::Value RabbitMQRestService::getQueueStats(const std::string& vhost,
                                               const std::string& queueName) {
        std::string url = baseUrl + "/api/queues/" + vhost + "/" + queueName;
        std::string response = performRequest(url, "GET");
        return parseJson(response);
    }

    std::vector<std::string> RabbitMQRestService::listQueues(const std::string& vhost) {
        std::string url = baseUrl + "/api/queues/" + vhost;
        std::string response = performRequest(url, "GET");
        Json::Value j = parseJson(response);
        std::vector<std::string> queues;
        for (const auto& item : j) {
            queues.push_back(item["name"].asString());
        }
        return queues;
    }

    bool RabbitMQRestService::bindQueueToExchange(const std::string& vhost,
                                              const std::string& queueName,
                                              const std::string& exchangeName, const std::string& routingKey) {
        std::string url = baseUrl + "/api/bindings/" + vhost + "/e/" + exchangeName + "/q/" + queueName;
        Json::Value body;
        body["routing_key"] = routingKey;
        performRequest(url, "POST", Json::writeString(Json::StreamWriterBuilder(), body));
        return true;
    }

    bool RabbitMQRestService::unbindQueueFromExchange(const std::string& vhost,
                                                  const std::string& queueName,
                                                  const std::string& exchangeName,
                                                  const std::string& routingKey) {
        std::string url = baseUrl + "/api/bindings/" + vhost + "/e/" + exchangeName + "/q/" + queueName + "/" + routingKey;
        performRequest(url, "DELETE");
        return true;
    }

    bool RabbitMQRestService::createUser(const std::string& username,
                                     const std::string& password) {
        std::string url = baseUrl + "/api/users/" + username;
        Json::Value body;
        body["password"] = password;
        body["tags"] = "worker";
        performRequest(url, "PUT", Json::writeString(Json::StreamWriterBuilder(), body));
        return true;
    }

    bool RabbitMQRestService::deleteUser(const std::string& username) {
        std::string url = baseUrl + "/api/users/" + username;
        performRequest(url, "DELETE");
        return true;
    }
}
