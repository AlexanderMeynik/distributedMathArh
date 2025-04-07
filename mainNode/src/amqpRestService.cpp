#include "amqpRestService.h"
#include <stdexcept>
#include <sstream>

#include "common/errorHandling.h"

namespace amqpCommon {

    RabbitMQRestService::RabbitMQRestService(const std::string& baseUrl,
                                             authHandler*authHandler)
            :baseUrl(baseUrl),
            m_authPtr(authHandler){
        curl_global_init(CURL_GLOBAL_DEFAULT);
    }

    RabbitMQRestService::~RabbitMQRestService() {
        curl_global_cleanup();
    }


    std::string RabbitMQRestService::performRequest(const std::string& path,
                                                    const std::string& method,
                                                    const std::string& data) {
        return performCurlRequest(path,method,baseUrl,m_authPtr,data);
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

}