#include "amqpRestService.h"
#include <stdexcept>
#include <sstream>

#include <drogon/HttpRequest.h>

namespace amqpCommon {

    static std::string base64_encode(const std::string& input) {
        const std::string base64_chars =
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "abcdefghijklmnopqrstuvwxyz"
                "0123456789+/";
        std::string encoded;
        int i = 0;
        int j = 0;
        uint8_t char_array_3[3];
        uint8_t char_array_4[4];

        for (char c : input) {
            char_array_3[i++] = c;
            if (i == 3) {
                char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
                char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
                char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
                char_array_4[3] = char_array_3[2] & 0x3f;
                for (i = 0; i < 4; i++) {
                    encoded += base64_chars[char_array_4[i]];
                }
                i = 0;
            }
        }

        if (i) {
            for (j = i; j < 3; j++) {
                char_array_3[j] = '\0';
            }
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;
            for (j = 0; j < i + 1; j++) {
                encoded += base64_chars[char_array_4[j]];
            }
            while (i++ < 3) {
                encoded += '=';
            }
        }
        return encoded;
    }

    RabbitMQRestService::RabbitMQRestService(const std::string& baseUrl,
                                             const std::string& username,
                                             const std::string& password)
            : baseUrl(baseUrl) {
        authHeader = "Basic " + base64_encode(username + ":" + password);
        httpClient = drogon::HttpClient::newHttpClient(baseUrl);
    }

    RabbitMQRestService::~RabbitMQRestService() {
    }

    std::string RabbitMQRestService::performRequest(const std::string& path,
                                                    const std::string& method,
                                                    const std::string& data) {
        auto req = drogon::HttpRequest::newHttpRequest();

        req->setPath(path);

        if(!methodList.count(method))
        {
            throw std::invalid_argument("Unsupported HTTP method: " + method);
        }

        req->setMethod(methodList.at(method));

        std::cout<<req->methodString()<<'\t';
        std::string fullUrl = baseUrl + path;
        std::cout << "Requesting: " << fullUrl << '\n';


        req->addHeader("Authorization", authHeader);
        if (!data.empty()) {
            req->setBody(data);
        }
        auto [result, resp] = httpClient->sendRequest(req);
        if (result != drogon::ReqResult::Ok) {
            throw std::runtime_error("Request failed");
        }
        if (resp->getStatusCode() >= 400) {
            throw std::runtime_error("HTTP error: " + std::to_string(resp->getStatusCode()));
        }
        return std::string(resp->getBody());
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

    bool RabbitMQRestService::sendStartEventLoopRequest(const std::string& workerId,
                                                        const std::string& queueName) {
        std::string path = "/api/exchanges/%2F/control/publish";
        Json::Value body;
        body["properties"] = Json::Value::null;
        body["routing_key"] = "start_event_loop";
        body["payload"] = "Start event loop for " + queueName + " by " + workerId;
        body["payload_encoding"] = "string";
        std::string data = Json::writeString(Json::StreamWriterBuilder(), body);
        performRequest(path, "POST", data);
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

    bool RabbitMQRestService::createUser(const std::string& username,
                                         const std::string& password) {
        std::string path = "/api/users/" + username;
        Json::Value body;
        body["password"] = password;
        body["tags"] = "worker";
        std::string data = Json::writeString(Json::StreamWriterBuilder(), body);
        performRequest(path, "PUT", data);
        return true;
    }

    bool RabbitMQRestService::deleteUser(const std::string& username) {
        std::string path = "/api/users/" + username;
        performRequest(path, "DELETE");
        return true;
    }
}
