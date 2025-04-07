
#pragma once
#ifndef DISTRIBUTED_MATH_ARH_AMQPRESTSERVICE_H
#define DISTRIBUTED_MATH_ARH_AMQPRESTSERVICE_H

#include <vector>
#include <string>

#include <json/json.h>

#include "restUtils.h"

///amqpCommon namespace
namespace amqpCommon {


    class RabbitMQRestService {
    public:
        RabbitMQRestService(const std::string &baseUrl,
                            authHandler *authHandler);

        ~RabbitMQRestService();

        Json::Value whoami();

        bool createQueue(const std::string &vhost,
                         const std::string &queueName,
                         const Json::Value &arguments);

        bool deleteQueue(const std::string &vhost,
                         const std::string &queueName);

        Json::Value getQueueStats(const std::string &vhost,
                                  const std::string &queueName);

        std::vector<std::string> listQueues(const std::string &vhost);

        bool bindQueueToExchange(const std::string &vhost,
                                 const std::string &queueName,
                                 const std::string &exchangeName,
                                 const std::string &routingKey);

        bool unbindQueueFromExchange(const std::string &vhost,
                                     const std::string &queueName,
                                     const std::string &exchangeName,
                                     const std::string &routingKey);

        bool createUser(const std::string &username,
                        const std::string &password);

        bool deleteUser(const std::string &username);

    private:
        std::string baseUrl;
        std::string username;
        std::string password;

        authHandler *m_authPtr;

        std::string performRequest(const std::string &path,
                                   const std::string &method,
                                   const std::string &data = "");

        static Json::Value parseJson(const std::string &jsonStr);
    };
}
#endif //DISTRIBUTED_MATH_ARH_AMQPRESTSERVICE_H
