
#pragma once
#ifndef DATA_DEDUPLICATION_SERVICE_AMQPRESTSERVICE_H
#define DATA_DEDUPLICATION_SERVICE_AMQPRESTSERVICE_H

#include <vector>
#include <string>

#include <curl/curl.h>
#include <json/json.h>

///amqpCommon namespace
namespace amqpCommon {

    class RabbitMQRestService {
    public:
        RabbitMQRestService(const std::string& baseUrl,
                            const std::string& username,
                            const std::string& password);
        ~RabbitMQRestService();


        bool createQueue(const std::string& vhost,
                         const std::string& queueName,
                         const Json::Value& arguments);
        bool deleteQueue(const std::string& vhost,
                         const std::string& queueName);
        bool sendStartEventLoopRequest(const std::string& workerId,
                                       const std::string& queueName);


        Json::Value getQueueStats(const std::string& vhost,
                                  const std::string& queueName);
        std::vector<std::string> listQueues(const std::string& vhost);


        bool bindQueueToExchange(const std::string& vhost,
                                 const std::string& queueName,
                                 const std::string& exchangeName,
                                 const std::string& routingKey);
        bool unbindQueueFromExchange(const std::string& vhost,
                                     const std::string& queueName,
                                     const std::string& exchangeName,
                                     const std::string& routingKey);
        bool createUser(const std::string& username,
                        const std::string& password);
        bool deleteUser(const std::string& username);

    private:
        std::string baseUrl;
        std::string authHeader;
        CURL* curl;

        static size_t writeCallback(void* contents,
                                    size_t size, size_t nmemb,
                                    std::string* userp);
        std::string performRequest(const std::string& url,
                                   const std::string& method,
                                   const std::string& data = "");
        Json::Value parseJson(const std::string& jsonStr);
    };
}
#endif //DATA_DEDUPLICATION_SERVICE_AMQPRESTSERVICE_H
